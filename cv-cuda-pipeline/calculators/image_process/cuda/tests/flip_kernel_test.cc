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

#include "cv-cuda-pipeline/calculators/image_process/cuda/flip_kernel.h"

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
void Test(int width, int height, int code) {
  const std::size_t src_stride = static_cast<std::size_t>(width) * 3 + 5;
  const std::size_t dst_stride = static_cast<std::size_t>(width) * 3 + 7;
  std::vector<std::uint8_t> src(src_stride * height, 0x5a);
  std::vector<std::uint8_t> dst(dst_stride * height, 0xa5);
  for (int y = 0; y < height; ++y)
    for (int x = 0; x < width; ++x)
      for (int c = 0; c < 3; ++c)
        src[y * src_stride + x * 3 + c] = (x * 37 + y * 61 + c * 83) % 256;
  Buffer in(src.size()), out(dst.size());
  cudaStream_t stream;
  Check(cudaStreamCreate(&stream));
  Check(cudaMemcpyAsync(in.get(), src.data(), src.size(),
                        cudaMemcpyHostToDevice, stream));
  Check(cudaMemcpyAsync(out.get(), dst.data(), dst.size(),
                        cudaMemcpyHostToDevice, stream));
  cv_cuda_pipeline::FlipImageView view{in.get(),   src_stride, out.get(),
                                       dst_stride, width,      height};
  Check(cv_cuda_pipeline::Flip(view, code, stream));
  Check(cudaMemcpyAsync(dst.data(), out.get(), dst.size(),
                        cudaMemcpyDeviceToHost, stream));
  Check(cudaStreamSynchronize(stream));
  Check(cudaStreamDestroy(stream));
  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      const int sx = code != 0 ? width - 1 - x : x;
      const int sy = code != 1 ? height - 1 - y : y;
      for (int c = 0; c < 3; ++c)
        Require(dst[y * dst_stride + x * 3 + c] ==
                    src[sy * src_stride + sx * 3 + c],
                "Flip output mismatch");
    }
    for (std::size_t i = width * 3; i < dst_stride; ++i)
      Require(dst[y * dst_stride + i] == 0xa5, "Output padding modified");
  }
  Require(cv_cuda_pipeline::Flip(view, 2) == cudaErrorInvalidValue,
          "Invalid flip code accepted");
  auto invalid = view;
  invalid.src = nullptr;
  Require(cv_cuda_pipeline::Flip(invalid, code) == cudaErrorInvalidValue,
          "Null input accepted");
  invalid = view;
  invalid.src_stride = static_cast<std::size_t>(width) * 3 - 1;
  Require(cv_cuda_pipeline::Flip(invalid, code) == cudaErrorInvalidValue,
          "Invalid stride accepted");
  invalid = view;
  invalid.height = 2;
  invalid.dst_stride = std::numeric_limits<std::size_t>::max();
  Require(cv_cuda_pipeline::Flip(invalid, code) == cudaErrorInvalidValue,
          "Overflowing stride accepted");
}
}  // namespace

int main() {
  try {
    for (int code : {-1, 0, 1}) {
      Test(17, 11, code);
      Test(1, 1, code);
      Test(1, 9, code);
      Test(9, 1, code);
      Test(720, 720, code);
    }
    std::cout << "Flip tests passed\n";
    return 0;
  } catch (const std::exception& e) {
    std::cerr << "Flip test failed: " << e.what() << '\n';
    return 1;
  }
}
