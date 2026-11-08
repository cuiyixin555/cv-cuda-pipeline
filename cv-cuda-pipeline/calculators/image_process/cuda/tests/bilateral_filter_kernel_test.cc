// SPDX-FileCopyrightText: Copyright (c) 2026 CUI Xin
// SPDX-License-Identifier: MIT

// MIT License

// Copyright (c) 2026 CUI Xin (崔 欣)

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "cv-cuda-pipeline/calculators/image_process/cuda/bilateral_filter_kernel.h"

#include <cuda_runtime.h>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <vector>

namespace {
void Check(cudaError_t status) {
  if (status != cudaSuccess) throw std::runtime_error(cudaGetErrorString(status));
}
void Require(bool condition, const char* message) {
  if (!condition) throw std::runtime_error(message);
}
class Buffer {
 public:
  explicit Buffer(std::size_t bytes) { Check(cudaMalloc(&data_, bytes)); }
  ~Buffer() { cudaFree(data_); }
  Buffer(const Buffer&) = delete;
  Buffer& operator=(const Buffer&) = delete;
  std::uint8_t* get() const { return static_cast<std::uint8_t*>(data_); }
 private:
  void* data_ = nullptr;
};
int Reflect(int p, int length) {
  while (p < 0 || p >= length) {
    if (p < 0) p = -p - 1;
    else p = 2 * length - 1 - p;
  }
  return p;
}
void Test(int width, int height, int pattern) {
  const std::size_t row = static_cast<std::size_t>(width) * 3;
  const std::size_t src_stride = row + 5, dst_stride = row + 7;
  std::vector<std::uint8_t> src(src_stride * height, 0x5a);
  std::vector<std::uint8_t> dst(dst_stride * height, 0xa5);
  for (int y = 0; y < height; ++y)
    for (int x = 0; x < width; ++x)
      for (int c = 0; c < 3; ++c)
        src[y * src_stride + x * 3 + c] =
            pattern == 0 ? 127 :
            pattern == 1 ? ((x == 0 && y == 0) ? 255 : 0) :
            pattern == 2 ? (x < width / 2 ? 15 : 240) :
            (x * 37 + y * 61 + c * 83) % 256;
  Buffer in(src.size()), out(dst.size());
  cudaStream_t stream;
  Check(cudaStreamCreate(&stream));
  Check(cudaMemcpyAsync(in.get(), src.data(), src.size(), cudaMemcpyHostToDevice, stream));
  Check(cudaMemcpyAsync(out.get(), dst.data(), dst.size(), cudaMemcpyHostToDevice, stream));
  cv_cuda_pipeline::BilateralFilterImageView view{in.get(), src_stride, out.get(),
                                                   dst_stride, width, height};
  Check(cv_cuda_pipeline::BilateralFilter(view, stream));
  Check(cudaMemcpyAsync(dst.data(), out.get(), dst.size(), cudaMemcpyDeviceToHost, stream));
  Check(cudaStreamSynchronize(stream));
  Check(cudaStreamDestroy(stream));
  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      const auto* center = src.data() + y * src_stride + x * 3;
      double sum[3] = {0, 0, 0}, total = 0;
      for (int dy = -4; dy <= 4; ++dy)
        for (int dx = -4; dx <= 4; ++dx) {
          const int distance = dx * dx + dy * dy;
          if (distance > 16) continue;
          const auto* p = src.data() + Reflect(y + dy, height) * src_stride +
                          Reflect(x + dx, width) * 3;
          int difference = 0;
          for (int c = 0; c < 3; ++c)
            difference += std::abs(static_cast<int>(p[c]) - center[c]);
          const double weight = std::exp(-(distance + difference * difference) / 11250.0);
          total += weight;
          for (int c = 0; c < 3; ++c) sum[c] += weight * p[c];
        }
      for (int c = 0; c < 3; ++c) {
        const int expected = static_cast<int>(std::nearbyint(sum[c] / total));
        Require(std::abs(static_cast<int>(dst[y * dst_stride + x * 3 + c]) - expected) <= 1,
                "Output differs from double-precision reference");
        if (pattern == 0)
          Require(dst[y * dst_stride + x * 3 + c] == 127, "Constant image changed");
        if (pattern == 2)
          Require(std::abs(static_cast<int>(dst[y * dst_stride + x * 3 + c]) - center[c]) <= 1,
                  "Sharp color edge not preserved");
      }
    }
    for (std::size_t x = row; x < dst_stride; ++x)
      Require(dst[y * dst_stride + x] == 0xa5, "Output padding modified");
  }
  auto invalid = view;
  invalid.src = nullptr;
  Require(cv_cuda_pipeline::BilateralFilter(invalid) == cudaErrorInvalidValue, "Null source accepted");
  invalid = view; invalid.dst = nullptr;
  Require(cv_cuda_pipeline::BilateralFilter(invalid) == cudaErrorInvalidValue, "Null destination accepted");
  invalid = view; invalid.width = 0;
  Require(cv_cuda_pipeline::BilateralFilter(invalid) == cudaErrorInvalidValue, "Zero width accepted");
  invalid = view; invalid.height = -1;
  Require(cv_cuda_pipeline::BilateralFilter(invalid) == cudaErrorInvalidValue, "Negative height accepted");
  invalid = view; invalid.src_stride = row - 1;
  Require(cv_cuda_pipeline::BilateralFilter(invalid) == cudaErrorInvalidValue, "Short input stride accepted");
  invalid = view; invalid.dst_stride = row - 1;
  Require(cv_cuda_pipeline::BilateralFilter(invalid) == cudaErrorInvalidValue, "Short output stride accepted");
  invalid = view; invalid.height = 2;
  invalid.src_stride = std::numeric_limits<std::size_t>::max();
  Require(cv_cuda_pipeline::BilateralFilter(invalid) == cudaErrorInvalidValue, "Input stride overflow accepted");
  invalid = view; invalid.height = 2;
  invalid.dst_stride = std::numeric_limits<std::size_t>::max();
  Require(cv_cuda_pipeline::BilateralFilter(invalid) == cudaErrorInvalidValue, "Output stride overflow accepted");
}
}  // namespace

int main() {
  try {
    for (int pattern = 0; pattern < 4; ++pattern) {
      Test(1, 1, pattern);
      Test(1, 2, pattern);
      Test(2, 1, pattern);
      Test(2, 3, pattern);
      Test(9, 9, pattern);
      Test(31, 23, pattern);
    }
    Test(257, 129, 3);
    std::cout << "Bilateral filter tests passed\n";
    return 0;
  } catch (const std::exception& e) {
    std::cerr << "Bilateral filter test failed: " << e.what() << '\n';
    return 1;
  }
}
