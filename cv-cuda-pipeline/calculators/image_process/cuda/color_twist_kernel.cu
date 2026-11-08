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
#include <cmath>
#include <limits>

#include "cv-cuda-pipeline/calculators/image_process/cuda/color_twist_kernel.h"

namespace cv_cuda_pipeline {
namespace {

bool ValidStride(std::size_t stride, std::size_t row, std::size_t height) {
  return stride >= row &&
         height - 1 <= (std::numeric_limits<std::size_t>::max() - row) / stride;
}

__global__ void ColorTwistKernel(ColorTwistImageView image,
                                 ColorTwistMatrix matrix, std::size_t pixels) {
  for (std::size_t i =
           static_cast<std::size_t>(blockIdx.x) * blockDim.x + threadIdx.x;
       i < pixels; i += static_cast<std::size_t>(gridDim.x) * blockDim.x) {
    const std::size_t x = i % image.width, y = i / image.width;
    const auto* src = image.src + y * image.src_stride + 3 * x;
    auto* dst = image.dst + y * image.dst_stride + 3 * x;
    const float r = src[0], g = src[1], b = src[2];
    for (int c = 0; c < 3; ++c) {
      const auto* m = matrix.rows[c];
      const float value = fmaf(m[2], b, fmaf(m[1], g, fmaf(m[0], r, m[3])));
      dst[c] = static_cast<std::uint8_t>(
          __float2int_rn(fminf(255.f, fmaxf(0.f, value))));
    }
  }
}
}  // namespace

cudaError_t ColorTwist(ColorTwistImageView image, ColorTwistMatrix matrix,
                       cudaStream_t stream) {
  if (!image.src || !image.dst || image.src == image.dst || image.width <= 0 ||
      image.height <= 0)
    return cudaErrorInvalidValue;
  for (const auto& row : matrix.rows) {
    double bound = 0;
    for (int c = 0; c < 4; ++c) {
      if (!std::isfinite(row[c])) return cudaErrorInvalidValue;
      bound += std::abs(static_cast<double>(row[c])) * (c == 3 ? 1 : 255);
    }
    if (bound > static_cast<double>(std::numeric_limits<float>::max()) / 2)
      return cudaErrorInvalidValue;
  }
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
  ColorTwistKernel<<<blocks, 256, 0, stream>>>(image, matrix, pixels);
  return cudaGetLastError();
}
}  // namespace cv_cuda_pipeline
