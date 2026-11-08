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

#include "cv-cuda-pipeline/calculators/image_process/cuda/adv_cvt_color_kernel.h"

#include <cuda_runtime.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <vector>

namespace {
using cv_cuda_pipeline::AdvColorDirection;
using cv_cuda_pipeline::AdvColorImageView;

void Check(cudaError_t status) {
  if (status != cudaSuccess) throw std::runtime_error(cudaGetErrorString(status));
}
void Require(bool condition, const char* message) {
  if (!condition) throw std::runtime_error(message);
}
class Buffer {
 public:
  explicit Buffer(std::size_t size) { Check(cudaMalloc(&data_, size)); }
  ~Buffer() { cudaFree(data_); }
  Buffer(const Buffer&) = delete;
  Buffer& operator=(const Buffer&) = delete;
  std::uint8_t* get() const { return static_cast<std::uint8_t*>(data_); }
 private:
  void* data_ = nullptr;
};
using Pixel = std::array<int, 3>;
int Quantize(double value) {
  return std::clamp(static_cast<int>(std::floor(value + 0.5)), 0, 255);
}
Pixel Reference(Pixel p, AdvColorDirection direction) {
  if (direction == AdvColorDirection::kRgbToYuv) {
    const int y = Quantize((3483.0 * p[0] + 11718.0 * p[1] + 1183.0 * p[2]) / 16384);
    return {y, Quantize(128 + (p[2] - y) * 8829.0 / 16384),
            Quantize(128 + (p[0] - y) * 10404.0 / 16384)};
  }
  const int u = p[1] - 128, v = p[2] - 128;
  return {Quantize(p[0] + v * 25802.0 / 16384),
          Quantize(p[0] + (u * -3069.0 + v * -7670.0) / 16384),
          Quantize(p[0] + u * 30402.0 / 16384)};
}
void Test(int width, int height, AdvColorDirection direction) {
  const std::size_t input_stride = static_cast<std::size_t>(width) * 3 + 5;
  const std::size_t output_stride = static_cast<std::size_t>(width) * 3 + 7;
  std::vector<std::uint8_t> input(input_stride * height, 0x5a);
  std::vector<std::uint8_t> output(output_stride * height, 0xa5);
  std::vector<std::uint8_t> roundtrip(input_stride * height, 0xa5);
  for (int y = 0; y < height; ++y)
    for (int x = 0; x < width; ++x)
      for (int c = 0; c < 3; ++c)
        input[y * input_stride + x * 3 + c] = (x * 37 + y * 61 + c * 83) % 256;
  if (width >= 6) {
    const Pixel values[] = {{0, 0, 0}, {255, 255, 255}, {255, 0, 0},
                            {0, 255, 0}, {0, 0, 255}, {128, 128, 128}};
    for (int x = 0; x < 6; ++x)
      for (int c = 0; c < 3; ++c) input[x * 3 + c] = values[x][c];
  }
  Buffer src(input.size()), dst(output.size()), restored(roundtrip.size());
  cudaStream_t stream;
  Check(cudaStreamCreate(&stream));
  Check(cudaMemcpyAsync(src.get(), input.data(), input.size(), cudaMemcpyHostToDevice, stream));
  Check(cudaMemcpyAsync(dst.get(), output.data(), output.size(), cudaMemcpyHostToDevice, stream));
  AdvColorImageView view{src.get(), input_stride, dst.get(), output_stride, width, height};
  Check(cv_cuda_pipeline::AdvCvtColor(view, direction, stream));
  Check(cudaMemcpyAsync(output.data(), dst.get(), output.size(), cudaMemcpyDeviceToHost, stream));
  if (direction == AdvColorDirection::kRgbToYuv) {
    Check(cv_cuda_pipeline::AdvCvtColor(
        {dst.get(), output_stride, restored.get(), input_stride, width, height},
        AdvColorDirection::kYuvToRgb, stream));
    Check(cudaMemcpyAsync(roundtrip.data(), restored.get(), roundtrip.size(),
                          cudaMemcpyDeviceToHost, stream));
  }
  Check(cudaStreamSynchronize(stream));
  Check(cudaStreamDestroy(stream));
  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      const auto* p = input.data() + y * input_stride + x * 3;
      const Pixel expected = Reference({p[0], p[1], p[2]}, direction);
      for (int c = 0; c < 3; ++c)
        Require(output[y * output_stride + x * 3 + c] == expected[c],
                "Color conversion differs from reference");
      if (direction == AdvColorDirection::kRgbToYuv) {
        const Pixel restored_expected = Reference(expected, AdvColorDirection::kYuvToRgb);
        for (int c = 0; c < 3; ++c)
          Require(roundtrip[y * input_stride + x * 3 + c] == restored_expected[c] &&
                      std::abs(restored_expected[c] - p[c]) <= 3,
                  "GPU round-trip mismatch or error too large");
      }
    }
    for (std::size_t x = static_cast<std::size_t>(width) * 3; x < output_stride; ++x)
      Require(output[y * output_stride + x] == 0xa5, "Output padding modified");
  }
  if (width >= 6 && direction == AdvColorDirection::kRgbToYuv) {
    const Pixel expected[] = {{0, 128, 128}, {255, 128, 128}, {54, 99, 255},
                              {182, 30, 12}, {18, 255, 117}, {128, 128, 128}};
    for (int x = 0; x < 6; ++x)
      for (int c = 0; c < 3; ++c)
        Require(output[x * 3 + c] == expected[x][c], "Known RGB color mismatch");
  }
  if (width >= 6 && direction == AdvColorDirection::kYuvToRgb)
    Require(output[0] == 0 && output[1] == 84 && output[2] == 0,
            "Known out-of-gamut YUV color mismatch");
  auto invalid = view;
  invalid.src = nullptr;
  Require(cv_cuda_pipeline::AdvCvtColor(invalid, direction) == cudaErrorInvalidValue, "Null source accepted");
  invalid = view; invalid.dst = nullptr;
  Require(cv_cuda_pipeline::AdvCvtColor(invalid, direction) == cudaErrorInvalidValue, "Null destination accepted");
  invalid = view; invalid.width = 0;
  Require(cv_cuda_pipeline::AdvCvtColor(invalid, direction) == cudaErrorInvalidValue, "Zero width accepted");
  invalid = view; invalid.height = -1;
  Require(cv_cuda_pipeline::AdvCvtColor(invalid, direction) == cudaErrorInvalidValue, "Negative height accepted");
  invalid = view; invalid.src_stride = static_cast<std::size_t>(width) * 3 - 1;
  Require(cv_cuda_pipeline::AdvCvtColor(invalid, direction) == cudaErrorInvalidValue, "Short source stride accepted");
  invalid = view; invalid.dst_stride = static_cast<std::size_t>(width) * 3 - 1;
  Require(cv_cuda_pipeline::AdvCvtColor(invalid, direction) == cudaErrorInvalidValue, "Short destination stride accepted");
  invalid = view; invalid.height = 2; invalid.dst_stride = std::numeric_limits<std::size_t>::max();
  Require(cv_cuda_pipeline::AdvCvtColor(invalid, direction) == cudaErrorInvalidValue, "Overflowing stride accepted");
  Require(cv_cuda_pipeline::AdvCvtColor(view, static_cast<AdvColorDirection>(-1)) ==
              cudaErrorInvalidValue, "Invalid direction accepted");
}
}  // namespace
int main() {
  try {
    for (auto direction : {AdvColorDirection::kRgbToYuv, AdvColorDirection::kYuvToRgb}) {
      Test(1, 1, direction);
      Test(1, 9, direction);
      Test(9, 1, direction);
      Test(31, 23, direction);
      Test(720, 720, direction);
    }
    std::cout << "Advanced color conversion tests passed\n";
    return 0;
  } catch (const std::exception& e) {
    std::cerr << "Advanced color conversion test failed: " << e.what() << '\n';
    return 1;
  }
}
