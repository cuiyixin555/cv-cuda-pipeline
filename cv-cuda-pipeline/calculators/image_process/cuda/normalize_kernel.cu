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
#include <cstdint>
#include <limits>

#include "cv-cuda-pipeline/calculators/image_process/cuda/normalize_kernel.h"

namespace cv_cuda_pipeline {
namespace {
bool ValidStride(std::size_t stride, std::size_t row, std::size_t height) {
  return stride >= row &&
         height - 1 <= (std::numeric_limits<std::size_t>::max() - row) / stride;
}

__global__ void Normalize(NormalizeImageView image, std::size_t pixels) {
  constexpr float means[] = {103.53f, 116.28f, 123.675f};
  constexpr float stddevs[] = {57.375f, 57.12f, 58.395f};
  for (std::size_t i =
           static_cast<std::size_t>(blockIdx.x) * blockDim.x + threadIdx.x;
       i < pixels; i += static_cast<std::size_t>(gridDim.x) * blockDim.x) {
    const std::size_t x = i % image.width, y = i / image.width;
    const auto* src = image.src + y * image.src_stride + x * 3;
    auto* dst =
        reinterpret_cast<float*>(reinterpret_cast<std::uint8_t*>(image.dst) +
                                 y * image.dst_stride) +
        x * 3;
    for (int c = 0; c < 3; ++c) {
      const float variance = stddevs[c] * stddevs[c] + 1.e-5f;
      dst[c] = (static_cast<float>(src[c]) - means[c]) * rsqrtf(variance);
    }
  }
}
}  // namespace

cudaError_t NormalizeImageNet(NormalizeImageView image, cudaStream_t stream) {
  if (!image.src || !image.dst ||
      reinterpret_cast<const void*>(image.src) ==
          reinterpret_cast<const void*>(image.dst) ||
      image.width <= 0 || image.height <= 0)
    return cudaErrorInvalidValue;
  if (reinterpret_cast<std::uintptr_t>(image.dst) % alignof(float) != 0 ||
      image.dst_stride % alignof(float) != 0)
    return cudaErrorInvalidValue;
  const auto width = static_cast<std::size_t>(image.width);
  const auto height = static_cast<std::size_t>(image.height);
  const auto limit = std::numeric_limits<std::size_t>::max();
  if (width > limit / 3 || width > limit / (3 * sizeof(float)) ||
      width > limit / height)
    return cudaErrorInvalidValue;
  if (!ValidStride(image.src_stride, width * 3, height) ||
      !ValidStride(image.dst_stride, width * 3 * sizeof(float), height))
    return cudaErrorInvalidValue;
  const auto pixels = width * height;
  const auto blocks = static_cast<unsigned int>(
      std::min<std::size_t>((pixels - 1) / 256 + 1, 65535));
  Normalize<<<blocks, 256, 0, stream>>>(image, pixels);
  return cudaGetLastError();
}
}  // namespace cv_cuda_pipeline
