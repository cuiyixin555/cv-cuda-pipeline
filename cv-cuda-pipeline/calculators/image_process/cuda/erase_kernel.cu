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

#include "cv-cuda-pipeline/calculators/image_process/cuda/erase_kernel.h"

namespace cv_cuda_pipeline {
namespace {
bool ValidStride(std::size_t stride, std::size_t row, std::size_t height) {
  return stride >= row &&
         height - 1 <= (std::numeric_limits<std::size_t>::max() - row) / stride;
}
__global__ void EraseKernel(EraseImageView image, std::size_t pixels) {
  for (std::size_t i =
           static_cast<std::size_t>(blockIdx.x) * blockDim.x + threadIdx.x;
       i < pixels; i += static_cast<std::size_t>(blockDim.x) * gridDim.x) {
    const std::size_t x = i % image.width, y = i / image.width;
    const auto* src = image.src + y * image.src_stride + x * 3;
    auto* dst = image.dst + y * image.dst_stride + x * 3;
    unsigned char b = src[0], g = src[1], r = src[2];
    if (x >= 50 && x < 170 && y >= 80 && y < 160) {
      b = 0;
      g = 0;
      r = 255;
    }
    if (x >= 200 && x < 360 && y >= 150 && y < 250) {
      b = 255;
      g = 0;
      r = 0;
    }
    if (x >= 350 && x < 450 && y >= 300 && y < 420) {
      b = 0;
      g = 0;
      r = 0;
    }
    if (x >= 480 && x < 600 && y >= 80 && y < 180) {
      b = 0;
      g = 255;
      r = 0;
    }
    if (x >= 480 && x < 600 && y >= 260 && y < 360) {
      b = 255;
      g = 255;
      r = 255;
    }
    if (x >= 200 && x < 360 && y >= 400 && y < 500) {
      g = 0;
    }
    dst[0] = b;
    dst[1] = g;
    dst[2] = r;
  }
}

__global__ void EraseRectangleKernel(EraseImageView image, std::size_t pixels,
                                     int x, int y, int width, int height,
                                     EraseColor fill) {
  for (std::size_t i =
           static_cast<std::size_t>(blockIdx.x) * blockDim.x + threadIdx.x;
       i < pixels; i += static_cast<std::size_t>(blockDim.x) * gridDim.x) {
    const std::size_t px = i % image.width, py = i / image.width;
    const auto* src = image.src + py * image.src_stride + px * 3;
    auto* dst = image.dst + py * image.dst_stride + px * 3;
    const bool inside = px >= static_cast<std::size_t>(x) &&
                        px < static_cast<std::size_t>(x + width) &&
                        py >= static_cast<std::size_t>(y) &&
                        py < static_cast<std::size_t>(y + height);
    dst[0] = inside ? fill.b : src[0];
    dst[1] = inside ? fill.g : src[1];
    dst[2] = inside ? fill.r : src[2];
  }
}
}  // namespace
cudaError_t EraseSample(EraseImageView image, cudaStream_t stream) {
  if (!image.src || !image.dst || image.src == image.dst || image.width <= 0 ||
      image.height <= 0)
    return cudaErrorInvalidValue;
  const auto w = static_cast<std::size_t>(image.width);
  const auto h = static_cast<std::size_t>(image.height);
  const auto max = std::numeric_limits<std::size_t>::max();
  if (w > max / 3 || w > max / h || !ValidStride(image.src_stride, w * 3, h) ||
      !ValidStride(image.dst_stride, w * 3, h))
    return cudaErrorInvalidValue;
  const auto count = w * h;
  const auto blocks = static_cast<unsigned int>(
      std::min<std::size_t>((count - 1) / 256 + 1, 65535));
  EraseKernel<<<blocks, 256, 0, stream>>>(image, count);
  return cudaGetLastError();
}

cudaError_t EraseRectangle(EraseImageView image, int x, int y, int width,
                           int height, EraseColor fill, cudaStream_t stream) {
  if (!image.src || !image.dst || image.src == image.dst || image.width <= 0 ||
      image.height <= 0 || x < 0 || y < 0 || width <= 0 || height <= 0 ||
      x >= image.width || y >= image.height || width > image.width - x ||
      height > image.height - y)
    return cudaErrorInvalidValue;
  const auto w = static_cast<std::size_t>(image.width);
  const auto h = static_cast<std::size_t>(image.height);
  const auto max = std::numeric_limits<std::size_t>::max();
  if (w > max / 3 || w > max / h || !ValidStride(image.src_stride, w * 3, h) ||
      !ValidStride(image.dst_stride, w * 3, h))
    return cudaErrorInvalidValue;
  const auto count = w * h;
  const auto blocks = static_cast<unsigned int>(
      std::min<std::size_t>((count - 1) / 256 + 1, 65535));
  EraseRectangleKernel<<<blocks, 256, 0, stream>>>(image, count, x, y, width,
                                                   height, fill);
  return cudaGetLastError();
}
}  // namespace cv_cuda_pipeline
