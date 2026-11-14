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

#include "cv-cuda-pipeline/calculators/image_process/cuda/histogrameq_kernel.h"

#include <cuda_runtime.h>

#include <algorithm>
#include <cmath>
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
    Check(cudaMalloc(&data_, size), "cudaMalloc");
  }
  ~Buffer() { cudaFree(data_); }
  Buffer(const Buffer&) = delete;
  Buffer& operator=(const Buffer&) = delete;
  std::uint8_t* get() const { return static_cast<std::uint8_t*>(data_); }
  std::uint32_t* get32() const { return static_cast<std::uint32_t*>(data_); }

 private:
  void* data_ = nullptr;
};

std::vector<std::uint8_t> Reference(const std::vector<std::uint8_t>& input,
                                    int width, int height,
                                    std::size_t src_stride,
                                    std::size_t dst_stride) {
  std::uint32_t histogram[256]{};
  for (int y = 0; y < height; ++y)
    for (int x = 0; x < width; ++x)
      ++histogram[input[static_cast<std::size_t>(y) * src_stride + x]];
  std::uint32_t cdf_min = 0;
  for (const auto count : histogram)
    if (count != 0) {
      cdf_min = count;
      break;
    }
  const auto pixels = static_cast<std::uint32_t>(width * height);
  const float denominator = static_cast<float>(pixels - cdf_min);
  const float scale = denominator > 0.0f ? 255.0f / denominator : 0.0f;
  std::uint8_t lookup[256];
  std::uint32_t cdf = 0;
  for (int i = 0; i < 256; ++i) {
    cdf += histogram[i];
    const float value =
        cdf > cdf_min ? static_cast<float>(cdf - cdf_min) * scale : 0.0f;
    lookup[i] = static_cast<std::uint8_t>(
        std::clamp(static_cast<int>(std::nearbyint(value)), 0, 255));
  }
  std::vector<std::uint8_t> output(dst_stride * height, 0xA5);
  for (int y = 0; y < height; ++y)
    for (int x = 0; x < width; ++x)
      output[static_cast<std::size_t>(y) * dst_stride + x] =
          lookup[input[static_cast<std::size_t>(y) * src_stride + x]];
  return output;
}

void Test() {
  constexpr int width = 43, height = 27;
  constexpr std::size_t src_stride = width + 5, dst_stride = width + 9;
  std::vector<std::uint8_t> input(src_stride * height, 0x39);
  std::vector<std::uint8_t> output(dst_stride * height, 0xA5);
  for (int y = 0; y < height; ++y)
    for (int x = 0; x < width; ++x)
      input[static_cast<std::size_t>(y) * src_stride + x] =
          static_cast<std::uint8_t>((x * 17 + y * 31) % 256);
  Buffer device_input(input.size()), device_output(output.size()),
      histogram(256 * sizeof(std::uint32_t)), lookup(256);
  cudaStream_t stream;
  Check(cudaStreamCreate(&stream), "cudaStreamCreate");
  Check(cudaMemcpyAsync(device_input.get(), input.data(), input.size(),
                        cudaMemcpyHostToDevice, stream),
        "upload input");
  Check(cudaMemcpyAsync(device_output.get(), output.data(), output.size(),
                        cudaMemcpyHostToDevice, stream),
        "initialize output");
  Check(cv_cuda_pipeline::HistogramEqualize(
            {device_input.get(), src_stride, device_output.get(), dst_stride,
             histogram.get32(), lookup.get(), width, height},
            stream),
        "launch histogram equalization");
  Check(cudaMemcpyAsync(output.data(), device_output.get(), output.size(),
                        cudaMemcpyDeviceToHost, stream),
        "download output");
  Check(cudaStreamSynchronize(stream), "synchronize");
  Check(cudaStreamDestroy(stream), "destroy stream");
  const auto expected = Reference(input, width, height, src_stride, dst_stride);
  Require(output == expected, "histogram equalization or row padding mismatch");
}

void TestInvalidInputs() {
  std::uint8_t byte = 0;
  std::uint32_t histogram[256]{};
  Require(cv_cuda_pipeline::HistogramEqualize(
              {nullptr, 1, &byte, 1, histogram, &byte, 1, 1}) ==
              cudaErrorInvalidValue,
          "null input accepted");
  Require(cv_cuda_pipeline::HistogramEqualize(
              {&byte, 1, nullptr, 1, histogram, &byte, 1, 1}) ==
              cudaErrorInvalidValue,
          "null output accepted");
  Require(
      cv_cuda_pipeline::HistogramEqualize({&byte, 1, &byte, 1, histogram, &byte,
                                           0, 1}) == cudaErrorInvalidValue,
      "zero width accepted");
  Require(
      cv_cuda_pipeline::HistogramEqualize(
          {&byte, 1, &byte, 1, nullptr, &byte, 1, 1}) == cudaErrorInvalidValue,
      "null histogram accepted");
}
}  // namespace

int main() {
  try {
    Test();
    TestInvalidInputs();
    std::cout << "Histogram equalization tests passed\n";
    return 0;
  } catch (const std::exception& e) {
    std::cerr << "Histogram equalization test failed: " << e.what() << '\n';
    return 1;
  }
}
