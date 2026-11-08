// SPDX-FileCopyrightText: Copyright (c) 2026 CUI Xin
// SPDX-License-Identifier: MIT

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

#include "cv-cuda-pipeline/calculators/image_process/cuda/center_crop_kernel.h"

#include <cuda_runtime.h>

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <vector>

namespace {
void Check(cudaError_t status) {
  if (status != cudaSuccess) throw std::runtime_error(cudaGetErrorString(status));
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
void Test(int width, int height, int crop_width, int crop_height) {
  const std::size_t src_row = static_cast<std::size_t>(width) * 3;
  const std::size_t dst_row = static_cast<std::size_t>(crop_width) * 3;
  const std::size_t src_stride = src_row + 5, dst_stride = dst_row + 7;
  std::vector<std::uint8_t> src(src_stride * height, 0x5a);
  std::vector<std::uint8_t> dst(dst_stride * crop_height, 0xa5);
  for (int y = 0; y < height; ++y)
    for (int x = 0; x < width; ++x)
      for (int c = 0; c < 3; ++c)
        src[y * src_stride + x * 3 + c] = (x * 37 + y * 61 + c * 83) % 256;
  Buffer in(src.size()), out(dst.size());
  cudaStream_t stream;
  Check(cudaStreamCreate(&stream));
  Check(cudaMemcpyAsync(in.get(), src.data(), src.size(), cudaMemcpyHostToDevice, stream));
  Check(cudaMemcpyAsync(out.get(), dst.data(), dst.size(), cudaMemcpyHostToDevice, stream));
  cv_cuda_pipeline::CenterCropImageView view{in.get(), src_stride, width, height,
                                              out.get(), dst_stride, crop_width, crop_height};
  Check(cv_cuda_pipeline::CenterCrop(view, stream));
  Check(cudaMemcpyAsync(dst.data(), out.get(), dst.size(), cudaMemcpyDeviceToHost, stream));
  Check(cudaStreamSynchronize(stream));
  Check(cudaStreamDestroy(stream));
  const int left = (width - crop_width) / 2, top = (height - crop_height) / 2;
  for (int y = 0; y < crop_height; ++y) {
    for (int x = 0; x < crop_width; ++x)
      for (int c = 0; c < 3; ++c)
        Require(dst[y * dst_stride + x * 3 + c] ==
                    src[(y + top) * src_stride + (x + left) * 3 + c],
                "Output differs from centered reference");
    for (std::size_t x = dst_row; x < dst_stride; ++x)
      Require(dst[y * dst_stride + x] == 0xa5, "Output padding modified");
  }
  auto invalid = view;
  invalid.src = nullptr;
  Require(cv_cuda_pipeline::CenterCrop(invalid) == cudaErrorInvalidValue, "Null source accepted");
  invalid = view; invalid.dst = nullptr;
  Require(cv_cuda_pipeline::CenterCrop(invalid) == cudaErrorInvalidValue, "Null destination accepted");
  invalid = view; invalid.dst = in.get();
  Require(cv_cuda_pipeline::CenterCrop(invalid) == cudaErrorInvalidValue, "In-place buffers accepted");
  invalid = view; invalid.src_width = 0;
  Require(cv_cuda_pipeline::CenterCrop(invalid) == cudaErrorInvalidValue, "Zero input width accepted");
  invalid = view; invalid.src_height = -1;
  Require(cv_cuda_pipeline::CenterCrop(invalid) == cudaErrorInvalidValue, "Negative input height accepted");
  invalid = view; invalid.dst_width = 0;
  Require(cv_cuda_pipeline::CenterCrop(invalid) == cudaErrorInvalidValue, "Zero output width accepted");
  invalid = view; invalid.dst_height = -1;
  Require(cv_cuda_pipeline::CenterCrop(invalid) == cudaErrorInvalidValue, "Negative output height accepted");
  invalid = view; invalid.src_width = 1; invalid.dst_width = 2;
  Require(cv_cuda_pipeline::CenterCrop(invalid) == cudaErrorInvalidValue, "Oversized crop width accepted");
  invalid = view; invalid.src_height = 1; invalid.dst_height = 2;
  Require(cv_cuda_pipeline::CenterCrop(invalid) == cudaErrorInvalidValue, "Oversized crop height accepted");
  invalid = view; invalid.src_stride = src_row - 1;
  Require(cv_cuda_pipeline::CenterCrop(invalid) == cudaErrorInvalidValue, "Short input stride accepted");
  invalid = view; invalid.dst_stride = dst_row - 1;
  Require(cv_cuda_pipeline::CenterCrop(invalid) == cudaErrorInvalidValue, "Short output stride accepted");
  invalid = view; invalid.src_height = 2; invalid.src_stride = std::numeric_limits<std::size_t>::max();
  invalid.dst_height = 1;
  Require(cv_cuda_pipeline::CenterCrop(invalid) == cudaErrorInvalidValue, "Input stride overflow accepted");
  invalid = view; invalid.src_height = 2; invalid.dst_height = 2;
  invalid.dst_stride = std::numeric_limits<std::size_t>::max();
  Require(cv_cuda_pipeline::CenterCrop(invalid) == cudaErrorInvalidValue, "Output stride overflow accepted");
}
}  // namespace

int main() {
  try {
    Test(1, 1, 1, 1);
    Test(1, 9, 1, 4);
    Test(9, 1, 4, 1);
    Test(9, 7, 4, 2);
    Test(8, 6, 3, 5);
    Test(31, 23, 31, 23);
    Test(31, 23, 1, 1);
    Test(720, 720, 224, 224);
    Test(2876, 1699, 640, 480);
    std::cout << "Center crop tests passed\n";
    return 0;
  } catch (const std::exception& e) {
    std::cerr << "Center crop test failed: " << e.what() << '\n';
    return 1;
  }
}
