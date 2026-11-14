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

#include "cv-cuda-pipeline/calculators/image_process/cuda/warp_kernel.h"

#include <cuda_runtime.h>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <limits>
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
class Buffer {
 public:
  explicit Buffer(std::size_t size) {
    Check(cudaMalloc(&data_, size), "cudaMalloc");
  }
  ~Buffer() { cudaFree(data_); }
  Buffer(const Buffer&) = delete;
  Buffer& operator=(const Buffer&) = delete;
  std::uint8_t* get() const { return static_cast<std::uint8_t*>(data_); }

 private:
  void* data_ = nullptr;
};

std::uint8_t Sample(const std::vector<std::uint8_t>& input, std::size_t stride,
                    int width, int height, int x, int y, int channel) {
  if (x < 0 || x >= width || y < 0 || y >= height) return 0;
  return input[static_cast<std::size_t>(y) * stride +
               static_cast<std::size_t>(x) * 3 + channel];
}

void Test(int width, int height, cv_cuda_pipeline::WarpTransform transform) {
  constexpr int channels = 3;
  const std::size_t src_stride = static_cast<std::size_t>(width) * channels + 7;
  const std::size_t dst_stride = static_cast<std::size_t>(width) * channels + 9;
  std::vector<std::uint8_t> input(src_stride * height, 0x53);
  std::vector<std::uint8_t> output(dst_stride * height, 0xA5);
  for (int y = 0; y < height; ++y)
    for (int x = 0; x < width; ++x)
      for (int c = 0; c < channels; ++c)
        input[static_cast<std::size_t>(y) * src_stride +
              static_cast<std::size_t>(x) * channels + c] =
            static_cast<std::uint8_t>((x * 37 + y * 61 + c * 83) % 256);

  Buffer device_input(input.size()), device_output(output.size());
  Check(cudaMemcpy(device_input.get(), input.data(), input.size(),
                   cudaMemcpyHostToDevice),
        "upload input");
  Check(cudaMemcpy(device_output.get(), output.data(), output.size(),
                   cudaMemcpyHostToDevice),
        "initialize output");
  Check(cv_cuda_pipeline::WarpLinear(
            {device_input.get(), src_stride, device_output.get(), dst_stride,
             width, height, channels},
            transform),
        "launch warp");
  Check(cudaMemcpy(output.data(), device_output.get(), output.size(),
                   cudaMemcpyDeviceToHost),
        "download output");

  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      const float denominator = transform.values[6] * x +
                                transform.values[7] * y + transform.values[8];
      const float source_x = (transform.values[0] * x +
                              transform.values[1] * y + transform.values[2]) /
                             denominator;
      const float source_y = (transform.values[3] * x +
                              transform.values[4] * y + transform.values[5]) /
                             denominator;
      const int x0 = static_cast<int>(std::floor(source_x));
      const int y0 = static_cast<int>(std::floor(source_y));
      const int x1 = x0 + 1, y1 = y0 + 1;
      const float wx = source_x - x0, wy = source_y - y0;
      for (int c = 0; c < channels; ++c) {
        const float value =
            Sample(input, src_stride, width, height, x0, y0, c) *
                ((1.0f - wx) * (1.0f - wy)) +
            Sample(input, src_stride, width, height, x1, y0, c) *
                (wx * (1.0f - wy)) +
            Sample(input, src_stride, width, height, x0, y1, c) *
                ((1.0f - wx) * wy) +
            Sample(input, src_stride, width, height, x1, y1, c) * (wx * wy);
        const int expected =
            std::clamp(static_cast<int>(std::nearbyint(value)), 0, 255);
        const auto actual = output[static_cast<std::size_t>(y) * dst_stride +
                                   static_cast<std::size_t>(x) * channels + c];
        Require(std::abs(static_cast<int>(actual) - expected) <= 1,
                "bilinear warp pixel mismatch");
      }
    }
    for (std::size_t x = width * channels; x < dst_stride; ++x)
      Require(output[static_cast<std::size_t>(y) * dst_stride + x] == 0xA5,
              "destination row padding was modified");
  }
}

void TestInvalidInputs() {
  std::uint8_t src = 0, dst = 0;
  const cv_cuda_pipeline::WarpTransform identity{{1, 0, 0, 0, 1, 0, 0, 0, 1}};
  Require(cv_cuda_pipeline::WarpLinear({nullptr, 3, &dst, 3, 1, 1, 3},
                                       identity) == cudaErrorInvalidValue,
          "null input accepted");
  Require(cv_cuda_pipeline::WarpLinear({&src, 3, nullptr, 3, 1, 1, 3},
                                       identity) == cudaErrorInvalidValue,
          "null output accepted");
  Require(cv_cuda_pipeline::WarpLinear({&src, 3, &dst, 3, 1, 1, 2}, identity) ==
              cudaErrorInvalidValue,
          "unsupported channel count accepted");
  auto non_finite = identity;
  non_finite.values[0] = std::numeric_limits<float>::infinity();
  Require(cv_cuda_pipeline::WarpLinear({&src, 3, &dst, 3, 1, 1, 3},
                                       non_finite) == cudaErrorInvalidValue,
          "non-finite transform accepted");
}
}  // namespace

int main() {
  try {
    const cv_cuda_pipeline::WarpTransform identity{{1, 0, 0, 0, 1, 0, 0, 0, 1}};
    const cv_cuda_pipeline::WarpTransform translation{
        {1, 0, 0.5f, 0, 1, -0.25f, 0, 0, 1}};
    const cv_cuda_pipeline::WarpTransform perspective{
        {1.0f, 0.05f, -0.7f, -0.03f, 1.0f, 0.6f, 0.002f, -0.001f, 1.0f}};
    Test(1, 1, identity);
    Test(3, 2, translation);
    Test(19, 13, perspective);
    TestInvalidInputs();
    std::cout << "Warp tests passed\n";
    return 0;
  } catch (const std::exception& e) {
    std::cerr << "Warp test failed: " << e.what() << '\n';
    return 1;
  }
}
