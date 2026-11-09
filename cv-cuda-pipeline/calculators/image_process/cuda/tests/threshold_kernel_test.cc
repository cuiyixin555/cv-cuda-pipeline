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

#include "cv-cuda-pipeline/calculators/image_process/cuda/threshold_kernel.h"

#include <cuda_runtime.h>

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
void Test(int width, int height) {
  const std::size_t row = static_cast<std::size_t>(width) * 3, ss = row + 5,
                    ds = row + 7;
  std::vector<std::uint8_t> src(ss * height, 0x5a), dst(ds * height, 0xa5);
  for (int y = 0; y < height; ++y)
    for (std::size_t x = 0; x < row; ++x) src[y * ss + x] = (x + y * 61) % 256;
  Buffer in(src.size()), out(dst.size());
  cudaStream_t stream;
  Check(cudaStreamCreate(&stream));
  Check(cudaMemcpyAsync(in.get(), src.data(), src.size(),
                        cudaMemcpyHostToDevice, stream));
  Check(cudaMemcpyAsync(out.get(), dst.data(), dst.size(),
                        cudaMemcpyHostToDevice, stream));
  cv_cuda_pipeline::ThresholdImageView view{in.get(), ss,    out.get(),
                                            ds,       width, height};
  Check(cv_cuda_pipeline::Threshold(view, stream));
  Check(cudaMemcpyAsync(dst.data(), out.get(), dst.size(),
                        cudaMemcpyDeviceToHost, stream));
  Check(cudaStreamSynchronize(stream));
  Check(cudaStreamDestroy(stream));
  for (int y = 0; y < height; ++y) {
    for (std::size_t x = 0; x < row; ++x)
      Require(dst[y * ds + x] == (src[y * ss + x] > 128 ? 255 : 0),
              "Threshold mismatch");
    for (std::size_t x = row; x < ds; ++x)
      Require(dst[y * ds + x] == 0xa5, "Padding modified");
  }
  auto bad = view;
  bad.src = nullptr;
  Require(cv_cuda_pipeline::Threshold(bad) == cudaErrorInvalidValue,
          "Null input accepted");
  bad = view;
  bad.dst = nullptr;
  Require(cv_cuda_pipeline::Threshold(bad) == cudaErrorInvalidValue,
          "Null output accepted");
  bad = view;
  bad.dst = in.get();
  Require(cv_cuda_pipeline::Threshold(bad) == cudaErrorInvalidValue,
          "In-place accepted");
  bad = view;
  bad.width = 0;
  Require(cv_cuda_pipeline::Threshold(bad) == cudaErrorInvalidValue,
          "Zero width accepted");
  bad = view;
  bad.height = -1;
  Require(cv_cuda_pipeline::Threshold(bad) == cudaErrorInvalidValue,
          "Negative height accepted");
  bad = view;
  bad.src_stride = row - 1;
  Require(cv_cuda_pipeline::Threshold(bad) == cudaErrorInvalidValue,
          "Short input stride accepted");
  bad = view;
  bad.dst_stride = row - 1;
  Require(cv_cuda_pipeline::Threshold(bad) == cudaErrorInvalidValue,
          "Short output stride accepted");
  bad = view;
  bad.height = 2;
  bad.src_stride = std::numeric_limits<std::size_t>::max();
  Require(cv_cuda_pipeline::Threshold(bad) == cudaErrorInvalidValue,
          "Stride overflow accepted");
}
}  // namespace
int main() {
  try {
    Test(1, 1);
    Test(256, 1);
    Test(1, 257);
    Test(31, 23);
    Test(720, 720);
    Test(2876, 1699);
    std::cout << "Threshold tests passed\n";
    return 0;
  } catch (const std::exception& e) {
    std::cerr << "Threshold test failed: " << e.what() << '\n';
    return 1;
  }
}
