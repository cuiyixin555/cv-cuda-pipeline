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

#include "cv-cuda-pipeline/calculators/image_process/cuda/erase_kernel.h"

#include <cuda_runtime.h>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <vector>

namespace {
void Check(cudaError_t s) {
  if (s != cudaSuccess) throw std::runtime_error(cudaGetErrorString(s));
}
void Require(bool ok, const char* msg) {
  if (!ok) throw std::runtime_error(msg);
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
void Test(int width, int height) {
  const std::size_t row = static_cast<std::size_t>(width) * 3, ss = row + 5,
                    ds = row + 7;
  std::vector<std::uint8_t> src(ss * height, 0x5a), dst(ds * height, 0xa5),
      expected(dst);
  for (int y = 0; y < height; ++y)
    for (std::size_t x = 0; x < row; ++x) {
      src[y * ss + x] = (x + y * 61) % 256;
      expected[y * ds + x] = src[y * ss + x];
    }
  const int rects[][5] = {{50, 80, 120, 80, 7},    {200, 150, 160, 100, 7},
                          {350, 300, 100, 120, 7}, {480, 80, 120, 100, 7},
                          {480, 260, 120, 100, 7}, {200, 400, 160, 100, 2}};
  const std::uint8_t values[] = {255, 0, 0,   0, 0,   255, 0,   0,
                                 0,   0, 255, 0, 255, 255, 255, 0,
                                 0,   0, 0,   0, 0,   255, 0,   0};
  for (int n = 0; n < 6; ++n)
    for (int y = rects[n][1]; y < std::min(height, rects[n][1] + rects[n][3]);
         ++y)
      for (int x = rects[n][0]; x < std::min(width, rects[n][0] + rects[n][2]);
           ++x)
        for (int c = 0; c < 3; ++c)
          if (rects[n][4] & (1 << c))
            expected[y * ds + x * 3 + (2 - c)] = values[n * 3 + c];
  Buffer in(src.size()), out(dst.size());
  cudaStream_t stream;
  Check(cudaStreamCreate(&stream));
  Check(cudaMemcpyAsync(in.get(), src.data(), src.size(),
                        cudaMemcpyHostToDevice, stream));
  Check(cudaMemcpyAsync(out.get(), dst.data(), dst.size(),
                        cudaMemcpyHostToDevice, stream));
  cv_cuda_pipeline::EraseImageView view{in.get(), ss,    out.get(),
                                        ds,       width, height};
  Check(cv_cuda_pipeline::EraseSample(view, stream));
  Check(cudaMemcpyAsync(dst.data(), out.get(), dst.size(),
                        cudaMemcpyDeviceToHost, stream));
  Check(cudaStreamSynchronize(stream));
  Require(dst == expected, "Output or padding mismatch");
  std::vector<std::uint8_t> rectangle_expected(ds * height, 0xa5);
  for (int y = 0; y < height; ++y)
    for (std::size_t x = 0; x < row; ++x)
      rectangle_expected[y * ds + x] = src[y * ss + x];
  if (width >= 4 && height >= 3) {
    constexpr int x0 = 1, y0 = 1, rect_width = 2, rect_height = 2;
    constexpr cv_cuda_pipeline::EraseColor fill{224, 96, 32};
    for (int y = y0; y < y0 + rect_height; ++y)
      for (int x = x0; x < x0 + rect_width; ++x) {
        rectangle_expected[y * ds + x * 3] = fill.b;
        rectangle_expected[y * ds + x * 3 + 1] = fill.g;
        rectangle_expected[y * ds + x * 3 + 2] = fill.r;
      }
    Check(cudaMemcpyAsync(out.get(), dst.data(), dst.size(),
                          cudaMemcpyHostToDevice, stream));
    Check(cv_cuda_pipeline::EraseRectangle(view, x0, y0, rect_width,
                                           rect_height, fill, stream));
    Check(cudaMemcpyAsync(dst.data(), out.get(), dst.size(),
                          cudaMemcpyDeviceToHost, stream));
    Check(cudaStreamSynchronize(stream));
    Require(dst == rectangle_expected, "Rectangle erase or padding mismatch");
  }
  Check(cudaStreamDestroy(stream));
  auto bad = view;
  bad.src = nullptr;
  Require(cv_cuda_pipeline::EraseSample(bad) == cudaErrorInvalidValue,
          "Null input accepted");
  bad = view;
  bad.dst = nullptr;
  Require(cv_cuda_pipeline::EraseSample(bad) == cudaErrorInvalidValue,
          "Null output accepted");
  bad = view;
  bad.dst = in.get();
  Require(cv_cuda_pipeline::EraseSample(bad) == cudaErrorInvalidValue,
          "In-place accepted");
  bad = view;
  bad.width = 0;
  Require(cv_cuda_pipeline::EraseSample(bad) == cudaErrorInvalidValue,
          "Invalid width accepted");
  bad = view;
  bad.height = -1;
  Require(cv_cuda_pipeline::EraseSample(bad) == cudaErrorInvalidValue,
          "Invalid height accepted");
  bad = view;
  bad.src_stride = row - 1;
  Require(cv_cuda_pipeline::EraseSample(bad) == cudaErrorInvalidValue,
          "Short input stride accepted");
  bad = view;
  bad.dst_stride = row - 1;
  Require(cv_cuda_pipeline::EraseSample(bad) == cudaErrorInvalidValue,
          "Short output stride accepted");
  bad = view;
  bad.height = 2;
  bad.src_stride = std::numeric_limits<std::size_t>::max();
  Require(cv_cuda_pipeline::EraseSample(bad) == cudaErrorInvalidValue,
          "Stride overflow accepted");
  const cv_cuda_pipeline::EraseColor fill{1, 2, 3};
  Require(cv_cuda_pipeline::EraseRectangle(view, -1, 0, 1, 1, fill) ==
              cudaErrorInvalidValue,
          "Negative rectangle origin accepted");
  Require(cv_cuda_pipeline::EraseRectangle(view, width, 0, 1, 1, fill) ==
              cudaErrorInvalidValue,
          "Out-of-bounds rectangle accepted");
}
}  // namespace
int main() {
  try {
    Test(1, 1);
    Test(51, 81);
    Test(355, 410);
    Test(600, 500);
    Test(720, 720);
    Test(2876, 1699);
    std::cout << "Erase tests passed\n";
    return 0;
  } catch (const std::exception& e) {
    std::cerr << "Erase test failed: " << e.what() << '\n';
    return 1;
  }
}
