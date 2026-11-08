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

#include "cv-cuda-pipeline/calculators/image_process/cuda/color_twist_kernel.h"

#include <cuda_runtime.h>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <vector>

namespace {
void Check(cudaError_t s) {
  if (s != cudaSuccess) throw std::runtime_error(cudaGetErrorString(s));
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
void Test(int width, int height, cv_cuda_pipeline::ColorTwistMatrix matrix) {
  const std::size_t row = static_cast<std::size_t>(width) * 3;
  const auto src_stride = row + 5, dst_stride = row + 7;
  std::vector<std::uint8_t> src(src_stride * height, 0x5a);
  std::vector<std::uint8_t> dst(dst_stride * height, 0xa5);
  for (int y = 0; y < height; ++y)
    for (int x = 0; x < width; ++x)
      for (int c = 0; c < 3; ++c)
        src[y * src_stride + x * 3 + c] = (x + y * 61 + c * 83) % 256;
  Buffer in(src.size()), out(dst.size());
  cudaStream_t stream;
  Check(cudaStreamCreate(&stream));
  Check(cudaMemcpyAsync(in.get(), src.data(), src.size(),
                        cudaMemcpyHostToDevice, stream));
  Check(cudaMemcpyAsync(out.get(), dst.data(), dst.size(),
                        cudaMemcpyHostToDevice, stream));
  cv_cuda_pipeline::ColorTwistImageView view{in.get(),   src_stride, out.get(),
                                             dst_stride, width,      height};
  Check(cv_cuda_pipeline::ColorTwist(view, matrix, stream));
  Check(cudaMemcpyAsync(dst.data(), out.get(), dst.size(),
                        cudaMemcpyDeviceToHost, stream));
  Check(cudaStreamSynchronize(stream));
  Check(cudaStreamDestroy(stream));
  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      const auto* p = &src[y * src_stride + x * 3];
      for (int c = 0; c < 3; ++c) {
        const auto* m = matrix.rows[c];
        const float value =
            std::fma(m[2], static_cast<float>(p[2]),
                     std::fma(m[1], static_cast<float>(p[1]),
                              std::fma(m[0], static_cast<float>(p[0]), m[3])));
        const int expected =
            static_cast<int>(std::nearbyint(std::clamp(value, 0.f, 255.f)));
        Require(dst[y * dst_stride + x * 3 + c] == expected, "Pixel mismatch");
      }
    }
    for (std::size_t x = row; x < dst_stride; ++x)
      Require(dst[y * dst_stride + x] == 0xa5, "Output padding modified");
  }
  auto invalid = view;
  invalid.src = nullptr;
  Require(
      cv_cuda_pipeline::ColorTwist(invalid, matrix) == cudaErrorInvalidValue,
      "Null input accepted");
  invalid = view;
  invalid.dst = nullptr;
  Require(
      cv_cuda_pipeline::ColorTwist(invalid, matrix) == cudaErrorInvalidValue,
      "Null output accepted");
  invalid = view;
  invalid.dst = in.get();
  Require(
      cv_cuda_pipeline::ColorTwist(invalid, matrix) == cudaErrorInvalidValue,
      "In-place image accepted");
  invalid = view;
  invalid.width = 0;
  Require(
      cv_cuda_pipeline::ColorTwist(invalid, matrix) == cudaErrorInvalidValue,
      "Zero width accepted");
  invalid = view;
  invalid.height = -1;
  Require(
      cv_cuda_pipeline::ColorTwist(invalid, matrix) == cudaErrorInvalidValue,
      "Negative height accepted");
  invalid = view;
  invalid.src_stride = row - 1;
  Require(
      cv_cuda_pipeline::ColorTwist(invalid, matrix) == cudaErrorInvalidValue,
      "Short input stride accepted");
  invalid = view;
  invalid.dst_stride = row - 1;
  Require(
      cv_cuda_pipeline::ColorTwist(invalid, matrix) == cudaErrorInvalidValue,
      "Short output stride accepted");
  invalid = view;
  invalid.height = 2;
  invalid.src_stride = std::numeric_limits<std::size_t>::max();
  Require(
      cv_cuda_pipeline::ColorTwist(invalid, matrix) == cudaErrorInvalidValue,
      "Input stride overflow accepted");
  invalid = view;
  invalid.height = 2;
  invalid.dst_stride = std::numeric_limits<std::size_t>::max();
  Require(
      cv_cuda_pipeline::ColorTwist(invalid, matrix) == cudaErrorInvalidValue,
      "Output stride overflow accepted");
  auto invalid_matrix = matrix;
  invalid_matrix.rows[0][0] = std::numeric_limits<float>::quiet_NaN();
  Require(cv_cuda_pipeline::ColorTwist(view, invalid_matrix) ==
              cudaErrorInvalidValue,
          "NaN accepted");
  invalid_matrix.rows[0][0] = std::numeric_limits<float>::infinity();
  Require(cv_cuda_pipeline::ColorTwist(view, invalid_matrix) ==
              cudaErrorInvalidValue,
          "Infinity accepted");
  invalid_matrix.rows[0][0] = std::numeric_limits<float>::max();
  Require(cv_cuda_pipeline::ColorTwist(view, invalid_matrix) ==
              cudaErrorInvalidValue,
          "Unsafe matrix accepted");
}
}  // namespace
int main() {
  try {
    const cv_cuda_pipeline::ColorTwistMatrix warm{
        {{1.2f, 0, 0, 10}, {0, 1, 0, 0}, {0, 0, .8f, -10}}};
    const cv_cuda_pipeline::ColorTwistMatrix identity{
        {{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}}};
    const cv_cuda_pipeline::ColorTwistMatrix mix{{{.5f, .25f, -.75f, 120.5f},
                                                  {-1.f, 2.f, .5f, -30.f},
                                                  {0, 0, 0, 12.5f}}};
    Test(256, 1, warm);
    Test(1, 1, identity);
    Test(1, 257, mix);
    Test(31, 23, mix);
    Test(720, 720, warm);
    Test(2876, 1699, warm);
    std::cout << "Color twist tests passed\n";
    return 0;
  } catch (const std::exception& e) {
    std::cerr << "Color twist test failed: " << e.what() << '\n';
    return 1;
  }
}
