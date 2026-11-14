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
#include <cmath>
#include <limits>

#include "cv-cuda-pipeline/calculators/image_process/cuda/warp_kernel.h"

namespace cv_cuda_pipeline {
namespace {
__device__ float Sample(const WarpImageView& image, int x, int y, int channel) {
  if (x < 0 || x >= image.width || y < 0 || y >= image.height) return 0.0f;
  return image.src[static_cast<std::size_t>(y) * image.src_stride +
                   static_cast<std::size_t>(x) * image.channels + channel];
}

__global__ void WarpKernel(WarpImageView image, WarpTransform transform,
                           std::size_t pixels) {
  for (std::size_t i =
           static_cast<std::size_t>(blockIdx.x) * blockDim.x + threadIdx.x;
       i < pixels; i += static_cast<std::size_t>(gridDim.x) * blockDim.x) {
    const int x = static_cast<int>(i % image.width);
    const int y = static_cast<int>(i / image.width);
    const float denominator =
        transform.values[6] * x + transform.values[7] * y + transform.values[8];
    auto* out = image.dst + static_cast<std::size_t>(y) * image.dst_stride +
                static_cast<std::size_t>(x) * image.channels;
    if (!isfinite(denominator) || fabsf(denominator) < 1.0e-8f) {
      for (int c = 0; c < image.channels; ++c) out[c] = 0;
      continue;
    }
    const float source_x = (transform.values[0] * x + transform.values[1] * y +
                            transform.values[2]) /
                           denominator;
    const float source_y = (transform.values[3] * x + transform.values[4] * y +
                            transform.values[5]) /
                           denominator;
    if (!isfinite(source_x) || !isfinite(source_y) || source_x <= -1.0f ||
        source_x >= image.width || source_y <= -1.0f ||
        source_y >= image.height) {
      for (int c = 0; c < image.channels; ++c) out[c] = 0;
      continue;
    }
    const int x0 = __float2int_rd(source_x);
    const int y0 = __float2int_rd(source_y);
    const int x1 = x0 + 1;
    const int y1 = y0 + 1;
    const float wx = source_x - x0;
    const float wy = source_y - y0;
    for (int c = 0; c < image.channels; ++c) {
      const float value =
          Sample(image, x0, y0, c) * ((1.0f - wx) * (1.0f - wy)) +
          Sample(image, x1, y0, c) * (wx * (1.0f - wy)) +
          Sample(image, x0, y1, c) * ((1.0f - wx) * wy) +
          Sample(image, x1, y1, c) * (wx * wy);
      out[c] =
          static_cast<std::uint8_t>(max(0, min(255, __float2int_rn(value))));
    }
  }
}

bool ValidStride(std::size_t stride, std::size_t row, std::size_t height) {
  return stride >= row &&
         height - 1 <= (std::numeric_limits<std::size_t>::max() - row) / stride;
}
}  // namespace

cudaError_t WarpLinear(WarpImageView image, WarpTransform transform,
                       cudaStream_t stream) {
  if (!image.src || !image.dst || image.src == image.dst || image.width <= 0 ||
      image.height <= 0 ||
      !(image.channels == 1 || image.channels == 3 || image.channels == 4))
    return cudaErrorInvalidValue;
  for (float value : transform.values)
    if (!std::isfinite(value)) return cudaErrorInvalidValue;
  const auto width = static_cast<std::size_t>(image.width);
  const auto height = static_cast<std::size_t>(image.height);
  const auto channels = static_cast<std::size_t>(image.channels);
  const auto limit = std::numeric_limits<std::size_t>::max();
  if (width > limit / channels || width > limit / height ||
      !ValidStride(image.src_stride, width * channels, height) ||
      !ValidStride(image.dst_stride, width * channels, height))
    return cudaErrorInvalidValue;
  const auto pixels = width * height;
  const auto blocks = static_cast<unsigned int>(
      std::min<std::size_t>((pixels - 1) / 256 + 1, 65535));
  WarpKernel<<<blocks, 256, 0, stream>>>(image, transform, pixels);
  return cudaGetLastError();
}
}  // namespace cv_cuda_pipeline
