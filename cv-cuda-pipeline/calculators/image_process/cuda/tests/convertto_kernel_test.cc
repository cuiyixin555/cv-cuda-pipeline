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

#include "cv-cuda-pipeline/calculators/image_process/cuda/convertto_kernel.h"

#include <cuda_runtime.h>

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <vector>

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
void Test(int width, int height) {
  const std::size_t row = static_cast<std::size_t>(width) * 3;
  const std::size_t ss = row + 5, ds = row + 7, fs = row * 4 + 8;
  std::vector<std::uint8_t> src(ss * height, 0x5a), dst(ds * height, 0xa5);
  std::vector<float> converted(fs / 4 * height, -1.f);
  for (int y = 0; y < height; ++y)
    for (std::size_t x = 0; x < row; ++x) src[y * ss + x] = (x + y * 61) % 256;
  Buffer in(src.size()), out(dst.size()), tmp(fs * height);
  cudaStream_t stream;
  Check(cudaStreamCreate(&stream));
  Check(cudaMemcpyAsync(in.get(), src.data(), src.size(),
                        cudaMemcpyHostToDevice, stream));
  Check(cudaMemcpyAsync(out.get(), dst.data(), dst.size(),
                        cudaMemcpyHostToDevice, stream));
  Check(cudaMemcpyAsync(tmp.get(), converted.data(), fs * height,
                        cudaMemcpyHostToDevice, stream));
  cv_cuda_pipeline::ConvertToImageView view{
      static_cast<std::uint8_t*>(in.get()),
      ss,
      static_cast<float*>(tmp.get()),
      fs,
      static_cast<std::uint8_t*>(out.get()),
      ds,
      width,
      height};
  Check(cv_cuda_pipeline::ConvertToRoundTrip(view, stream));
  Check(cudaMemcpyAsync(dst.data(), out.get(), dst.size(),
                        cudaMemcpyDeviceToHost, stream));
  Check(cudaMemcpyAsync(converted.data(), tmp.get(), fs * height,
                        cudaMemcpyDeviceToHost, stream));
  Check(cudaStreamSynchronize(stream));
  Check(cudaStreamDestroy(stream));
  for (int y = 0; y < height; ++y) {
    for (std::size_t x = 0; x < row; ++x) {
      Require(dst[y * ds + x] == src[y * ss + x], "Round trip mismatch");
      Require(
          std::abs(converted[y * fs / 4 + x] - src[y * ss + x] / 255.f) < 1e-7f,
          "Intermediate float mismatch");
    }
    for (std::size_t x = row; x < ds; ++x)
      Require(dst[y * ds + x] == 0xa5, "Output padding modified");
    for (std::size_t x = row; x < fs / 4; ++x)
      Require(converted[y * fs / 4 + x] == -1.f, "Float padding modified");
  }
  auto bad = view;
  bad.src = nullptr;
  Require(cv_cuda_pipeline::ConvertToRoundTrip(bad) == cudaErrorInvalidValue,
          "Null input accepted");
  bad = view;
  bad.dst = nullptr;
  Require(cv_cuda_pipeline::ConvertToRoundTrip(bad) == cudaErrorInvalidValue,
          "Null output accepted");
  bad = view;
  bad.scratch = nullptr;
  Require(cv_cuda_pipeline::ConvertToRoundTrip(bad) == cudaErrorInvalidValue,
          "Null scratch accepted");
  bad = view;
  bad.dst = static_cast<std::uint8_t*>(in.get());
  Require(cv_cuda_pipeline::ConvertToRoundTrip(bad) == cudaErrorInvalidValue,
          "In-place accepted");
  bad = view;
  bad.width = 0;
  Require(cv_cuda_pipeline::ConvertToRoundTrip(bad) == cudaErrorInvalidValue,
          "Invalid width accepted");
  bad = view;
  bad.scratch_stride = fs - 1;
  Require(cv_cuda_pipeline::ConvertToRoundTrip(bad) == cudaErrorInvalidValue,
          "Misaligned stride accepted");
  bad = view;
  bad.scratch_stride = row * 4 - 4;
  Require(cv_cuda_pipeline::ConvertToRoundTrip(bad) == cudaErrorInvalidValue,
          "Short float stride accepted");
  bad = view;
  bad.height = 2;
  bad.src_stride = std::numeric_limits<std::size_t>::max();
  Require(cv_cuda_pipeline::ConvertToRoundTrip(bad) == cudaErrorInvalidValue,
          "Overflow accepted");
}
}  // namespace
int main() {
  try {
    Test(1, 1);
    Test(1, 257);
    Test(256, 1);
    Test(31, 23);
    Test(720, 720);
    Test(2876, 1699);
    std::cout << "ConvertTo tests passed\n";
    return 0;
  } catch (const std::exception& e) {
    std::cerr << "ConvertTo test failed: " << e.what() << '\n';
    return 1;
  }
}
