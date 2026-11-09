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

#include "cv-cuda-pipeline/calculators/image_process/cuda/threshold_kernel.h"
namespace cv_cuda_pipeline {
namespace {
bool ValidStride(std::size_t stride, std::size_t row, std::size_t height) {
  return stride >= row &&
         height - 1 <= (std::numeric_limits<std::size_t>::max() - row) / stride;
}
__global__ void BinaryThreshold(ThresholdImageView image, std::size_t pixels) {
  for (std::size_t i =
           static_cast<std::size_t>(blockIdx.x) * blockDim.x + threadIdx.x;
       i < pixels; i += static_cast<std::size_t>(gridDim.x) * blockDim.x) {
    const std::size_t x = i % image.width, y = i / image.width;
    const auto* src = image.src + y * image.src_stride + x * 3;
    auto* dst = image.dst + y * image.dst_stride + x * 3;
    for (int c = 0; c < 3; ++c) dst[c] = src[c] > 128 ? 255 : 0;
  }
}
}  // namespace
cudaError_t Threshold(ThresholdImageView image, cudaStream_t stream) {
  if (!image.src || !image.dst || image.src == image.dst || image.width <= 0 ||
      image.height <= 0)
    return cudaErrorInvalidValue;
  const auto w = static_cast<std::size_t>(image.width);
  const auto h = static_cast<std::size_t>(image.height);
  const auto limit = std::numeric_limits<std::size_t>::max();
  if (w > limit / 3 || w > limit / h ||
      !ValidStride(image.src_stride, w * 3, h) ||
      !ValidStride(image.dst_stride, w * 3, h))
    return cudaErrorInvalidValue;
  const auto count = w * h;
  const auto blocks = static_cast<unsigned int>(
      std::min<std::size_t>((count - 1) / 256 + 1, 65535));
  BinaryThreshold<<<blocks, 256, 0, stream>>>(image, count);
  return cudaGetLastError();
}
}  // namespace cv_cuda_pipeline
