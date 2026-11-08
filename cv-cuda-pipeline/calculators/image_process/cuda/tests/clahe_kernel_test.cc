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

#include "cv-cuda-pipeline/calculators/image_process/cuda/clahe_kernel.h"

#include <cuda_runtime.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <vector>

namespace {
void Check(cudaError_t s) {
  if (s != cudaSuccess) throw std::runtime_error(cudaGetErrorString(s));
}
void Require(bool condition, const char* message) {
  if (!condition) throw std::runtime_error(message);
}
class Buffer {
 public:
  explicit Buffer(std::size_t n) { Check(cudaMalloc(&p_, n)); }
  ~Buffer() { cudaFree(p_); }
  Buffer(const Buffer&) = delete;
  Buffer& operator=(const Buffer&) = delete;
  std::uint8_t* get() const { return static_cast<std::uint8_t*>(p_); }

 private:
  void* p_ = nullptr;
};
int Reflect(int p, int n) {
  if (n == 1) return 0;
  const int period = 2 * (n - 1);
  p %= period;
  return p < n ? p : period - p;
}
std::vector<std::uint8_t> Reference(const std::vector<std::uint8_t>& input,
                                    int width, int height, int stride) {
  const int tw = (width + 7) / 8, th = (height + 7) / 8, area = tw * th;
  std::array<std::array<std::uint8_t, 256>, 64> lut{};
  for (int ty = 0; ty < 8; ++ty) {
    for (int tx = 0; tx < 8; ++tx) {
      std::array<int, 256> hist{};
      for (int y = 0; y < th; ++y)
        for (int x = 0; x < tw; ++x)
          ++hist[input[Reflect(ty * th + y, height) * stride +
                       Reflect(tx * tw + x, width)]];
      const int limit = std::max(area / 128, 1);
      int excess = 0;
      for (auto& bin : hist) {
        excess += std::max(bin - limit, 0);
        bin = std::min(bin, limit);
      }
      const int residual = excess % 256;
      for (auto& bin : hist) bin += excess / 256;
      if (residual)
        for (int i = 0; i < residual; ++i) ++hist[i * (256 / residual)];
      int sum = 0;
      for (int i = 0; i < 256; ++i) {
        sum += hist[i];
        lut[ty * 8 + tx][i] = std::min((sum * 255 + area / 2) / area, 255);
      }
    }
  }
  std::vector<std::uint8_t> output(width * height);
  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      float gx = std::max(0.f, std::min(7.f, x * (1.f / tw) - 0.5f));
      float gy = std::max(0.f, std::min(7.f, y * (1.f / th) - 0.5f));
      const int x0 = static_cast<int>(gx), y0 = static_cast<int>(gy);
      const int x1 = std::min(x0 + 1, 7), y1 = std::min(y0 + 1, 7);
      const float fx = gx - x0, fy = gy - y0;
      const int v = input[y * stride + x];
      const float value = (1 - fx) * (1 - fy) * lut[y0 * 8 + x0][v] +
                          fx * (1 - fy) * lut[y0 * 8 + x1][v] +
                          (1 - fx) * fy * lut[y1 * 8 + x0][v] +
                          fx * fy * lut[y1 * 8 + x1][v];
      output[y * width + x] =
          static_cast<std::uint8_t>(std::min(value + .5f, 255.f));
    }
  }
  return output;
}
void Test(int width, int height, bool constant) {
  const int src_stride = width + 5, dst_stride = width + 7;
  std::vector<std::uint8_t> src(src_stride * height, 0x5a);
  std::vector<std::uint8_t> dst(dst_stride * height, 0xa5);
  for (int y = 0; y < height; ++y)
    for (int x = 0; x < width; ++x)
      src[y * src_stride + x] = constant ? 67 : (x * 37 + y * 61 + x * y) % 256;
  auto expected = Reference(src, width, height, src_stride);
  Buffer in(src.size()), out(dst.size()),
      luts(cv_cuda_pipeline::kClaheScratchBytes);
  cudaStream_t stream;
  Check(cudaStreamCreate(&stream));
  Check(cudaMemcpyAsync(in.get(), src.data(), src.size(),
                        cudaMemcpyHostToDevice, stream));
  Check(cudaMemcpyAsync(out.get(), dst.data(), dst.size(),
                        cudaMemcpyHostToDevice, stream));
  cv_cuda_pipeline::ClaheImageView view{
      in.get(),  static_cast<std::size_t>(src_stride),
      out.get(), static_cast<std::size_t>(dst_stride),
      width,     height};
  Check(cv_cuda_pipeline::Clahe(view, luts.get(), stream));
  Check(cudaMemcpyAsync(dst.data(), out.get(), dst.size(),
                        cudaMemcpyDeviceToHost, stream));
  Check(cudaStreamSynchronize(stream));
  Check(cudaStreamDestroy(stream));
  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x)
      Require(std::abs(static_cast<int>(dst[y * dst_stride + x]) -
                       expected[y * width + x]) <= 1,
              "CLAHE output differs from CPU reference");
    for (int x = width; x < dst_stride; ++x)
      Require(dst[y * dst_stride + x] == 0xa5, "Output padding modified");
  }
  auto invalid = view;
  invalid.src = nullptr;
  Require(cv_cuda_pipeline::Clahe(invalid, luts.get()) == cudaErrorInvalidValue,
          "Null source accepted");
  invalid = view;
  invalid.dst = in.get();
  Require(cv_cuda_pipeline::Clahe(invalid, luts.get()) == cudaErrorInvalidValue,
          "In-place image accepted");
  invalid = view;
  invalid.width = 0;
  Require(cv_cuda_pipeline::Clahe(invalid, luts.get()) == cudaErrorInvalidValue,
          "Invalid width accepted");
  invalid = view;
  invalid.dst_stride = width - 1;
  Require(cv_cuda_pipeline::Clahe(invalid, luts.get()) == cudaErrorInvalidValue,
          "Short stride accepted");
  invalid = view;
  invalid.height = 2;
  invalid.src_stride = std::numeric_limits<std::size_t>::max();
  Require(cv_cuda_pipeline::Clahe(invalid, luts.get()) == cudaErrorInvalidValue,
          "Overflow accepted");
  Require(cv_cuda_pipeline::Clahe(view, nullptr) == cudaErrorInvalidValue,
          "Null scratch accepted");
  Require(cv_cuda_pipeline::Clahe(view, out.get()) == cudaErrorInvalidValue,
          "Aliased scratch accepted");
}
void TestGray() {
  const std::vector<std::uint8_t> rgb = {255, 0, 0,   0,  255, 0,
                                         0,   0, 255, 12, 45,  99};
  std::vector<std::uint8_t> gray(4);
  Buffer src(rgb.size()), dst(gray.size());
  Check(cudaMemcpy(src.get(), rgb.data(), rgb.size(), cudaMemcpyHostToDevice));
  Check(cv_cuda_pipeline::ClaheRgbToGray({src.get(), 12, dst.get(), 4, 4, 1}));
  Check(
      cudaMemcpy(gray.data(), dst.get(), gray.size(), cudaMemcpyDeviceToHost));
  for (int i = 0; i < 4; ++i)
    Require(gray[i] == (rgb[3 * i] * 9798 + rgb[3 * i + 1] * 19235 +
                        rgb[3 * i + 2] * 3735 + 16384) /
                           32768,
            "Grayscale mismatch");
  cv_cuda_pipeline::ClaheImageView invalid{src.get(), 11, dst.get(), 4, 4, 1};
  Require(cv_cuda_pipeline::ClaheRgbToGray(invalid) == cudaErrorInvalidValue,
          "Short RGB stride accepted");
  invalid.src_stride = 12;
  invalid.dst = nullptr;
  Require(cv_cuda_pipeline::ClaheRgbToGray(invalid) == cudaErrorInvalidValue,
          "Null grayscale destination accepted");
}
}  // namespace
int main() {
  try {
    TestGray();
    for (auto shape : {std::array<int, 2>{1, 1},
                       {2, 5},
                       {8, 8},
                       {9, 8},
                       {8, 9},
                       {31, 23},
                       {720, 720},
                       {2876, 1699}}) {
      Test(shape[0], shape[1], false);
      Test(shape[0], shape[1], true);
    }
    std::cout << "CLAHE tests passed\n";
    return 0;
  } catch (const std::exception& e) {
    std::cerr << "CLAHE test failed: " << e.what() << '\n';
    return 1;
  }
}
