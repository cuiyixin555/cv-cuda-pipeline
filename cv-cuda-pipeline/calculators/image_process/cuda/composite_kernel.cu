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

#include <cuda_runtime.h>

#include <algorithm>
#include <limits>

#include "cv-cuda-pipeline/calculators/image_process/cuda/composite_kernel.h"

namespace cv_cuda_pipeline {
namespace {

bool ValidStride(std::size_t stride, std::size_t row, std::size_t height) {
  return stride >= row &&
         height - 1 <= (std::numeric_limits<std::size_t>::max() - row) / stride;
}

__global__ void CompositeKernel(CompositeImageView image, std::size_t pixels) {
  for (std::size_t i =
           static_cast<std::size_t>(blockIdx.x) * blockDim.x + threadIdx.x;
       i < pixels; i += static_cast<std::size_t>(gridDim.x) * blockDim.x) {
    const std::size_t x = i % image.width, y = i / image.width;
    const auto* fg = image.foreground + y * image.foreground_stride + x * 3;
    const auto* bg = image.background + y * image.background_stride + x * 3;
    const unsigned int alpha = image.mask[y * image.mask_stride + x];
    auto* out =
        image.output + y * image.output_stride + x * image.output_channels;
    for (int c = 0; c < 3; ++c) {
      const unsigned int sum = fg[c] * alpha + bg[c] * (255 - alpha);
      out[c] = static_cast<std::uint8_t>((sum + 127) / 255);
    }
    if (image.output_channels == 4) out[3] = 255;
  }
}
}  // namespace

cudaError_t Composite(CompositeImageView image, cudaStream_t stream) {
  if (!image.foreground || !image.background || !image.mask || !image.output ||
      image.output == image.foreground || image.output == image.background ||
      image.output == image.mask || image.width <= 0 || image.height <= 0 ||
      (image.output_channels != 3 && image.output_channels != 4))
    return cudaErrorInvalidValue;
  const auto width = static_cast<std::size_t>(image.width);
  const auto height = static_cast<std::size_t>(image.height);
  const auto limit = std::numeric_limits<std::size_t>::max();
  if (width > limit / 4 || width > limit / height ||
      !ValidStride(image.foreground_stride, width * 3, height) ||
      !ValidStride(image.background_stride, width * 3, height) ||
      !ValidStride(image.mask_stride, width, height) ||
      !ValidStride(image.output_stride, width * image.output_channels, height))
    return cudaErrorInvalidValue;
  const auto pixels = width * height;
  const auto blocks = static_cast<unsigned int>(
      std::min<std::size_t>((pixels - 1) / 256 + 1, 65535));
  CompositeKernel<<<blocks, 256, 0, stream>>>(image, pixels);
  return cudaGetLastError();
}
}  // namespace cv_cuda_pipeline
