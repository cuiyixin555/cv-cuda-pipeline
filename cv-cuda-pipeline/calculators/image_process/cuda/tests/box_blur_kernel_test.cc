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

#include "cv-cuda-pipeline/calculators/image_process/cuda/box_blur_kernel.h"

#include <cuda_runtime.h>

#include <algorithm>
#include <array>
#include <climits>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <vector>

namespace {
using cv_cuda_pipeline::BoxBlurBoxes;
using Pixel = std::array<std::uint8_t, 3>;
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
void Test(int width, int height, BoxBlurBoxes boxes, bool constant = false) {
  const std::size_t row = static_cast<std::size_t>(width) * 3;
  const std::size_t src_stride = row + 5, dst_stride = row + 7;
  std::vector<std::uint8_t> src(src_stride * height, 0x5a);
  std::vector<std::uint8_t> dst(dst_stride * height, 0xa5);
  std::vector<std::uint8_t> expected = dst;
  for (int y = 0; y < height; ++y)
    for (int x = 0; x < width; ++x)
      for (int c = 0; c < 3; ++c) {
        const auto v = static_cast<std::uint8_t>(constant ? 123 : (x * 37 + y * 61 + c * 83) % 256);
        src[y * src_stride + x * 3 + c] = v;
        expected[y * dst_stride + x * 3 + c] = v;
      }
  for (int b = 0; b < boxes.count; ++b) {
    const auto box = boxes.boxes[b];
    if (box.width < 3 || box.height < 3) continue;
    const int left = std::clamp(box.x, 0, width - 1);
    const int top = std::clamp(box.y, 0, height - 1);
    const int right = static_cast<int>(std::clamp<long long>(
        static_cast<long long>(left) + box.width - 1, 0, width - 1));
    const int bottom = static_cast<int>(std::clamp<long long>(
        static_cast<long long>(top) + box.height - 1, 0, height - 1));
    if (left == right || top == bottom) continue;
    const int w = right - left, h = bottom - top;
    std::array<Pixel, 1024> crop{}, blurred{};
    for (int y = 0; y < 32; ++y)
      for (int x = 0; x < 32; ++x) {
        const int sx = std::min(static_cast<int>(x / 32.0f * w + 0.5f + left), width - 1);
        const int sy = std::min(static_cast<int>(y / 32.0f * h + 0.5f + top), height - 1);
        for (int c = 0; c < 3; ++c) crop[y * 32 + x][c] = src[sy * src_stride + sx * 3 + c];
      }
    const int radius = std::min(box.kernel_size / 2, 31);
    for (int y = 0; y < 32; ++y)
      for (int x = 0; x < 32; ++x) {
        int sums[3] = {0, 0, 0}, count = 0;
        for (int yy = 0; yy < 32; ++yy)
          for (int xx = 0; xx < 32; ++xx)
            if (std::abs(yy - y) <= radius && std::abs(xx - x) <= radius) {
              for (int c = 0; c < 3; ++c) sums[c] += crop[yy * 32 + xx][c];
              ++count;
            }
        for (int c = 0; c < 3; ++c) blurred[y * 32 + x][c] = sums[c] / count;
      }
    for (int y = top; y < bottom; ++y)
      for (int x = left; x < right; ++x) {
        const int cx = static_cast<int>((x - left) / static_cast<float>(w) * 32);
        const int cy = static_cast<int>((y - top) / static_cast<float>(h) * 32);
        if (cx >= 32 || cy >= 32) continue;
        for (int c = 0; c < 3; ++c)
          expected[y * dst_stride + x * 3 + c] = blurred[cy * 32 + cx][c];
      }
  }
  Buffer in(src.size()), out(dst.size()), scratch(cv_cuda_pipeline::kBoxBlurScratchBytes);
  cudaStream_t stream;
  Check(cudaStreamCreate(&stream));
  Check(cudaMemcpyAsync(in.get(), src.data(), src.size(), cudaMemcpyHostToDevice, stream));
  Check(cudaMemcpyAsync(out.get(), dst.data(), dst.size(), cudaMemcpyHostToDevice, stream));
  cv_cuda_pipeline::BoxBlurImageView view{in.get(), src_stride, out.get(), dst_stride,
                                           width, height};
  Check(cv_cuda_pipeline::BoxBlur(view, boxes, scratch.get(), stream));
  Check(cudaMemcpyAsync(dst.data(), out.get(), dst.size(), cudaMemcpyDeviceToHost, stream));
  Check(cudaStreamSynchronize(stream));
  Check(cudaStreamDestroy(stream));
  Require(dst == expected, "Output or padding differs from CPU reference");
  auto invalid = view;
  invalid.src = nullptr;
  Require(cv_cuda_pipeline::BoxBlur(invalid, boxes, scratch.get()) == cudaErrorInvalidValue, "Null source accepted");
  invalid = view; invalid.dst = nullptr;
  Require(cv_cuda_pipeline::BoxBlur(invalid, boxes, scratch.get()) == cudaErrorInvalidValue, "Null destination accepted");
  Require(cv_cuda_pipeline::BoxBlur(view, boxes, nullptr) == cudaErrorInvalidValue, "Null scratch accepted");
  invalid = view; invalid.dst = in.get();
  Require(cv_cuda_pipeline::BoxBlur(invalid, boxes, scratch.get()) == cudaErrorInvalidValue, "In-place accepted");
  invalid = view; invalid.width = 0;
  Require(cv_cuda_pipeline::BoxBlur(invalid, boxes, scratch.get()) == cudaErrorInvalidValue, "Zero width accepted");
  invalid = view; invalid.height = -1;
  Require(cv_cuda_pipeline::BoxBlur(invalid, boxes, scratch.get()) == cudaErrorInvalidValue, "Negative height accepted");
  invalid = view; invalid.src_stride = row - 1;
  Require(cv_cuda_pipeline::BoxBlur(invalid, boxes, scratch.get()) == cudaErrorInvalidValue, "Short input stride accepted");
  invalid = view; invalid.dst_stride = row - 1;
  Require(cv_cuda_pipeline::BoxBlur(invalid, boxes, scratch.get()) == cudaErrorInvalidValue, "Short output stride accepted");
  invalid = view; invalid.height = 2; invalid.src_stride = std::numeric_limits<std::size_t>::max();
  Require(cv_cuda_pipeline::BoxBlur(invalid, boxes, scratch.get()) == cudaErrorInvalidValue, "Stride overflow accepted");
  invalid = view; invalid.height = 2; invalid.dst_stride = std::numeric_limits<std::size_t>::max();
  Require(cv_cuda_pipeline::BoxBlur(invalid, boxes, scratch.get()) == cudaErrorInvalidValue, "Stride overflow accepted");
  auto bad = boxes; bad.count = 4;
  Require(cv_cuda_pipeline::BoxBlur(view, bad, scratch.get()) == cudaErrorInvalidValue, "Too many boxes accepted");
  bad.count = -1;
  Require(cv_cuda_pipeline::BoxBlur(view, bad, scratch.get()) == cudaErrorInvalidValue, "Negative count accepted");
  bad = boxes; bad.count = 1; bad.boxes[0].kernel_size = 0;
  Require(cv_cuda_pipeline::BoxBlur(view, bad, scratch.get()) == cudaErrorInvalidValue, "Invalid kernel accepted");
  bad.boxes[0].kernel_size = 3; bad.boxes[0].width = -1;
  Require(cv_cuda_pipeline::BoxBlur(view, bad, scratch.get()) == cudaErrorInvalidValue, "Negative box width accepted");
}
}  // namespace

int main() {
  try {
    const BoxBlurBoxes boxes{{{3, 5, 25, 30, 21}, {15, 16, 40, 32, 45},
                               {-7, 28, 12, 14, 11}}, 3};
    Test(1, 1, boxes);
    Test(2, 9, boxes);
    Test(31, 23, {{{}, {}, {}}, 0});
    Test(63, 53, boxes);
    Test(63, 53, boxes, true);
    Test(127, 91, {{{4, 3, 71, 51, 1}, {20, 15, 90, 75, 4},
                    {30, 30, 3, 3, INT_MAX}}, 3});
    Test(47, 33, {{{INT_MIN, INT_MIN, INT_MAX, INT_MAX, 11}}, 1});
    Test(720, 720, {{{90, 90, 144, 144, 21}, {180, 180, 360, 360, 45},
                     {540, 540, 120, 120, 11}}, 3});
    std::cout << "Box blur tests passed\n";
    return 0;
  } catch (const std::exception& e) {
    std::cerr << "Box blur test failed: " << e.what() << '\n';
    return 1;
  }
}
