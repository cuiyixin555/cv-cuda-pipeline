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

#include "cv-cuda-pipeline/calculators/image_process/cuda/median_blur_kernel.h"

#include <cuda_runtime.h>

#include <algorithm>
#include <cstddef>
#include <cstdint>
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
class Buffer {
 public:
  explicit Buffer(std::size_t size) {
    Check(cudaMalloc(&data_, size), "malloc");
  }
  ~Buffer() { cudaFree(data_); }
  Buffer(const Buffer&) = delete;
  Buffer& operator=(const Buffer&) = delete;
  std::uint8_t* get() const { return static_cast<std::uint8_t*>(data_); }

 private:
  void* data_ = nullptr;
};

std::vector<std::uint8_t> Reference(const std::vector<std::uint8_t>& src,
                                    int width, int height, std::size_t stride) {
  std::vector<std::uint8_t> out(stride * height, 0xD3);
  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      for (int c = 0; c < 3; ++c) {
        std::uint8_t values[49];
        int i = 0;
        for (int dy = -3; dy <= 3; ++dy) {
          const int sy = std::clamp(y + dy, 0, height - 1);
          for (int dx = -3; dx <= 3; ++dx) {
            const int sx = std::clamp(x + dx, 0, width - 1);
            values[i++] = src[static_cast<std::size_t>(sy) * stride +
                              static_cast<std::size_t>(sx) * 3 + c];
          }
        }
        std::sort(values, values + 49);
        out[static_cast<std::size_t>(y) * stride +
            static_cast<std::size_t>(x) * 3 + c] = values[24];
      }
    }
  }
  return out;
}

void Test(int width, int height) {
  const std::size_t stride = static_cast<std::size_t>(width) * 3 + 11;
  std::vector<std::uint8_t> input(stride * height, 0x5A);
  std::vector<std::uint8_t> output(stride * height, 0xD3);
  for (int y = 0; y < height; ++y)
    for (int x = 0; x < width; ++x)
      for (int c = 0; c < 3; ++c)
        input[static_cast<std::size_t>(y) * stride +
              static_cast<std::size_t>(x) * 3 + c] =
            static_cast<std::uint8_t>((x * 37 + y * 61 + c * 83) % 256);
  Buffer device_input(input.size()), device_output(output.size());
  Check(cudaMemcpy(device_input.get(), input.data(), input.size(),
                   cudaMemcpyHostToDevice),
        "upload");
  Check(cudaMemcpy(device_output.get(), output.data(), output.size(),
                   cudaMemcpyHostToDevice),
        "initialize output");
  Check(cv_cuda_pipeline::MedianBlur7x7({device_input.get(), stride,
                                         device_output.get(), stride, width,
                                         height}),
        "launch");
  Check(cudaMemcpy(output.data(), device_output.get(), output.size(),
                   cudaMemcpyDeviceToHost),
        "download");
  const auto expected = Reference(input, width, height, stride);
  for (int y = 0; y < height; ++y)
    for (std::size_t x = 0; x < stride; ++x)
      Require(output[static_cast<std::size_t>(y) * stride + x] ==
                  expected[static_cast<std::size_t>(y) * stride + x],
              "median output or row padding mismatch");
}

void TestInvalidInputs() {
  std::uint8_t* ptr = nullptr;
  Require(cv_cuda_pipeline::MedianBlur7x7({nullptr, 3, ptr, 3, 1, 1}) ==
              cudaErrorInvalidValue,
          "null input accepted");
  Require(cv_cuda_pipeline::MedianBlur7x7({ptr, 3, nullptr, 3, 1, 1}) ==
              cudaErrorInvalidValue,
          "null output accepted");
  Require(cv_cuda_pipeline::MedianBlur7x7({ptr, 3, ptr, 3, 0, 1}) ==
              cudaErrorInvalidValue,
          "zero width accepted");
  Require(cv_cuda_pipeline::MedianBlur7x7({ptr, 3, ptr, 3, 1, 1}) ==
              cudaErrorInvalidValue,
          "in-place input accepted");
}
}  // namespace

int main() {
  try {
    TestInvalidInputs();
    Test(1, 1);
    Test(3, 2);
    Test(19, 13);
    std::cout << "Median blur tests passed\n";
    return 0;
  } catch (const std::exception& e) {
    std::cerr << "Median blur test failed: " << e.what() << '\n';
    return 1;
  }
}
