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

#include <cuda_runtime.h>

#include <algorithm>
#include <limits>

#include "cv-cuda-pipeline/calculators/image_process/cuda/conv2d_kernel.h"

namespace cv_cuda_pipeline {
namespace {

bool ValidStride(std::size_t stride, std::size_t row, std::size_t height) {
  return stride >= row &&
         height - 1 <= (std::numeric_limits<std::size_t>::max() - row) / stride;
}

__global__ void Sharpen(Conv2dImageView image, std::size_t count) {
  for (std::size_t i =
           static_cast<std::size_t>(blockIdx.x) * blockDim.x + threadIdx.x;
       i < count; i += static_cast<std::size_t>(gridDim.x) * blockDim.x) {
    const int x = i % image.width, y = i / image.width;
    const int left = x > 0 ? x - 1 : (image.width > 1 ? 1 : 0);
    const int right =
        x < image.width - 1 ? x + 1 : (image.width > 1 ? x - 1 : 0);
    const int top = y > 0 ? y - 1 : (image.height > 1 ? 1 : 0);
    const int bottom =
        y < image.height - 1 ? y + 1 : (image.height > 1 ? y - 1 : 0);
    const auto* center = image.src +
                         static_cast<std::size_t>(y) * image.src_stride +
                         static_cast<std::size_t>(x) * 3;
    const auto* l = image.src + static_cast<std::size_t>(y) * image.src_stride +
                    static_cast<std::size_t>(left) * 3;
    const auto* r = image.src + static_cast<std::size_t>(y) * image.src_stride +
                    static_cast<std::size_t>(right) * 3;
    const auto* t = image.src +
                    static_cast<std::size_t>(top) * image.src_stride +
                    static_cast<std::size_t>(x) * 3;
    const auto* b = image.src +
                    static_cast<std::size_t>(bottom) * image.src_stride +
                    static_cast<std::size_t>(x) * 3;
    auto* out = image.dst + static_cast<std::size_t>(y) * image.dst_stride +
                static_cast<std::size_t>(x) * 3;
    for (int c = 0; c < 3; ++c)
      out[c] = max(0, min(255, 5 * center[c] - l[c] - r[c] - t[c] - b[c]));
  }
}
}  // namespace

cudaError_t Conv2dSharpen(Conv2dImageView image, cudaStream_t stream) {
  if (!image.src || !image.dst || image.src == image.dst || image.width <= 0 ||
      image.height <= 0)
    return cudaErrorInvalidValue;
  const auto width = static_cast<std::size_t>(image.width);
  const auto height = static_cast<std::size_t>(image.height);
  const auto limit = std::numeric_limits<std::size_t>::max();
  if (width > limit / 3 || width > limit / height ||
      !ValidStride(image.src_stride, width * 3, height) ||
      !ValidStride(image.dst_stride, width * 3, height))
    return cudaErrorInvalidValue;
  const auto pixels = width * height;
  const auto blocks = static_cast<unsigned int>(
      std::min<std::size_t>((pixels - 1) / 256 + 1, 65535));
  Sharpen<<<blocks, 256, 0, stream>>>(image, pixels);
  return cudaGetLastError();
}
}  // namespace cv_cuda_pipeline
