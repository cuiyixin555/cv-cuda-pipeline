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

#include "cv-cuda-pipeline/calculators/image_process/cuda/bndbox_kernel.h"

#include <cuda_runtime.h>

#include <algorithm>
#include <limits>

namespace cv_cuda_pipeline {
namespace {

bool ValidStride(std::size_t stride, std::size_t row_bytes, int height) {
  return stride >= row_bytes &&
         static_cast<std::size_t>(height - 1) <=
             (std::numeric_limits<std::size_t>::max() - row_bytes) / stride;
}

__device__ long long Clamp(long long value, long long high) {
  return value < 0 ? 0 : (value > high ? high : value);
}

__device__ bool InRect(long long x, long long y, long long left, long long top,
                       long long right, long long bottom) {
  if (left < right && top < bottom)
    return x > left && x < right && y > top && y < bottom;
  if (left > right && top > bottom)
    return x > right && x < left && y > bottom && y < top;
  return false;
}

__global__ void BndBoxKernel(BndBoxImageView image, BndBoxList boxes,
                             std::size_t pixels) {
  for (std::size_t i = static_cast<std::size_t>(blockIdx.x) * blockDim.x +
                       threadIdx.x;
       i < pixels; i += static_cast<std::size_t>(gridDim.x) * blockDim.x) {
    const int x = static_cast<int>(i % image.width);
    const int y = static_cast<int>(i / image.width);
    const auto* src = image.src + static_cast<std::size_t>(y) * image.src_stride +
                      static_cast<std::size_t>(x) * 3;
    std::uint8_t r = src[0], g = src[1], b = src[2];
    for (int j = 0; j < boxes.count; ++j) {
      const auto box = boxes.boxes[j];
      const long long left = Clamp(box.x, image.width - 1);
      const long long top = Clamp(box.y, image.height - 1);
      const long long right = Clamp(left + box.width - 1LL, image.width - 1);
      const long long bottom = Clamp(top + box.height - 1LL, image.height - 1);
      if (left == right || top == bottom) continue;
      if (InRect(x, y, left - 2, top - 2, right + 2, bottom + 2) &&
          !InRect(x, y, left + 2, top + 2, right - 2, bottom - 2)) {
        r = box.r;
        g = box.g;
        b = box.b;
      }
    }
    auto* dst = image.dst + static_cast<std::size_t>(y) * image.dst_stride +
                static_cast<std::size_t>(x) * 3;
    dst[0] = r;
    dst[1] = g;
    dst[2] = b;
  }
}

}  // namespace

cudaError_t DrawBndBoxes(BndBoxImageView image, BndBoxList boxes,
                        cudaStream_t stream) {
  if (!image.src || !image.dst || image.src == image.dst ||
      image.width <= 0 || image.height <= 0 ||
      boxes.count < 0 || boxes.count > 3)
    return cudaErrorInvalidValue;
  for (int i = 0; i < boxes.count; ++i)
    if (boxes.boxes[i].width <= 0 || boxes.boxes[i].height <= 0)
      return cudaErrorInvalidValue;
  const std::size_t width = static_cast<std::size_t>(image.width);
  const std::size_t height = static_cast<std::size_t>(image.height);
  const auto max = std::numeric_limits<std::size_t>::max();
  if (width > max / 3 || width > max / height ||
      !ValidStride(image.src_stride, width * 3, image.height) ||
      !ValidStride(image.dst_stride, width * 3, image.height))
    return cudaErrorInvalidValue;
  constexpr unsigned int kThreads = 256;
  const std::size_t pixels = width * height;
  const auto blocks = static_cast<unsigned int>(
      std::min<std::size_t>((pixels - 1) / kThreads + 1, 65535));
  BndBoxKernel<<<blocks, kThreads, 0, stream>>>(image, boxes, pixels);
  return cudaGetLastError();
}

}  // namespace cv_cuda_pipeline
