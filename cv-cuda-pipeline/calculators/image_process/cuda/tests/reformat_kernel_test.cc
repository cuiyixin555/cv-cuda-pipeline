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

#include "cv-cuda-pipeline/calculators/image_process/cuda/reformat_kernel.h"

#include <cuda_runtime.h>

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
  std::uint8_t* get() const { return static_cast<std::uint8_t*>(data_); }

 private:
  void* data_ = nullptr;
};
void Test(int width, int height) {
  const std::size_t row = static_cast<std::size_t>(width) * 3;
  const std::size_t src_stride = row + 5;
  const std::size_t planar_row = width + 3;
  const std::size_t plane_stride = planar_row * height + 7;
  const std::size_t out_stride = row + 9;
  std::vector<std::uint8_t> src(src_stride * height, 0x5a);
  std::vector<std::uint8_t> planar(plane_stride * 3, 0xa5);
  std::vector<std::uint8_t> roundtrip(out_stride * height, 0x6a);
  for (int y = 0; y < height; ++y)
    for (int x = 0; x < width; ++x)
      for (int c = 0; c < 3; ++c)
        src[y * src_stride + x * 3 + c] =
            static_cast<std::uint8_t>((x * 37 + y * 61 + c * 83) % 256);
  Buffer in(src.size()), mid(planar.size()), out(roundtrip.size());
  cudaStream_t stream;
  Check(cudaStreamCreate(&stream));
  Check(cudaMemcpyAsync(in.get(), src.data(), src.size(),
                        cudaMemcpyHostToDevice, stream));
  Check(cudaMemcpyAsync(mid.get(), planar.data(), planar.size(),
                        cudaMemcpyHostToDevice, stream));
  Check(cudaMemcpyAsync(out.get(), roundtrip.data(), roundtrip.size(),
                        cudaMemcpyHostToDevice, stream));
  cv_cuda_pipeline::ReformatImageView to_planar{
      in.get(),   src_stride,   0,     mid.get(),
      planar_row, plane_stride, width, height};
  Check(cv_cuda_pipeline::HwcToChw(to_planar, stream));
  Check(cudaMemcpyAsync(planar.data(), mid.get(), planar.size(),
                        cudaMemcpyDeviceToHost, stream));
  cv_cuda_pipeline::ReformatImageView to_hwc{
      mid.get(),  planar_row, plane_stride, out.get(),
      out_stride, 0,          width,        height};
  Check(cv_cuda_pipeline::ChwToHwc(to_hwc, stream));
  Check(cudaMemcpyAsync(roundtrip.data(), out.get(), roundtrip.size(),
                        cudaMemcpyDeviceToHost, stream));
  Check(cudaStreamSynchronize(stream));
  Check(cudaStreamDestroy(stream));
  for (int c = 0; c < 3; ++c) {
    for (int y = 0; y < height; ++y) {
      for (int x = 0; x < width; ++x)
        Require(planar[c * plane_stride + y * planar_row + x] ==
                    src[y * src_stride + x * 3 + c],
                "HWC to CHW mismatch");
      for (std::size_t x = width; x < planar_row; ++x)
        Require(planar[c * plane_stride + y * planar_row + x] == 0xa5,
                "Planar row padding modified");
    }
    for (std::size_t x = planar_row * height; x < plane_stride; ++x)
      Require(planar[c * plane_stride + x] == 0xa5,
              "Planar image padding modified");
  }
  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x)
      for (int c = 0; c < 3; ++c)
        Require(roundtrip[y * out_stride + x * 3 + c] ==
                    src[y * src_stride + x * 3 + c],
                "CHW to HWC mismatch");
    for (std::size_t x = row; x < out_stride; ++x)
      Require(roundtrip[y * out_stride + x] == 0x6a,
              "Interleaved padding modified");
  }
  auto bad = to_planar;
  bad.src = nullptr;
  Require(cv_cuda_pipeline::HwcToChw(bad) == cudaErrorInvalidValue,
          "Null input accepted");
  bad = to_planar;
  bad.dst = nullptr;
  Require(cv_cuda_pipeline::HwcToChw(bad) == cudaErrorInvalidValue,
          "Null planar output accepted");
  bad = to_planar;
  bad.dst_row_stride = width - 1;
  Require(cv_cuda_pipeline::HwcToChw(bad) == cudaErrorInvalidValue,
          "Short planar row stride accepted");
  bad = to_planar;
  bad.dst_plane_stride = planar_row * height - 1;
  Require(cv_cuda_pipeline::HwcToChw(bad) == cudaErrorInvalidValue,
          "Short planar plane stride accepted");
  bad = to_hwc;
  bad.src = nullptr;
  Require(cv_cuda_pipeline::ChwToHwc(bad) == cudaErrorInvalidValue,
          "Null planar input accepted");
  bad = to_hwc;
  bad.dst = nullptr;
  Require(cv_cuda_pipeline::ChwToHwc(bad) == cudaErrorInvalidValue,
          "Null HWC output accepted");
  bad = to_hwc;
  bad.src_row_stride = width - 1;
  Require(cv_cuda_pipeline::ChwToHwc(bad) == cudaErrorInvalidValue,
          "Short source row stride accepted");
  bad = to_hwc;
  bad.src_plane_stride = planar_row * height - 1;
  Require(cv_cuda_pipeline::ChwToHwc(bad) == cudaErrorInvalidValue,
          "Short source plane stride accepted");
  bad = to_hwc;
  bad.width = 0;
  Require(cv_cuda_pipeline::ChwToHwc(bad) == cudaErrorInvalidValue,
          "Invalid dimensions accepted");
}
}  // namespace
int main() {
  try {
    Test(1, 1);
    Test(1, 257);
    Test(257, 1);
    Test(31, 23);
    Test(720, 720);
    Test(2876, 1699);
    std::cout << "Reformat tests passed\n";
    return 0;
  } catch (const std::exception& e) {
    std::cerr << "Reformat test failed: " << e.what() << '\n';
    return 1;
  }
}
