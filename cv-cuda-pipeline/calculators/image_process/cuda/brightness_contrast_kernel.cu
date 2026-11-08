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

#include "cv-cuda-pipeline/calculators/image_process/cuda/brightness_contrast_kernel.h"

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

__global__ void BrightnessContrastKernel(BrightnessContrastImageView image,
                                         std::size_t pixels) {
  for (std::size_t i = static_cast<std::size_t>(blockIdx.x) * blockDim.x +
                       threadIdx.x;
       i < pixels; i += static_cast<std::size_t>(gridDim.x) * blockDim.x) {
    const std::size_t x = i % image.width, y = i / image.width;
    const auto* src = image.src + y * image.src_stride + x * 3;
    auto* dst = image.dst + y * image.dst_stride + x * 3;
    for (int c = 0; c < 3; ++c) {
      const float adjusted = fmaf(1.5f, fmaf(1.4f, src[c] - 127.0f, 127.0f), 20.0f);
      dst[c] = static_cast<std::uint8_t>(__float2int_rn(fminf(255.0f, fmaxf(0.0f, adjusted))));
    }
  }
}

}  // namespace

cudaError_t BrightnessContrast(BrightnessContrastImageView image,
                               cudaStream_t stream) {
  if (!image.src || !image.dst || image.src == image.dst ||
      image.width <= 0 || image.height <= 0)
    return cudaErrorInvalidValue;
  const std::size_t width = static_cast<std::size_t>(image.width);
  const std::size_t height = static_cast<std::size_t>(image.height);
  const auto max = std::numeric_limits<std::size_t>::max();
  if (width > max / 3 || width > max / height ||
      !ValidStride(image.src_stride, width * 3, image.height) ||
      !ValidStride(image.dst_stride, width * 3, image.height))
    return cudaErrorInvalidValue;
  const std::size_t pixels = width * height;
  constexpr unsigned int kThreads = 256;
  const auto blocks = static_cast<unsigned int>(
      std::min<std::size_t>((pixels - 1) / kThreads + 1, 65535));
  BrightnessContrastKernel<<<blocks, kThreads, 0, stream>>>(image, pixels);
  return cudaGetLastError();
}

}  // namespace cv_cuda_pipeline
