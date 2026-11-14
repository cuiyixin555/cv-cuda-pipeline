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

#include "cv-cuda-pipeline/calculators/image_process/cuda/inpaint_kernel.h"

#include <cuda_runtime.h>

#include <algorithm>
#include <cmath>
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

void Test() {
  constexpr int kWidth = 5, kHeight = 3;
  constexpr std::size_t kSrcStride = kWidth * 3 + 5;
  constexpr std::size_t kMaskStride = kWidth + 3;
  constexpr std::size_t kDstStride = kWidth * 3 + 7;
  std::vector<std::uint8_t> src(kSrcStride * kHeight, 0x5a);
  std::vector<std::uint8_t> mask(kMaskStride * kHeight, 0);
  std::vector<std::uint8_t> dst(kDstStride * kHeight, 0xa5);
  for (int y = 0; y < kHeight; ++y)
    for (int x = 0; x < kWidth; ++x)
      for (int c = 0; c < 3; ++c)
        src[y * kSrcStride + x * 3 + c] =
            static_cast<std::uint8_t>(20 + x * 25 + y * 31 + c * 47);
  mask[kMaskStride + 2] = 255;

  DeviceBuffer device_src(src.size()), device_mask(mask.size()),
      device_dst(dst.size());
  Check(cudaMemcpy(device_src.get(), src.data(), src.size(),
                   cudaMemcpyHostToDevice),
        "Upload source");
  Check(cudaMemcpy(device_mask.get(), mask.data(), mask.size(),
                   cudaMemcpyHostToDevice),
        "Upload mask");
  Check(cudaMemcpy(device_dst.get(), dst.data(), dst.size(),
                   cudaMemcpyHostToDevice),
        "Initialize destination");
  cv_cuda_pipeline::InpaintImageView image{
      static_cast<const std::uint8_t*>(device_src.get()),
      kSrcStride,
      static_cast<const std::uint8_t*>(device_mask.get()),
      kMaskStride,
      static_cast<std::uint8_t*>(device_dst.get()),
      kDstStride,
      kWidth,
      kHeight};
  Check(cv_cuda_pipeline::Inpaint(image), "Inpaint");
  Check(cudaMemcpy(dst.data(), device_dst.get(), dst.size(),
                   cudaMemcpyDeviceToHost),
        "Download destination");

  for (int y = 0; y < kHeight; ++y) {
    for (int x = 0; x < kWidth; ++x) {
      for (int c = 0; c < 3; ++c) {
        if (mask[y * kMaskStride + x] == 0) {
          Require(dst[y * kDstStride + x * 3 + c] ==
                      src[y * kSrcStride + x * 3 + c],
                  "Unmasked pixel changed");
          continue;
        }
        double sum = 0.0, weight_sum = 0.0;
        for (int dy = -4; dy <= 4; ++dy)
          for (int dx = -4; dx <= 4; ++dx) {
            const int sx = x + dx, sy = y + dy;
            if ((dx == 0 && dy == 0) || sx < 0 || sx >= kWidth || sy < 0 ||
                sy >= kHeight || mask[sy * kMaskStride + sx] != 0)
              continue;
            const double weight = 1.0 / (1.0 + dx * dx + dy * dy);
            sum += weight * src[sy * kSrcStride + sx * 3 + c];
            weight_sum += weight;
          }
        const int expected = static_cast<int>(std::nearbyint(sum / weight_sum));
        Require(std::abs(static_cast<int>(dst[y * kDstStride + x * 3 + c]) -
                         expected) <= 1,
                "Masked value differs from inverse-distance reference");
      }
    }
    for (std::size_t x = kWidth * 3; x < kDstStride; ++x)
      Require(dst[y * kDstStride + x] == 0xa5, "Destination padding changed");
  }
  image.src = nullptr;
  Require(cv_cuda_pipeline::Inpaint(image) == cudaErrorInvalidValue,
          "Null source accepted");
}
}  // namespace

int main() {
  try {
    Test();
    std::cout << "Inpaint kernel tests passed\n";
    return 0;
  } catch (const std::exception& e) {
    std::cerr << "Inpaint kernel test failed: " << e.what() << '\n';
    return 1;
  }
}
