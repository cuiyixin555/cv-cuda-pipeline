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

#include "cv-cuda-pipeline/calculators/image_process/cuda/customcrop_kernel.h"

namespace cv_cuda_pipeline {
namespace {
bool ValidStride(std::size_t stride, std::size_t row, std::size_t height) {
  return stride >= row &&
         height - 1 <= (std::numeric_limits<std::size_t>::max() - row) / stride;
}
__global__ void Crop(CustomCropImageView image, std::size_t count) {
  for (std::size_t i =
           static_cast<std::size_t>(blockIdx.x) * blockDim.x + threadIdx.x;
       i < count; i += static_cast<std::size_t>(gridDim.x) * blockDim.x) {
    const std::size_t x = i % image.width, y = i / image.width;
    const auto* src =
        image.src + (y + image.y) * image.src_stride + (x + image.x) * 3;
    auto* dst = image.dst + y * image.dst_stride + x * 3;
    for (int c = 0; c < 3; ++c) dst[c] = src[c];
  }
}
}  // namespace
cudaError_t CustomCrop(CustomCropImageView image, cudaStream_t stream) {
  if (!image.src || !image.dst || image.src == image.dst ||
      image.src_width <= 0 || image.src_height <= 0 || image.x < 0 ||
      image.y < 0 || image.width <= 0 || image.height <= 0 ||
      image.x >= image.src_width || image.y >= image.src_height ||
      image.width > image.src_width - image.x ||
      image.height > image.src_height - image.y)
    return cudaErrorInvalidValue;
  const auto sw = static_cast<std::size_t>(image.src_width);
  const auto sh = static_cast<std::size_t>(image.src_height);
  const auto w = static_cast<std::size_t>(image.width);
  const auto h = static_cast<std::size_t>(image.height);
  const auto max = std::numeric_limits<std::size_t>::max();
  if (sw > max / 3 || w > max / 3 || w > max / h ||
      !ValidStride(image.src_stride, sw * 3, sh) ||
      !ValidStride(image.dst_stride, w * 3, h))
    return cudaErrorInvalidValue;
  const auto pixels = w * h;
  const auto blocks = static_cast<unsigned int>(
      std::min<std::size_t>((pixels - 1) / 256 + 1, 65535));
  Crop<<<blocks, 256, 0, stream>>>(image, pixels);
  return cudaGetLastError();
}
}  // namespace cv_cuda_pipeline
