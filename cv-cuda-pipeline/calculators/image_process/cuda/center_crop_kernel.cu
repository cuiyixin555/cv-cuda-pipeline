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

#include <algorithm>
#include <limits>

namespace cv_cuda_pipeline {
namespace {

bool ValidStride(std::size_t stride, std::size_t row_bytes, int height) {
  return stride >= row_bytes &&
         static_cast<std::size_t>(height - 1) <=
             (std::numeric_limits<std::size_t>::max() - row_bytes) / stride;
}

__global__ void CenterCropKernel(CenterCropImageView image, std::size_t pixels) {
  const std::size_t left = (image.src_width - image.dst_width) / 2;
  const std::size_t top = (image.src_height - image.dst_height) / 2;
  for (std::size_t i = static_cast<std::size_t>(blockIdx.x) * blockDim.x +
                       threadIdx.x;
       i < pixels; i += static_cast<std::size_t>(gridDim.x) * blockDim.x) {
    const std::size_t x = i % image.dst_width, y = i / image.dst_width;
    const auto* src = image.src + (y + top) * image.src_stride + (x + left) * 3;
    auto* dst = image.dst + y * image.dst_stride + x * 3;
    dst[0] = src[0];
    dst[1] = src[1];
    dst[2] = src[2];
  }
}

}  // namespace

cudaError_t CenterCrop(CenterCropImageView image, cudaStream_t stream) {
  if (!image.src || !image.dst || image.src == image.dst ||
      image.src_width <= 0 || image.src_height <= 0 || image.dst_width <= 0 ||
      image.dst_height <= 0 || image.dst_width > image.src_width ||
      image.dst_height > image.src_height)
    return cudaErrorInvalidValue;
  const auto sw = static_cast<std::size_t>(image.src_width);
  const auto dw = static_cast<std::size_t>(image.dst_width);
  const auto dh = static_cast<std::size_t>(image.dst_height);
  const auto max = std::numeric_limits<std::size_t>::max();
  if (sw > max / 3 || dw > max / 3 || dw > max / dh ||
      !ValidStride(image.src_stride, sw * 3, image.src_height) ||
      !ValidStride(image.dst_stride, dw * 3, image.dst_height))
    return cudaErrorInvalidValue;
  const std::size_t pixels = dw * dh;
  constexpr unsigned int kThreads = 256;
  const auto blocks = static_cast<unsigned int>(
      std::min<std::size_t>((pixels - 1) / kThreads + 1, 65535));
  CenterCropKernel<<<blocks, kThreads, 0, stream>>>(image, pixels);
  return cudaGetLastError();
}

}  // namespace cv_cuda_pipeline
