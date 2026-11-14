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

#include "cv-cuda-pipeline/calculators/image_process/cuda/inpaint_kernel.h"

namespace cv_cuda_pipeline {
namespace {
constexpr int kRadius = 4;
constexpr int kChannels = 3;
constexpr unsigned int kThreads = 256;

bool ValidStride(std::size_t stride, std::size_t row_bytes, int height) {
  return stride >= row_bytes &&
         static_cast<std::size_t>(height - 1) <=
             (std::numeric_limits<std::size_t>::max() - row_bytes) / stride;
}

__global__ void InpaintKernel(InpaintImageView image, std::size_t pixels) {
  for (std::size_t index =
           static_cast<std::size_t>(blockIdx.x) * blockDim.x + threadIdx.x;
       index < pixels;
       index += static_cast<std::size_t>(gridDim.x) * blockDim.x) {
    const int x = static_cast<int>(index % image.width);
    const int y = static_cast<int>(index / image.width);
    const auto* mask_row =
        image.mask + static_cast<std::size_t>(y) * image.mask_stride;
    const auto* src_pixel =
        image.src + static_cast<std::size_t>(y) * image.src_stride + x * 3;
    auto* dst_pixel =
        image.dst + static_cast<std::size_t>(y) * image.dst_stride + x * 3;
    if (mask_row[x] == 0) {
      for (int c = 0; c < kChannels; ++c) dst_pixel[c] = src_pixel[c];
      continue;
    }

    float sums[kChannels] = {0.0f, 0.0f, 0.0f};
    float weight_sum = 0.0f;
    for (int dy = -kRadius; dy <= kRadius; ++dy) {
      const int sy = y + dy;
      if (sy < 0 || sy >= image.height) continue;
      const auto* neighbor_mask =
          image.mask + static_cast<std::size_t>(sy) * image.mask_stride;
      const auto* neighbor_row =
          image.src + static_cast<std::size_t>(sy) * image.src_stride;
      for (int dx = -kRadius; dx <= kRadius; ++dx) {
        const int sx = x + dx;
        if ((dx == 0 && dy == 0) || sx < 0 || sx >= image.width ||
            neighbor_mask[sx] != 0)
          continue;
        const float weight = 1.0f / (1.0f + dx * dx + dy * dy);
        const auto* neighbor = neighbor_row + sx * kChannels;
        for (int c = 0; c < kChannels; ++c) sums[c] += weight * neighbor[c];
        weight_sum += weight;
      }
    }
    if (weight_sum == 0.0f) {
      for (int c = 0; c < kChannels; ++c) dst_pixel[c] = src_pixel[c];
    } else {
      for (int c = 0; c < kChannels; ++c)
        dst_pixel[c] =
            static_cast<std::uint8_t>(__float2int_rn(sums[c] / weight_sum));
    }
  }
}
}  // namespace

cudaError_t Inpaint(InpaintImageView image, cudaStream_t stream) {
  if (!image.src || !image.mask || !image.dst || image.width <= 0 ||
      image.height <= 0)
    return cudaErrorInvalidValue;
  const std::size_t width = static_cast<std::size_t>(image.width);
  const std::size_t height = static_cast<std::size_t>(image.height);
  const auto max = std::numeric_limits<std::size_t>::max();
  if (width > max / 3 || width > max / height) return cudaErrorInvalidValue;
  const std::size_t image_row = width * 3;
  if (!ValidStride(image.src_stride, image_row, image.height) ||
      !ValidStride(image.mask_stride, width, image.height) ||
      !ValidStride(image.dst_stride, image_row, image.height))
    return cudaErrorInvalidValue;
  const std::size_t pixels = width * height;
  const unsigned int blocks = static_cast<unsigned int>(
      std::min<std::size_t>((pixels - 1) / kThreads + 1, 65535));
  InpaintKernel<<<blocks, kThreads, 0, stream>>>(image, pixels);
  return cudaGetLastError();
}

}  // namespace cv_cuda_pipeline
