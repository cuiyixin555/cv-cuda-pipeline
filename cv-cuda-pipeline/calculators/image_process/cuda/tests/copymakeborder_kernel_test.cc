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

#include "cv-cuda-pipeline/calculators/image_process/cuda/copymakeborder_kernel.h"

#include <cuda_runtime.h>

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <vector>

#include "cv-cuda-pipeline/framework/port/opencv_core_inc.h"

namespace {
void Check(cudaError_t status) {
  if (status != cudaSuccess)
    throw std::runtime_error(cudaGetErrorString(status));
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
void Test(int width, int height, int top, int bottom, int left, int right) {
  const std::size_t row = static_cast<std::size_t>(width) * 3;
  const std::size_t outrow = static_cast<std::size_t>(width + left + right) * 3;
  const int outh = height + top + bottom;
  const std::size_t ss = row + 5, ds = outrow + 7;
  std::vector<std::uint8_t> src(ss * height, 0x5a), dst(ds * outh, 0xa5);
  for (int y = 0; y < height; ++y)
    for (std::size_t x = 0; x < row; ++x)
      src[y * ss + x] = (x * 37 + y * 61) % 256;
  cv::Mat input(height, width, CV_8UC3, src.data(), ss), expected;
  cv::copyMakeBorder(input, expected, top, bottom, left, right,
                     cv::BORDER_CONSTANT, cv::Scalar(0, 140, 255));
  Buffer in(src.size()), out(dst.size());
  cudaStream_t stream;
  Check(cudaStreamCreate(&stream));
  Check(cudaMemcpyAsync(in.get(), src.data(), src.size(),
                        cudaMemcpyHostToDevice, stream));
  Check(cudaMemcpyAsync(out.get(), dst.data(), dst.size(),
                        cudaMemcpyHostToDevice, stream));
  cv_cuda_pipeline::CopyMakeBorderView view{
      in.get(), ss,     out.get(), ds,    width,        height,
      top,      bottom, left,      right, {0, 140, 255}};
  Check(cv_cuda_pipeline::CopyMakeBorder(view, stream));
  Check(cudaMemcpyAsync(dst.data(), out.get(), dst.size(),
                        cudaMemcpyDeviceToHost, stream));
  Check(cudaStreamSynchronize(stream));
  Check(cudaStreamDestroy(stream));
  for (int y = 0; y < outh; ++y) {
    for (std::size_t x = 0; x < outrow; ++x)
      Require(dst[y * ds + x] == expected.ptr<std::uint8_t>(y)[x],
              "Output mismatch");
    for (std::size_t x = outrow; x < ds; ++x)
      Require(dst[y * ds + x] == 0xa5, "Padding modified");
  }
  auto bad = view;
  bad.src = nullptr;
  Require(cv_cuda_pipeline::CopyMakeBorder(bad) == cudaErrorInvalidValue,
          "Null input accepted");
  bad = view;
  bad.dst = nullptr;
  Require(cv_cuda_pipeline::CopyMakeBorder(bad) == cudaErrorInvalidValue,
          "Null output accepted");
  bad = view;
  bad.dst = in.get();
  Require(cv_cuda_pipeline::CopyMakeBorder(bad) == cudaErrorInvalidValue,
          "In-place accepted");
  bad = view;
  bad.width = 0;
  Require(cv_cuda_pipeline::CopyMakeBorder(bad) == cudaErrorInvalidValue,
          "Invalid width accepted");
  bad = view;
  bad.top = -1;
  Require(cv_cuda_pipeline::CopyMakeBorder(bad) == cudaErrorInvalidValue,
          "Negative border accepted");
  bad = view;
  bad.src_stride = row - 1;
  Require(cv_cuda_pipeline::CopyMakeBorder(bad) == cudaErrorInvalidValue,
          "Short input stride accepted");
  bad = view;
  bad.dst_stride = outrow - 1;
  Require(cv_cuda_pipeline::CopyMakeBorder(bad) == cudaErrorInvalidValue,
          "Short output stride accepted");
  bad = view;
  bad.height = 2;
  bad.src_stride = std::numeric_limits<std::size_t>::max();
  Require(cv_cuda_pipeline::CopyMakeBorder(bad) == cudaErrorInvalidValue,
          "Overflow accepted");
}
}  // namespace
int main() {
  try {
    Test(1, 1, 0, 0, 0, 0);
    Test(1, 2, 1, 2, 3, 4);
    Test(31, 23, 3, 7, 11, 2);
    Test(720, 720, 30, 30, 60, 60);
    Test(2876, 1699, 30, 30, 60, 60);
    std::cout << "CopyMakeBorder tests passed\n";
    return 0;
  } catch (const std::exception& e) {
    std::cerr << "CopyMakeBorder test failed: " << e.what() << '\n';
    return 1;
  }
}
