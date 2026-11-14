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

#include "cv-cuda-pipeline/calculators/image_process/cuda/laplacian_kernel.h"

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
  void* get() const { return data_; }

 private:
  void* data_ = nullptr;
};
void Test(int width, int height, int pattern) {
  const std::size_t row = static_cast<std::size_t>(width) * 3;
  const std::size_t src_stride = row + 5;
  const std::size_t dst_stride = row + 8;
  std::vector<std::uint8_t> src(src_stride * height, 0x5a);
  std::vector<std::uint8_t> dst(dst_stride * height, 0xa5);
  for (int y = 0; y < height; ++y)
    for (int x = 0; x < width; ++x)
      for (int c = 0; c < 3; ++c)
        src[y * src_stride + x * 3 + c] =
            pattern == 0   ? 127
            : pattern == 1 ? ((x == 0 && y == 0) ? 255 : 0)
                           : (x * 37 + y * 61 + c * 83) % 256;
  cv::Mat source(height, width, CV_8UC3, src.data(), src_stride), expected;
  cv::Laplacian(source, expected, CV_8U, 3, 1.0, 0.0, cv::BORDER_REPLICATE);
  Buffer in(src.size()), out(dst_stride * height);
  cudaStream_t stream;
  Check(cudaStreamCreate(&stream));
  Check(cudaMemcpyAsync(in.get(), src.data(), src.size(),
                        cudaMemcpyHostToDevice, stream));
  Check(cudaMemcpyAsync(out.get(), dst.data(), dst.size(),
                        cudaMemcpyHostToDevice, stream));
  cv_cuda_pipeline::LaplacianImageView view{
      static_cast<const std::uint8_t*>(in.get()),
      src_stride,
      static_cast<std::uint8_t*>(out.get()),
      dst_stride,
      width,
      height};
  Check(cv_cuda_pipeline::Laplacian3x3(view, stream));
  Check(cudaMemcpy2DAsync(dst.data(), dst_stride, out.get(), dst_stride,
                          dst_stride, height, cudaMemcpyDeviceToHost, stream));
  Check(cudaStreamSynchronize(stream));
  Check(cudaStreamDestroy(stream));
  for (int y = 0; y < height; ++y) {
    const auto* actual = dst.data() + y * dst_stride;
    for (int x = 0; x < width * 3; ++x)
      Require(actual[x] == expected.ptr<std::uint8_t>(y)[x],
              "Laplacian mismatch");
    const auto* padding = actual + row;
    for (std::size_t i = 0; i < dst_stride - row; ++i)
      Require(padding[i] == 0xa5, "Output padding modified");
  }
  auto bad = view;
  bad.src = nullptr;
  Require(cv_cuda_pipeline::Laplacian3x3(bad) == cudaErrorInvalidValue,
          "Null input accepted");
  bad = view;
  bad.dst = nullptr;
  Require(cv_cuda_pipeline::Laplacian3x3(bad) == cudaErrorInvalidValue,
          "Null output accepted");
  bad = view;
  bad.width = 0;
  Require(cv_cuda_pipeline::Laplacian3x3(bad) == cudaErrorInvalidValue,
          "Invalid width accepted");
  bad = view;
  bad.height = -1;
  Require(cv_cuda_pipeline::Laplacian3x3(bad) == cudaErrorInvalidValue,
          "Invalid height accepted");
  bad = view;
  bad.src_stride = row - 1;
  Require(cv_cuda_pipeline::Laplacian3x3(bad) == cudaErrorInvalidValue,
          "Short input stride accepted");
  bad = view;
  bad.dst_stride = row - 1;
  Require(cv_cuda_pipeline::Laplacian3x3(bad) == cudaErrorInvalidValue,
          "Short output stride accepted");
  bad = view;
  bad.height = 2;
  bad.dst_stride = std::numeric_limits<std::size_t>::max();
  Require(cv_cuda_pipeline::Laplacian3x3(bad) == cudaErrorInvalidValue,
          "Stride overflow accepted");
}
}  // namespace
int main() {
  try {
    for (int pattern = 0; pattern < 3; ++pattern) {
      Test(1, 1, pattern);
      Test(1, 2, pattern);
      Test(2, 1, pattern);
      Test(2, 3, pattern);
      Test(31, 23, pattern);
    }
    Test(720, 720, 2);
    Test(2876, 1699, 2);
    std::cout << "Laplacian tests passed\n";
    return 0;
  } catch (const std::exception& e) {
    std::cerr << "Laplacian test failed: " << e.what() << '\n';
    return 1;
  }
}
