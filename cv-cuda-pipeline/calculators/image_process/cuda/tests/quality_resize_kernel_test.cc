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

#include "cv-cuda-pipeline/calculators/image_process/cuda/quality_resize_kernel.h"

#include <cuda_runtime.h>

#include <cstddef>
#include <cstdint>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
void Check(cudaError_t status, const char* operation) {
  if (status != cudaSuccess)
    throw std::runtime_error(std::string(operation) + ": " +
                             cudaGetErrorString(status));
}
void Require(bool condition, const char* message) {
  if (!condition) throw std::runtime_error(message);
}
class DeviceBuffer {
 public:
  explicit DeviceBuffer(std::size_t bytes) {
    Check(cudaMalloc(&data_, bytes), "cudaMalloc");
  }
  ~DeviceBuffer() { cudaFree(data_); }
  DeviceBuffer(const DeviceBuffer&) = delete;
  DeviceBuffer& operator=(const DeviceBuffer&) = delete;
  void* get() const { return data_; }

 private:
  void* data_ = nullptr;
};

void Test(cv_cuda_pipeline::QualityResizeMode mode) {
  constexpr int kWidth = 3, kHeight = 2, kOutputWidth = 7, kOutputHeight = 5;
  constexpr std::size_t kSrcStride = kWidth * 3 + 5;
  constexpr std::size_t kDstStride = kOutputWidth * 3 + 7;
  constexpr std::size_t kIntermediateStride =
      kOutputWidth * 3 * sizeof(float) + 8;
  const std::uint8_t kColor[3] = {71, 126, 203};
  std::vector<std::uint8_t> src(kSrcStride * kHeight, 0x5a);
  std::vector<std::uint8_t> dst(kDstStride * kOutputHeight, 0xa5);
  for (int y = 0; y < kHeight; ++y)
    for (int x = 0; x < kWidth; ++x)
      for (int c = 0; c < 3; ++c) src[y * kSrcStride + x * 3 + c] = kColor[c];

  DeviceBuffer device_src(src.size()), device_dst(dst.size());
  DeviceBuffer intermediate(kIntermediateStride * kHeight);
  Check(cudaMemcpy(device_src.get(), src.data(), src.size(),
                   cudaMemcpyHostToDevice),
        "Upload input");
  Check(cudaMemcpy(device_dst.get(), dst.data(), dst.size(),
                   cudaMemcpyHostToDevice),
        "Initialize output");
  cv_cuda_pipeline::QualityResizeImageView image{
      static_cast<const std::uint8_t*>(device_src.get()),
      kSrcStride,
      static_cast<float*>(intermediate.get()),
      kIntermediateStride,
      static_cast<std::uint8_t*>(device_dst.get()),
      kDstStride,
      kWidth,
      kHeight,
      kOutputWidth,
      kOutputHeight};
  Check(cv_cuda_pipeline::QualityResize(image, mode), "QualityResize");
  Check(cudaMemcpy(dst.data(), device_dst.get(), dst.size(),
                   cudaMemcpyDeviceToHost),
        "Download output");
  for (int y = 0; y < kOutputHeight; ++y) {
    for (int x = 0; x < kOutputWidth; ++x)
      for (int c = 0; c < 3; ++c)
        Require(dst[y * kDstStride + x * 3 + c] == kColor[c],
                "Constant image changed during resize");
    for (std::size_t x = kOutputWidth * 3; x < kDstStride; ++x)
      Require(dst[y * kDstStride + x] == 0xa5, "Output padding changed");
  }
  image.dst = nullptr;
  Require(cv_cuda_pipeline::QualityResize(image, mode) == cudaErrorInvalidValue,
          "Null destination was accepted");
}
}  // namespace

int main() {
  try {
    Test(cv_cuda_pipeline::QualityResizeMode::kLanczos);
    Test(cv_cuda_pipeline::QualityResizeMode::kLanczosDownLinearUp);
    std::cout << "Quality resize tests passed\n";
    return 0;
  } catch (const std::exception& e) {
    std::cerr << "Quality resize test failed: " << e.what() << '\n';
    return 1;
  }
}
