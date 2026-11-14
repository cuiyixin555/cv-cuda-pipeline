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

#include "cv-cuda-pipeline/calculators/image_process/cuda/gaussian_kernel.h"

#include <cuda_runtime.h>

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <vector>

#include "cv-cuda-pipeline/framework/port/opencv_core_inc.h"
#include "cv-cuda-pipeline/framework/port/opencv_imgproc_inc.h"

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
void Test(int width, int height, int pattern, bool five_by_five = false) {
  const std::size_t row = static_cast<std::size_t>(width) * 3, ss = row + 5,
                    ds = row + 7;
  std::vector<std::uint8_t> src(ss * height, 0x5a), dst(ds * height, 0xa5);
  for (int y = 0; y < height; ++y)
    for (int x = 0; x < width; ++x)
      for (int c = 0; c < 3; ++c)
        src[y * ss + x * 3 + c] = pattern == 0 ? 127
                                  : pattern == 1
                                      ? ((x == 0 && y == 0) ? 255 : 0)
                                      : (x * 37 + y * 61 + c * 83) % 256;
  cv::Mat source(height, width, CV_8UC3, src.data(), ss), float_src, expected;
  source.convertTo(float_src, CV_32F);
  if (five_by_five)
    cv::GaussianBlur(float_src, expected, cv::Size(5, 5), 1.0, 1.0,
                     cv::BORDER_CONSTANT);
  else
    cv::GaussianBlur(float_src, expected, cv::Size(9, 9), 1.5, 1.5,
                     cv::BORDER_CONSTANT);
  Buffer in(src.size()), out(dst.size());
  cudaStream_t stream;
  Check(cudaStreamCreate(&stream));
  Check(cudaMemcpyAsync(in.get(), src.data(), src.size(),
                        cudaMemcpyHostToDevice, stream));
  Check(cudaMemcpyAsync(out.get(), dst.data(), dst.size(),
                        cudaMemcpyHostToDevice, stream));
  cv_cuda_pipeline::GaussianImageView view{in.get(), ss,    out.get(),
                                           ds,       width, height};
  Check(five_by_five ? cv_cuda_pipeline::GaussianBlur5x5(view, stream)
                     : cv_cuda_pipeline::GaussianBlur(view, stream));
  Check(cudaMemcpyAsync(dst.data(), out.get(), dst.size(),
                        cudaMemcpyDeviceToHost, stream));
  Check(cudaStreamSynchronize(stream));
  Check(cudaStreamDestroy(stream));
  for (int y = 0; y < height; ++y) {
    for (std::size_t x = 0; x < row; ++x)
      Require(std::abs(static_cast<float>(dst[y * ds + x]) -
                       expected.ptr<float>(y)[x]) <= 0.501f,
              "Gaussian mismatch");
    for (std::size_t x = row; x < ds; ++x)
      Require(dst[y * ds + x] == 0xa5, "Padding modified");
  }
  auto bad = view;
  bad.src = nullptr;
  Require(cv_cuda_pipeline::GaussianBlur(bad) == cudaErrorInvalidValue,
          "Null input accepted");
  bad = view;
  bad.dst = nullptr;
  Require(cv_cuda_pipeline::GaussianBlur(bad) == cudaErrorInvalidValue,
          "Null output accepted");
  bad = view;
  bad.dst = in.get();
  Require(cv_cuda_pipeline::GaussianBlur(bad) == cudaErrorInvalidValue,
          "In-place accepted");
  bad = view;
  bad.width = 0;
  Require(cv_cuda_pipeline::GaussianBlur(bad) == cudaErrorInvalidValue,
          "Invalid width accepted");
  bad = view;
  bad.height = -1;
  Require(cv_cuda_pipeline::GaussianBlur(bad) == cudaErrorInvalidValue,
          "Invalid height accepted");
  bad = view;
  bad.src_stride = row - 1;
  Require(cv_cuda_pipeline::GaussianBlur(bad) == cudaErrorInvalidValue,
          "Short input stride accepted");
  bad = view;
  bad.dst_stride = row - 1;
  Require(cv_cuda_pipeline::GaussianBlur(bad) == cudaErrorInvalidValue,
          "Short output stride accepted");
  bad = view;
  bad.height = 2;
  bad.src_stride = std::numeric_limits<std::size_t>::max();
  Require(cv_cuda_pipeline::GaussianBlur(bad) == cudaErrorInvalidValue,
          "Stride overflow accepted");
}
}  // namespace
int main() {
  try {
    for (int p = 0; p < 3; ++p) {
      Test(1, 1, p);
      Test(1, 7, p);
      Test(7, 1, p);
      Test(2, 3, p);
      Test(31, 23, p);
      Test(1, 1, p, true);
      Test(1, 7, p, true);
      Test(7, 1, p, true);
      Test(2, 3, p, true);
      Test(31, 23, p, true);
    }
    Test(720, 720, 2);
    Test(2876, 1699, 2);
    std::cout << "Gaussian tests passed\n";
    return 0;
  } catch (const std::exception& e) {
    std::cerr << "Gaussian test failed: " << e.what() << '\n';
    return 1;
  }
}
