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

#include "cv-cuda-pipeline/calculators/image_process/cuda/morphology_kernel.h"

#include <cuda_runtime.h>

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
  explicit Buffer(std::size_t n) { Check(cudaMalloc(&p_, n)); }
  ~Buffer() { cudaFree(p_); }
  Buffer(const Buffer&) = delete;
  Buffer& operator=(const Buffer&) = delete;
  std::uint8_t* get() const { return static_cast<std::uint8_t*>(p_); }

 private:
  void* p_ = nullptr;
};
void Test(int width, int height, int pattern) {
  const std::size_t row = static_cast<std::size_t>(width) * 3, ss = row + 5,
                    ts = row + 6, ds = row + 7;
  std::vector<std::uint8_t> src(ss * height, 0x5a), tmp(ts * height, 0x6a),
      dst(ds * height, 0xa5);
  for (int y = 0; y < height; ++y)
    for (int x = 0; x < width; ++x)
      for (int c = 0; c < 3; ++c)
        src[y * ss + x * 3 + c] = pattern == 0 ? 127
                                  : pattern == 1
                                      ? ((x == 0 && y == 0) ? 255 : 0)
                                      : (x * 37 + y * 61 + c * 83) % 256;
  cv::Mat source(height, width, CV_8UC3, src.data(), ss), intermediate,
      expected;
  const auto kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
  cv::dilate(source, intermediate, kernel, cv::Point(-1, -1), 1,
             cv::BORDER_REPLICATE);
  cv::erode(intermediate, expected, kernel, cv::Point(-1, -1), 1,
            cv::BORDER_REPLICATE);
  Buffer in(src.size()), scratch(tmp.size()), out(dst.size());
  cudaStream_t stream;
  Check(cudaStreamCreate(&stream));
  Check(cudaMemcpyAsync(in.get(), src.data(), src.size(),
                        cudaMemcpyHostToDevice, stream));
  Check(cudaMemcpyAsync(scratch.get(), tmp.data(), tmp.size(),
                        cudaMemcpyHostToDevice, stream));
  Check(cudaMemcpyAsync(out.get(), dst.data(), dst.size(),
                        cudaMemcpyHostToDevice, stream));
  cv_cuda_pipeline::MorphologyImageView view{
      in.get(), ss, scratch.get(), ts, out.get(), ds, width, height};
  Check(cv_cuda_pipeline::MorphologyClose(view, stream));
  Check(cudaMemcpyAsync(tmp.data(), scratch.get(), tmp.size(),
                        cudaMemcpyDeviceToHost, stream));
  Check(cudaMemcpyAsync(dst.data(), out.get(), dst.size(),
                        cudaMemcpyDeviceToHost, stream));
  Check(cudaStreamSynchronize(stream));
  Check(cudaStreamDestroy(stream));
  for (int y = 0; y < height; ++y) {
    for (std::size_t x = 0; x < row; ++x) {
      Require(tmp[y * ts + x] == intermediate.ptr<std::uint8_t>(y)[x],
              "Dilation mismatch");
      Require(dst[y * ds + x] == expected.ptr<std::uint8_t>(y)[x],
              "Closing mismatch");
    }
    for (std::size_t x = row; x < ts; ++x)
      Require(tmp[y * ts + x] == 0x6a, "Scratch padding modified");
    for (std::size_t x = row; x < ds; ++x)
      Require(dst[y * ds + x] == 0xa5, "Output padding modified");
  }
  auto bad = view;
  bad.src = nullptr;
  Require(cv_cuda_pipeline::MorphologyClose(bad) == cudaErrorInvalidValue,
          "Null input accepted");
  bad = view;
  bad.scratch = nullptr;
  Require(cv_cuda_pipeline::MorphologyClose(bad) == cudaErrorInvalidValue,
          "Null scratch accepted");
  bad = view;
  bad.dst = nullptr;
  Require(cv_cuda_pipeline::MorphologyClose(bad) == cudaErrorInvalidValue,
          "Null output accepted");
  bad = view;
  bad.scratch = in.get();
  Require(cv_cuda_pipeline::MorphologyClose(bad) == cudaErrorInvalidValue,
          "Overlapping scratch accepted");
  bad = view;
  bad.dst = in.get();
  Require(cv_cuda_pipeline::MorphologyClose(bad) == cudaErrorInvalidValue,
          "In-place accepted");
  bad = view;
  bad.width = 0;
  Require(cv_cuda_pipeline::MorphologyClose(bad) == cudaErrorInvalidValue,
          "Invalid width accepted");
  bad = view;
  bad.height = -1;
  Require(cv_cuda_pipeline::MorphologyClose(bad) == cudaErrorInvalidValue,
          "Invalid height accepted");
  bad = view;
  bad.src_stride = row - 1;
  Require(cv_cuda_pipeline::MorphologyClose(bad) == cudaErrorInvalidValue,
          "Short input stride accepted");
  bad = view;
  bad.scratch_stride = row - 1;
  Require(cv_cuda_pipeline::MorphologyClose(bad) == cudaErrorInvalidValue,
          "Short scratch stride accepted");
  bad = view;
  bad.dst_stride = row - 1;
  Require(cv_cuda_pipeline::MorphologyClose(bad) == cudaErrorInvalidValue,
          "Short output stride accepted");
  bad = view;
  bad.height = 2;
  bad.src_stride = std::numeric_limits<std::size_t>::max();
  Require(cv_cuda_pipeline::MorphologyClose(bad) == cudaErrorInvalidValue,
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
    }
    Test(720, 720, 2);
    Test(2876, 1699, 2);
    std::cout << "Morphology tests passed\n";
    return 0;
  } catch (const std::exception& e) {
    std::cerr << "Morphology test failed: " << e.what() << '\n';
    return 1;
  }
}
