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

#include "cv-cuda-pipeline/calculators/image_process/cuda/brightness_contrast_kernel.h"

#include <cuda_runtime.h>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <vector>

namespace {
void Check(cudaError_t status) {
  if (status != cudaSuccess) throw std::runtime_error(cudaGetErrorString(status));
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
int Reference(int value) {
  const float inner = std::fma(1.4f, static_cast<float>(value) - 127.0f, 127.0f);
  const float result = std::fma(1.5f, inner, 20.0f);
  return static_cast<int>(std::nearbyint(std::clamp(result, 0.0f, 255.0f)));
}
void Test(int width, int height) {
  const std::size_t row = static_cast<std::size_t>(width) * 3;
  const std::size_t src_stride = row + 5, dst_stride = row + 7;
  std::vector<std::uint8_t> src(src_stride * height, 0x5a);
  std::vector<std::uint8_t> dst(dst_stride * height, 0xa5);
  for (int y = 0; y < height; ++y)
    for (int x = 0; x < width; ++x)
      for (int c = 0; c < 3; ++c)
        src[y * src_stride + x * 3 + c] = (x * 3 + y * 61 + c) % 256;
  Buffer in(src.size()), out(dst.size());
  cudaStream_t stream;
  Check(cudaStreamCreate(&stream));
  Check(cudaMemcpyAsync(in.get(), src.data(), src.size(), cudaMemcpyHostToDevice, stream));
  Check(cudaMemcpyAsync(out.get(), dst.data(), dst.size(), cudaMemcpyHostToDevice, stream));
  cv_cuda_pipeline::BrightnessContrastImageView view{in.get(), src_stride, out.get(),
                                                       dst_stride, width, height};
  Check(cv_cuda_pipeline::BrightnessContrast(view, stream));
  Check(cudaMemcpyAsync(dst.data(), out.get(), dst.size(), cudaMemcpyDeviceToHost, stream));
  Check(cudaStreamSynchronize(stream));
  Check(cudaStreamDestroy(stream));
  for (int y = 0; y < height; ++y) {
    for (std::size_t x = 0; x < row; ++x)
      Require(dst[y * dst_stride + x] == Reference(src[y * src_stride + x]),
              "Pixel differs from CPU reference");
    for (std::size_t x = row; x < dst_stride; ++x)
      Require(dst[y * dst_stride + x] == 0xa5, "Output padding modified");
  }
  if (width >= 256) {
    const auto pixel = [&](int value) {
      const auto offset = static_cast<std::size_t>(value);
      return dst[offset];
    };
    Require(pixel(0) == 0 && pixel(50) == 49 && pixel(126) == 208 &&
            pixel(127) == 210 && pixel(128) == 213 && pixel(149) == 255 &&
            pixel(255) == 255, "Known clipping or rounding values differ");
  }
  auto invalid = view;
  invalid.src = nullptr;
  Require(cv_cuda_pipeline::BrightnessContrast(invalid) == cudaErrorInvalidValue, "Null source accepted");
  invalid = view; invalid.dst = nullptr;
  Require(cv_cuda_pipeline::BrightnessContrast(invalid) == cudaErrorInvalidValue, "Null destination accepted");
  invalid = view; invalid.dst = in.get();
  Require(cv_cuda_pipeline::BrightnessContrast(invalid) == cudaErrorInvalidValue, "In-place buffers accepted");
  invalid = view; invalid.width = 0;
  Require(cv_cuda_pipeline::BrightnessContrast(invalid) == cudaErrorInvalidValue, "Zero width accepted");
  invalid = view; invalid.height = -1;
  Require(cv_cuda_pipeline::BrightnessContrast(invalid) == cudaErrorInvalidValue, "Negative height accepted");
  invalid = view; invalid.src_stride = row - 1;
  Require(cv_cuda_pipeline::BrightnessContrast(invalid) == cudaErrorInvalidValue, "Short input stride accepted");
  invalid = view; invalid.dst_stride = row - 1;
  Require(cv_cuda_pipeline::BrightnessContrast(invalid) == cudaErrorInvalidValue, "Short output stride accepted");
  invalid = view; invalid.height = 2; invalid.src_stride = std::numeric_limits<std::size_t>::max();
  Require(cv_cuda_pipeline::BrightnessContrast(invalid) == cudaErrorInvalidValue, "Input stride overflow accepted");
  invalid = view; invalid.height = 2; invalid.dst_stride = std::numeric_limits<std::size_t>::max();
  Require(cv_cuda_pipeline::BrightnessContrast(invalid) == cudaErrorInvalidValue, "Output stride overflow accepted");
}
}  // namespace

int main() {
  try {
    Test(1, 1);
    Test(1, 9);
    Test(9, 1);
    Test(31, 23);
    Test(256, 1);
    Test(720, 720);
    std::cout << "Brightness contrast tests passed\n";
    return 0;
  } catch (const std::exception& e) {
    std::cerr << "Brightness contrast test failed: " << e.what() << '\n';
    return 1;
  }
}
