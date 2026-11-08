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

#include <cuda_runtime.h>

#include <algorithm>
#include <limits>

#include "cv-cuda-pipeline/calculators/image_process/cuda/flip_kernel.h"

namespace cv_cuda_pipeline {
namespace {

bool ValidStride(std::size_t stride, std::size_t row_bytes, int height) {
  return stride >= row_bytes &&
         static_cast<std::size_t>(height - 1) <=
             (std::numeric_limits<std::size_t>::max() - row_bytes) / stride;
}

__global__ void FlipKernel(FlipImageView image, int code, std::size_t pixels) {
  for (std::size_t i =
           static_cast<std::size_t>(blockIdx.x) * blockDim.x + threadIdx.x;
       i < pixels; i += static_cast<std::size_t>(gridDim.x) * blockDim.x) {
    const int x = static_cast<int>(i % image.width);
    const int y = static_cast<int>(i / image.width);
    const int sx = code != 0 ? image.width - 1 - x : x;
    const int sy = code != 1 ? image.height - 1 - y : y;
    const auto* src =
        image.src + sy * image.src_stride + static_cast<std::size_t>(sx) * 3;
    auto* dst =
        image.dst + y * image.dst_stride + static_cast<std::size_t>(x) * 3;
    for (int c = 0; c < 3; ++c) dst[c] = src[c];
  }
}

}  // namespace

cudaError_t Flip(FlipImageView image, int code, cudaStream_t stream) {
  if (!image.src || !image.dst || image.width <= 0 || image.height <= 0 ||
      (code != -1 && code != 0 && code != 1)) {
    return cudaErrorInvalidValue;
  }
  const std::size_t width = static_cast<std::size_t>(image.width);
  const std::size_t height = static_cast<std::size_t>(image.height);
  const std::size_t max = std::numeric_limits<std::size_t>::max();
  if (width > max / 3 || width > max / height ||
      !ValidStride(image.src_stride, width * 3, image.height) ||
      !ValidStride(image.dst_stride, width * 3, image.height)) {
    return cudaErrorInvalidValue;
  }
  const std::size_t pixels = width * height;
  constexpr unsigned int kThreads = 256;
  const auto blocks = static_cast<unsigned int>(
      std::min<std::size_t>((pixels - 1) / kThreads + 1, 65535));
  FlipKernel<<<blocks, kThreads, 0, stream>>>(image, code, pixels);
  return cudaGetLastError();
}

}  // namespace cv_cuda_pipeline
