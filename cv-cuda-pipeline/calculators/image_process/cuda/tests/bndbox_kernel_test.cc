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

#include "cv-cuda-pipeline/calculators/image_process/cuda/bndbox_kernel.h"

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
using cv_cuda_pipeline::BndBoxList;
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

// The strict signed edge tests also cover degenerate inner rectangles.
bool Inside(double x, double y, double left, double top, double right, double bottom) {
  return -(bottom - top) * (x - left) < 0 &&
         (right - left) * (y - bottom) < 0 &&
         (bottom - top) * (x - right) < 0 &&
         -(right - left) * (y - top) < 0;
}

void Test(int width, int height, BndBoxList boxes) {
  const std::size_t row = static_cast<std::size_t>(width) * 3;
  const std::size_t src_stride = row + 5, dst_stride = row + 7;
  std::vector<std::uint8_t> src(src_stride * height, 0x5a);
  std::vector<std::uint8_t> dst(dst_stride * height, 0xa5);
  for (int y = 0; y < height; ++y)
    for (int x = 0; x < width; ++x)
      for (int c = 0; c < 3; ++c)
        src[y * src_stride + x * 3 + c] = (x * 37 + y * 61 + c * 83) % 256;
  Buffer in(src.size()), out(dst.size());
  cudaStream_t stream;
  Check(cudaStreamCreate(&stream));
  Check(cudaMemcpyAsync(in.get(), src.data(), src.size(), cudaMemcpyHostToDevice, stream));
  Check(cudaMemcpyAsync(out.get(), dst.data(), dst.size(), cudaMemcpyHostToDevice, stream));
  cv_cuda_pipeline::BndBoxImageView view{in.get(), src_stride, out.get(),
                                         dst_stride, width, height};
  Check(cv_cuda_pipeline::DrawBndBoxes(view, boxes, stream));
  Check(cudaMemcpyAsync(dst.data(), out.get(), dst.size(), cudaMemcpyDeviceToHost, stream));
  Check(cudaStreamSynchronize(stream));
  Check(cudaStreamDestroy(stream));
  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      std::array<std::uint8_t, 3> expected{
          src[y * src_stride + x * 3], src[y * src_stride + x * 3 + 1],
          src[y * src_stride + x * 3 + 2]};
      for (int j = 0; j < boxes.count; ++j) {
        const auto box = boxes.boxes[j];
        const auto left = std::clamp<long long>(box.x, 0, width - 1);
        const auto top = std::clamp<long long>(box.y, 0, height - 1);
        const auto right = std::clamp<long long>(left + box.width - 1LL, 0, width - 1);
        const auto bottom = std::clamp<long long>(top + box.height - 1LL, 0, height - 1);
        if (left != right && top != bottom &&
            Inside(x, y, left - 2, top - 2, right + 2, bottom + 2) &&
            !Inside(x, y, left + 2, top + 2, right - 2, bottom - 2))
          expected = {box.r, box.g, box.b};
      }
      for (int c = 0; c < 3; ++c)
        Require(dst[y * dst_stride + x * 3 + c] == expected[c],
                "Output differs from rectangle reference");
    }
    for (std::size_t x = row; x < dst_stride; ++x)
      Require(dst[y * dst_stride + x] == 0xa5, "Output padding modified");
  }
  if (width == 31 && height == 23 && boxes.count == 3 &&
      boxes.boxes[0].x == 5 && boxes.boxes[0].width == 12) {
    const auto pixel = [&](int x, int y) {
      const auto* p = dst.data() + y * dst_stride + x * 3;
      return std::array<std::uint8_t, 3>{p[0], p[1], p[2]};
    };
    Require(pixel(5, 6) == std::array<std::uint8_t, 3>{255, 80, 0},
            "First border color mismatch");
    Require(pixel(12, 9) == std::array<std::uint8_t, 3>{0, 220, 60},
            "Overlapping border order mismatch");
    Require(pixel(10, 12) == std::array<std::uint8_t, 3>{
                src[12 * src_stride + 30], src[12 * src_stride + 31],
                src[12 * src_stride + 32]}, "Transparent interior changed");
    Require(pixel(3, 6) == std::array<std::uint8_t, 3>{
                src[6 * src_stride + 9], src[6 * src_stride + 10],
                src[6 * src_stride + 11]}, "Outer boundary mismatch");
  }
  auto invalid = view;
  invalid.src = nullptr;
  Require(cv_cuda_pipeline::DrawBndBoxes(invalid, boxes) == cudaErrorInvalidValue, "Null source accepted");
  invalid = view; invalid.dst = nullptr;
  Require(cv_cuda_pipeline::DrawBndBoxes(invalid, boxes) == cudaErrorInvalidValue, "Null destination accepted");
  invalid = view; invalid.dst = in.get();
  Require(cv_cuda_pipeline::DrawBndBoxes(invalid, boxes) == cudaErrorInvalidValue, "In-place buffers accepted");
  invalid = view; invalid.width = 0;
  Require(cv_cuda_pipeline::DrawBndBoxes(invalid, boxes) == cudaErrorInvalidValue, "Zero width accepted");
  invalid = view; invalid.height = -1;
  Require(cv_cuda_pipeline::DrawBndBoxes(invalid, boxes) == cudaErrorInvalidValue, "Negative height accepted");
  invalid = view; invalid.src_stride = row - 1;
  Require(cv_cuda_pipeline::DrawBndBoxes(invalid, boxes) == cudaErrorInvalidValue, "Short input stride accepted");
  invalid = view; invalid.dst_stride = row - 1;
  Require(cv_cuda_pipeline::DrawBndBoxes(invalid, boxes) == cudaErrorInvalidValue, "Short output stride accepted");
  invalid = view; invalid.height = 2; invalid.src_stride = std::numeric_limits<std::size_t>::max();
  Require(cv_cuda_pipeline::DrawBndBoxes(invalid, boxes) == cudaErrorInvalidValue, "Input stride overflow accepted");
  invalid = view; invalid.height = 2; invalid.dst_stride = std::numeric_limits<std::size_t>::max();
  Require(cv_cuda_pipeline::DrawBndBoxes(invalid, boxes) == cudaErrorInvalidValue, "Output stride overflow accepted");
  BndBoxList bad = boxes;
  bad.count = 4;
  Require(cv_cuda_pipeline::DrawBndBoxes(view, bad) == cudaErrorInvalidValue, "Too many boxes accepted");
  bad.count = -1;
  Require(cv_cuda_pipeline::DrawBndBoxes(view, bad) == cudaErrorInvalidValue, "Negative count accepted");
  bad = boxes; bad.count = 1; bad.boxes[0].width = 0;
  Require(cv_cuda_pipeline::DrawBndBoxes(view, bad) == cudaErrorInvalidValue, "Invalid box accepted");
}
}  // namespace

int main() {
  try {
    const BndBoxList empty{{}, 0};
    const BndBoxList boxes{{{5, 6, 12, 14, 255, 80, 0},
                             {12, 9, 15, 11, 0, 220, 60},
                             {-3, 15, 10, 20, 30, 120, 255}}, 3};
    const BndBoxList extreme{{{INT_MIN, INT_MIN, INT_MAX, INT_MAX, 9, 40, 100}}, 1};
    const BndBoxList small{{{1, 1, 2, 2, 4, 5, 6},
                             {5, 5, 5, 5, 7, 8, 9},
                             {0, 0, 2, 7, 10, 11, 12}}, 3};
    Test(1, 1, boxes);
    Test(1, 9, boxes);
    Test(9, 1, boxes);
    Test(31, 23, empty);
    Test(31, 23, boxes);
    Test(31, 23, small);
    Test(31, 23, extreme);
    Test(720, 720, {{{260, 60, 200, 190, 255, 80, 0},
                     {180, 280, 360, 260, 0, 220, 60},
                     {420, 500, 220, 180, 30, 120, 255}}, 3});
    std::cout << "Bounding box tests passed\n";
    return 0;
  } catch (const std::exception& e) {
    std::cerr << "Bounding box test failed: " << e.what() << '\n';
    return 1;
  }
}
