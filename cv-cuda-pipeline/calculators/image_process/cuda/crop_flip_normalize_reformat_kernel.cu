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

#include "cv-cuda-pipeline/calculators/image_process/cuda/crop_flip_normalize_reformat_kernel.h"

namespace cv_cuda_pipeline {
namespace {
__global__ void CropFlipNormalize(CropFlipNormalizeView image,
                                  std::size_t count) {
  const float base[] = {.485f, .456f, .406f};
  const float scale[] = {.229f, .224f, .225f};
  for (std::size_t i =
           static_cast<std::size_t>(blockIdx.x) * blockDim.x + threadIdx.x;
       i < count; i += static_cast<std::size_t>(gridDim.x) * blockDim.x) {
    const std::size_t x = i % image.width, y = i / image.width;
    const auto* src =
        image.src + (y + image.y) * image.src_stride +
        (static_cast<std::size_t>(image.x) + image.width - 1 - x) * 3;
    for (int c = 0; c < 3; ++c) {
      auto* row = reinterpret_cast<float*>(
          reinterpret_cast<std::uint8_t*>(image.dst) +
          c * image.dst_plane_stride + y * image.dst_row_stride);
      row[x] = (src[c] - base[c]) * (1.f / sqrtf(scale[c] * scale[c] + 1e-8f)) *
               (1.f / 255.f);
    }
  }
}
}  // namespace
cudaError_t CropFlipNormalizeReformat(CropFlipNormalizeView image,
                                      cudaStream_t stream) {
  if (!image.src || !image.dst ||
      reinterpret_cast<const void*>(image.src) == image.dst ||
      image.src_width <= 0 || image.src_height <= 0 || image.x < 0 ||
      image.y < 0 || image.width <= 0 || image.height <= 0 ||
      image.x >= image.src_width || image.y >= image.src_height ||
      image.width > image.src_width - image.x ||
      image.height > image.src_height - image.y ||
      reinterpret_cast<std::uintptr_t>(image.dst) % alignof(float) ||
      image.dst_row_stride % alignof(float) ||
      image.dst_plane_stride % alignof(float))
    return cudaErrorInvalidValue;
  const auto sw = static_cast<std::size_t>(image.src_width);
  const auto sh = static_cast<std::size_t>(image.src_height);
  const auto w = static_cast<std::size_t>(image.width);
  const auto h = static_cast<std::size_t>(image.height);
  const auto max = std::numeric_limits<std::size_t>::max();
  if (sw > max / 3 || w > max / sizeof(float) || w > max / h ||
      image.src_stride < sw * 3 || sh - 1 > (max - sw * 3) / image.src_stride ||
      image.dst_row_stride < w * sizeof(float) ||
      h - 1 > (max - w * sizeof(float)) / image.dst_row_stride)
    return cudaErrorInvalidValue;
  const auto plane = (h - 1) * image.dst_row_stride + w * sizeof(float);
  if (image.dst_plane_stride < plane ||
      image.dst_plane_stride > (max - plane) / 2)
    return cudaErrorInvalidValue;
  const auto pixels = w * h;
  const auto blocks = static_cast<unsigned int>(
      std::min<std::size_t>((pixels - 1) / 256 + 1, 65535));
  CropFlipNormalize<<<blocks, 256, 0, stream>>>(image, pixels);
  return cudaGetLastError();
}
}  // namespace cv_cuda_pipeline
