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

#include "cv-cuda-pipeline/calculators/image_process/cuda/joint_bilateral_filter_kernel.h"

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
int Reflect(int coordinate, int length) {
  const int period = 2 * length;
  coordinate %= period;
  if (coordinate < 0) coordinate += period;
  return coordinate < length ? coordinate : period - 1 - coordinate;
}

void Test() {
  constexpr int kWidth = 7, kHeight = 5;
  constexpr std::size_t kSrcStride = kWidth * 3 + 5;
  constexpr std::size_t kGuideStride = kWidth * 3 + 7;
  constexpr std::size_t kDstStride = kWidth * 3 + 9;
  std::vector<std::uint8_t> src(kSrcStride * kHeight, 0x5a);
  std::vector<std::uint8_t> guide(kGuideStride * kHeight, 0x6b);
  std::vector<std::uint8_t> dst(kDstStride * kHeight, 0xa5);
  for (int y = 0; y < kHeight; ++y)
    for (int x = 0; x < kWidth; ++x)
      for (int c = 0; c < 3; ++c) {
        src[y * kSrcStride + x * 3 + c] =
            static_cast<std::uint8_t>((x * 29 + y * 43 + c * 71) % 256);
        guide[y * kGuideStride + x * 3 + c] =
            static_cast<std::uint8_t>((x * 31 + y * 17) % 256);
      }

  DeviceBuffer device_src(src.size()), device_guide(guide.size()),
      device_dst(dst.size());
  Check(cudaMemcpy(device_src.get(), src.data(), src.size(),
                   cudaMemcpyHostToDevice),
        "Upload source");
  Check(cudaMemcpy(device_guide.get(), guide.data(), guide.size(),
                   cudaMemcpyHostToDevice),
        "Upload guide");
  Check(cudaMemcpy(device_dst.get(), dst.data(), dst.size(),
                   cudaMemcpyHostToDevice),
        "Initialize destination");
  cv_cuda_pipeline::JointBilateralFilterImageView image{
      static_cast<const std::uint8_t*>(device_src.get()),
      kSrcStride,
      static_cast<const std::uint8_t*>(device_guide.get()),
      kGuideStride,
      static_cast<std::uint8_t*>(device_dst.get()),
      kDstStride,
      kWidth,
      kHeight};
  Check(cv_cuda_pipeline::JointBilateralFilter(image), "JointBilateralFilter");
  Check(cudaMemcpy(dst.data(), device_dst.get(), dst.size(),
                   cudaMemcpyDeviceToHost),
        "Download destination");

  for (int y = 0; y < kHeight; ++y) {
    for (int x = 0; x < kWidth; ++x) {
      double sums[3] = {0.0, 0.0, 0.0};
      double weight_sum = 0.0;
      const auto* guide_center = guide.data() + y * kGuideStride + x * 3;
      for (int dy = -4; dy <= 4; ++dy)
        for (int dx = -4; dx <= 4; ++dx) {
          const int sy = Reflect(y + dy, kHeight);
          const int sx = Reflect(x + dx, kWidth);
          const auto* pixel = src.data() + sy * kSrcStride + sx * 3;
          const auto* guide_pixel = guide.data() + sy * kGuideStride + sx * 3;
          double color_distance = 0.0;
          for (int c = 0; c < 3; ++c) {
            const int d = static_cast<int>(guide_pixel[c]) - guide_center[c];
            color_distance += d * d;
          }
          const double space_distance = dx * dx + dy * dy;
          const double weight = std::exp(-(color_distance + space_distance) /
                                         (2.0 * 75.0 * 75.0));
          weight_sum += weight;
          for (int c = 0; c < 3; ++c) sums[c] += weight * pixel[c];
        }
      for (int c = 0; c < 3; ++c) {
        const int expected =
            static_cast<int>(std::nearbyint(sums[c] / weight_sum));
        Require(std::abs(static_cast<int>(dst[y * kDstStride + x * 3 + c]) -
                         expected) <= 1,
                "Output differs from CPU reference");
      }
    }
    for (std::size_t x = kWidth * 3; x < kDstStride; ++x)
      Require(dst[y * kDstStride + x] == 0xa5, "Destination padding changed");
  }
  image.guide = nullptr;
  Require(
      cv_cuda_pipeline::JointBilateralFilter(image) == cudaErrorInvalidValue,
      "Null guide accepted");
}
}  // namespace

int main() {
  try {
    Test();
    std::cout << "Joint bilateral filter tests passed\n";
    return 0;
  } catch (const std::exception& e) {
    std::cerr << "Joint bilateral filter test failed: " << e.what() << '\n';
    return 1;
  }
}
