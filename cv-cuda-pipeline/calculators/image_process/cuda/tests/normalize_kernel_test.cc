// MIT License
//
// Copyright (c) 2026 CUI Xin (崔 欣)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "cv-cuda-pipeline/calculators/image_process/cuda/normalize_kernel.h"

#include <cuda_runtime.h>

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <vector>

namespace {
void Check(cudaError_t status) {
  if (status != cudaSuccess)
    throw std::runtime_error(cudaGetErrorString(status));
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
  void* get() const { return data_; }

 private:
  void* data_ = nullptr;
};
void Test(int width, int height) {
  const std::size_t row = static_cast<std::size_t>(width) * 3;
  const std::size_t src_stride = row + 5;
  const std::size_t dst_stride = row * sizeof(float) + 8;
  std::vector<std::uint8_t> src(src_stride * height, 0x5a);
  std::vector<std::uint8_t> dst(dst_stride * height, 0xa5);
  std::vector<float> expected(row * height);
  constexpr float means[] = {103.53f, 116.28f, 123.675f};
  constexpr float stddevs[] = {57.375f, 57.12f, 58.395f};
  for (int y = 0; y < height; ++y)
    for (int x = 0; x < width; ++x)
      for (int c = 0; c < 3; ++c) {
        const auto value =
            static_cast<std::uint8_t>((x * 37 + y * 61 + c * 83) % 256);
        src[y * src_stride + x * 3 + c] = value;
        const float variance = stddevs[c] * stddevs[c] + 1.e-5f;
        expected[(static_cast<std::size_t>(y) * width + x) * 3 + c] =
            (static_cast<float>(value) - means[c]) / std::sqrt(variance);
      }
  Buffer in(src.size()), out(dst.size());
  cudaStream_t stream;
  Check(cudaStreamCreate(&stream));
  Check(cudaMemcpyAsync(in.get(), src.data(), src.size(),
                        cudaMemcpyHostToDevice, stream));
  Check(cudaMemcpyAsync(out.get(), dst.data(), dst.size(),
                        cudaMemcpyHostToDevice, stream));
  cv_cuda_pipeline::NormalizeImageView view{
      static_cast<const std::uint8_t*>(in.get()),
      src_stride,
      static_cast<float*>(out.get()),
      dst_stride,
      width,
      height};
  Check(cv_cuda_pipeline::NormalizeImageNet(view, stream));
  Check(cudaMemcpy2DAsync(dst.data(), dst_stride, out.get(), dst_stride,
                          dst_stride, height, cudaMemcpyDeviceToHost, stream));
  Check(cudaStreamSynchronize(stream));
  Check(cudaStreamDestroy(stream));
  for (int y = 0; y < height; ++y) {
    const auto* actual =
        reinterpret_cast<const float*>(dst.data() + y * dst_stride);
    for (int x = 0; x < width * 3; ++x)
      Require(
          std::abs(actual[x] -
                   expected[(static_cast<std::size_t>(y) * width * 3) + x]) <
              2e-6f,
          "Normalized value mismatch");
    const auto* padding =
        reinterpret_cast<const std::uint8_t*>(actual) + row * sizeof(float);
    for (std::size_t x = 0; x < dst_stride - row * sizeof(float); ++x)
      Require(padding[x] == 0xa5, "Padding modified");
  }
  auto bad = view;
  bad.src = nullptr;
  Require(cv_cuda_pipeline::NormalizeImageNet(bad) == cudaErrorInvalidValue,
          "Null input accepted");
  bad = view;
  bad.dst = nullptr;
  Require(cv_cuda_pipeline::NormalizeImageNet(bad) == cudaErrorInvalidValue,
          "Null output accepted");
  bad = view;
  bad.width = 0;
  Require(cv_cuda_pipeline::NormalizeImageNet(bad) == cudaErrorInvalidValue,
          "Invalid width accepted");
  bad = view;
  bad.height = -1;
  Require(cv_cuda_pipeline::NormalizeImageNet(bad) == cudaErrorInvalidValue,
          "Invalid height accepted");
  bad = view;
  bad.src_stride = row - 1;
  Require(cv_cuda_pipeline::NormalizeImageNet(bad) == cudaErrorInvalidValue,
          "Short input stride accepted");
  bad = view;
  bad.dst_stride = row * sizeof(float) - 1;
  Require(cv_cuda_pipeline::NormalizeImageNet(bad) == cudaErrorInvalidValue,
          "Short output stride accepted");
  bad = view;
  bad.height = 2;
  bad.dst_stride = std::numeric_limits<std::size_t>::max();
  Require(cv_cuda_pipeline::NormalizeImageNet(bad) == cudaErrorInvalidValue,
          "Stride overflow accepted");
}
}  // namespace
int main() {
  try {
    Test(1, 1);
    Test(1, 257);
    Test(257, 1);
    Test(31, 23);
    Test(720, 720);
    Test(2876, 1699);
    std::cout << "Normalize tests passed\n";
    return 0;
  } catch (const std::exception& e) {
    std::cerr << "Normalize test failed: " << e.what() << '\n';
    return 1;
  }
}
