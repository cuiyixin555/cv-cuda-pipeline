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

#include "cv-cuda-pipeline/calculators/image_process/cuda/customcrop_kernel.h"

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
void Test(int sw, int sh, int x, int y, int w, int h) {
  const std::size_t ss = static_cast<std::size_t>(sw) * 3 + 5;
  const std::size_t row = static_cast<std::size_t>(w) * 3, ds = row + 7;
  std::vector<std::uint8_t> src(ss * sh, 0x5a), dst(ds * h, 0xa5);
  for (int yy = 0; yy < sh; ++yy)
    for (int xx = 0; xx < sw; ++xx)
      for (int c = 0; c < 3; ++c)
        src[yy * ss + xx * 3 + c] = (xx * 37 + yy * 61 + c * 83) % 256;
  Buffer in(src.size()), out(dst.size());
  cudaStream_t stream;
  Check(cudaStreamCreate(&stream));
  Check(cudaMemcpyAsync(in.get(), src.data(), src.size(),
                        cudaMemcpyHostToDevice, stream));
  Check(cudaMemcpyAsync(out.get(), dst.data(), dst.size(),
                        cudaMemcpyHostToDevice, stream));
  cv_cuda_pipeline::CustomCropImageView view{in.get(), ss, sw, sh, out.get(),
                                             ds,       x,  y,  w,  h};
  Check(cv_cuda_pipeline::CustomCrop(view, stream));
  Check(cudaMemcpyAsync(dst.data(), out.get(), dst.size(),
                        cudaMemcpyDeviceToHost, stream));
  Check(cudaStreamSynchronize(stream));
  Check(cudaStreamDestroy(stream));
  for (int yy = 0; yy < h; ++yy) {
    for (std::size_t xx = 0; xx < row; ++xx)
      Require(dst[yy * ds + xx] == src[(yy + y) * ss + x * 3 + xx],
              "Crop mismatch");
    for (std::size_t xx = row; xx < ds; ++xx)
      Require(dst[yy * ds + xx] == 0xa5, "Padding modified");
  }
  auto bad = view;
  bad.src = nullptr;
  Require(cv_cuda_pipeline::CustomCrop(bad) == cudaErrorInvalidValue,
          "Null input accepted");
  bad = view;
  bad.dst = nullptr;
  Require(cv_cuda_pipeline::CustomCrop(bad) == cudaErrorInvalidValue,
          "Null output accepted");
  bad = view;
  bad.dst = in.get();
  Require(cv_cuda_pipeline::CustomCrop(bad) == cudaErrorInvalidValue,
          "In-place accepted");
  bad = view;
  bad.x = -1;
  Require(cv_cuda_pipeline::CustomCrop(bad) == cudaErrorInvalidValue,
          "Negative origin accepted");
  bad = view;
  bad.x = sw;
  Require(cv_cuda_pipeline::CustomCrop(bad) == cudaErrorInvalidValue,
          "Out-of-bounds origin accepted");
  bad = view;
  bad.width = sw;
  if (x != 0)
    Require(cv_cuda_pipeline::CustomCrop(bad) == cudaErrorInvalidValue,
            "Oversized crop accepted");
  bad = view;
  bad.height = 0;
  Require(cv_cuda_pipeline::CustomCrop(bad) == cudaErrorInvalidValue,
          "Empty crop accepted");
  bad = view;
  bad.src_stride = static_cast<std::size_t>(sw) * 3 - 1;
  Require(cv_cuda_pipeline::CustomCrop(bad) == cudaErrorInvalidValue,
          "Short input stride accepted");
  bad = view;
  bad.dst_stride = row - 1;
  Require(cv_cuda_pipeline::CustomCrop(bad) == cudaErrorInvalidValue,
          "Short output stride accepted");
  bad = view;
  bad.src_height = 2;
  bad.src_stride = std::numeric_limits<std::size_t>::max();
  bad.y = 0;
  bad.height = 1;
  Require(cv_cuda_pipeline::CustomCrop(bad) == cudaErrorInvalidValue,
          "Stride overflow accepted");
}
}  // namespace
int main() {
  try {
    Test(1, 1, 0, 0, 1, 1);
    Test(10, 9, 0, 0, 10, 9);
    Test(31, 23, 6, 4, 18, 13);
    Test(720, 720, 144, 144, 432, 432);
    Test(2876, 1699, 575, 339, 1725, 1019);
    std::cout << "Custom crop tests passed\n";
    return 0;
  } catch (const std::exception& e) {
    std::cerr << "Custom crop test failed: " << e.what() << '\n';
    return 1;
  }
}
