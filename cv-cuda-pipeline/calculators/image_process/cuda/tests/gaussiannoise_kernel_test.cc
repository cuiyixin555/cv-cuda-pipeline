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

#include "cv-cuda-pipeline/calculators/image_process/cuda/gaussiannoise_kernel.h"

#include <cuda_runtime.h>

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

 private:
  void* data_ = nullptr;
};

std::vector<std::uint8_t> Run(const std::vector<std::uint8_t>& input, int width,
                              int height, int channels, std::size_t src_stride,
                              std::size_t dst_stride, float mean, float sigma,
                              std::uint64_t seed) {
  std::vector<std::uint8_t> output(dst_stride * height, 0xA5);
  Buffer device_input(input.size()), device_output(output.size());
  Check(cudaMemcpy(device_input.get(), input.data(), input.size(),
                   cudaMemcpyHostToDevice),
        "upload input");
  Check(cudaMemcpy(device_output.get(), output.data(), output.size(),
                   cudaMemcpyHostToDevice),
        "initialize output");
  Check(cv_cuda_pipeline::AddGaussianNoise(
            {device_input.get(), src_stride, device_output.get(), dst_stride,
             width, height, channels, mean, sigma, seed}),
        "launch Gaussian noise");
  Check(cudaMemcpy(output.data(), device_output.get(), output.size(),
                   cudaMemcpyDeviceToHost),
        "download output");
  return output;
}

void TestReproducibilityAndChannels() {
  constexpr int width = 17, height = 11, channels = 3;
  constexpr std::size_t src_stride = width * channels + 5;
  constexpr std::size_t dst_stride = width * channels + 9;
  std::vector<std::uint8_t> input(src_stride * height, 0x39);
  for (int y = 0; y < height; ++y)
    for (int x = 0; x < width; ++x)
      for (int c = 0; c < channels; ++c)
        input[static_cast<std::size_t>(y) * src_stride +
              static_cast<std::size_t>(x) * channels + c] = 127;
  const auto first = Run(input, width, height, channels, src_stride, dst_stride,
                         0.0f, 25.0f, 42);
  const auto repeated = Run(input, width, height, channels, src_stride,
                            dst_stride, 0.0f, 25.0f, 42);
  const auto other_seed = Run(input, width, height, channels, src_stride,
                              dst_stride, 0.0f, 25.0f, 43);
  Require(first == repeated, "same seed is not reproducible");
  Require(first != other_seed, "different seeds produced identical output");
  bool changed = false;
  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      const auto offset = static_cast<std::size_t>(y) * dst_stride +
                          static_cast<std::size_t>(x) * channels;
      Require(first[offset] == first[offset + 1] &&
                  first[offset] == first[offset + 2],
              "noise sample was not shared across channels");
      changed = changed || first[offset] != 127;
    }
    for (std::size_t x = width * channels; x < dst_stride; ++x)
      Require(first[static_cast<std::size_t>(y) * dst_stride + x] == 0xA5,
              "destination row padding was modified");
  }
  Require(changed, "noise did not change any pixels");
}

void TestZeroSigmaAndOtherChannelCounts() {
  for (const int channels : {1, 4}) {
    const int width = 5, height = 3;
    const std::size_t stride = width * channels;
    std::vector<std::uint8_t> input(stride * height);
    for (std::size_t i = 0; i < input.size(); ++i)
      input[i] = static_cast<std::uint8_t>(i * 13);
    const auto output =
        Run(input, width, height, channels, stride, stride, 0.0f, 0.0f, 1);
    Require(input == output, "zero sigma changed image values");
  }
}

void TestInvalidInputs() {
  std::uint8_t* ptr = nullptr;
  Require(
      cv_cuda_pipeline::AddGaussianNoise({nullptr, 3, ptr, 3, 1, 1, 3, 0.0f,
                                          1.0f, 0}) == cudaErrorInvalidValue,
      "null input accepted");
  Require(
      cv_cuda_pipeline::AddGaussianNoise({ptr, 3, nullptr, 3, 1, 1, 3, 0.0f,
                                          1.0f, 0}) == cudaErrorInvalidValue,
      "null output accepted");
  Require(
      cv_cuda_pipeline::AddGaussianNoise(
          {ptr, 3, ptr, 3, 1, 1, 2, 0.0f, 1.0f, 0}) == cudaErrorInvalidValue,
      "unsupported channel count accepted");
  Require(
      cv_cuda_pipeline::AddGaussianNoise(
          {ptr, 3, ptr, 3, 0, 1, 3, 0.0f, 1.0f, 0}) == cudaErrorInvalidValue,
      "zero width accepted");
  Require(
      cv_cuda_pipeline::AddGaussianNoise(
          {ptr, 3, ptr, 3, 1, 1, 3, 0.0f, -1.0f, 0}) == cudaErrorInvalidValue,
      "negative standard deviation accepted");
  Require(cv_cuda_pipeline::AddGaussianNoise(
              {ptr, 3, ptr, 3, 1, 1, 3, 0.0f, 1.0f / 0.0f, 0}) ==
              cudaErrorInvalidValue,
          "infinite standard deviation accepted");
}
}  // namespace

int main() {
  try {
    TestReproducibilityAndChannels();
    TestZeroSigmaAndOtherChannelCounts();
    TestInvalidInputs();
    std::cout << "Gaussian noise tests passed\n";
    return 0;
  } catch (const std::exception& e) {
    std::cerr << "Gaussian noise test failed: " << e.what() << '\n';
    return 1;
  }
}
