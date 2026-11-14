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

#include "cv-cuda-pipeline/calculators/image_process/cuda/gaussian_kernel.h"

namespace cv_cuda_pipeline {
namespace {
bool ValidStride(std::size_t stride, std::size_t row, std::size_t height) {
  return stride >= row &&
         height - 1 <= (std::numeric_limits<std::size_t>::max() - row) / stride;
}

__global__ void Blur(GaussianImageView image, std::size_t count) {
  // Normalized 1D Gaussian coefficients for sigma=1.5 and radius=4.
  constexpr float weights[] = {0.0076144192f, 0.0360749697f, 0.1095860818f,
                               0.2134445419f, 0.2665599744f, 0.2134445419f,
                               0.1095860818f, 0.0360749697f, 0.0076144192f};
  for (std::size_t i =
           static_cast<std::size_t>(blockIdx.x) * blockDim.x + threadIdx.x;
       i < count; i += static_cast<std::size_t>(gridDim.x) * blockDim.x) {
    const auto x = static_cast<std::int64_t>(i % image.width);
    const auto y = static_cast<std::int64_t>(i / image.width);
    float sum[3] = {0.f, 0.f, 0.f};
    for (int dy = -4; dy <= 4; ++dy) {
      const auto sy = y + dy;
      if (sy < 0 || sy >= image.height) continue;
      for (int dx = -4; dx <= 4; ++dx) {
        const auto sx = x + dx;
        if (sx < 0 || sx >= image.width) continue;
        const auto* src = image.src +
                          static_cast<std::size_t>(sy) * image.src_stride +
                          static_cast<std::size_t>(sx) * 3;
        const float weight = weights[dy + 4] * weights[dx + 4];
        for (int c = 0; c < 3; ++c) sum[c] += src[c] * weight;
      }
    }
    auto* dst = image.dst + static_cast<std::size_t>(y) * image.dst_stride +
                static_cast<std::size_t>(x) * 3;
    for (int c = 0; c < 3; ++c)
      dst[c] = static_cast<std::uint8_t>(
          __float2int_rn(fminf(255.f, fmaxf(0.f, sum[c]))));
  }
}

__global__ void Blur5x5(GaussianImageView image, std::size_t count) {
  constexpr float weights[] = {0.0544886845f, 0.244201342f, 0.4026199589f,
                               0.244201342f, 0.0544886845f};
  for (std::size_t i =
           static_cast<std::size_t>(blockIdx.x) * blockDim.x + threadIdx.x;
       i < count; i += static_cast<std::size_t>(gridDim.x) * blockDim.x) {
    const auto x = static_cast<std::int64_t>(i % image.width);
    const auto y = static_cast<std::int64_t>(i / image.width);
    float sum[3] = {0.f, 0.f, 0.f};
    for (int dy = -2; dy <= 2; ++dy) {
      const auto sy = y + dy;
      if (sy < 0 || sy >= image.height) continue;
      for (int dx = -2; dx <= 2; ++dx) {
        const auto sx = x + dx;
        if (sx < 0 || sx >= image.width) continue;
        const auto* src = image.src +
                          static_cast<std::size_t>(sy) * image.src_stride +
                          static_cast<std::size_t>(sx) * 3;
        const float weight = weights[dy + 2] * weights[dx + 2];
        for (int c = 0; c < 3; ++c) sum[c] += src[c] * weight;
      }
    }
    auto* dst = image.dst + static_cast<std::size_t>(y) * image.dst_stride +
                static_cast<std::size_t>(x) * 3;
    for (int c = 0; c < 3; ++c)
      dst[c] = static_cast<std::uint8_t>(
          __float2int_rn(fminf(255.f, fmaxf(0.f, sum[c]))));
  }
}
}  // namespace
cudaError_t GaussianBlur(GaussianImageView image, cudaStream_t stream) {
  if (!image.src || !image.dst || image.src == image.dst || image.width <= 0 ||
      image.height <= 0)
    return cudaErrorInvalidValue;
  const auto w = static_cast<std::size_t>(image.width);
  const auto h = static_cast<std::size_t>(image.height);
  const auto limit = std::numeric_limits<std::size_t>::max();
  if (w > limit / 3 || w > limit / h ||
      !ValidStride(image.src_stride, w * 3, h) ||
      !ValidStride(image.dst_stride, w * 3, h))
    return cudaErrorInvalidValue;
  const auto count = w * h;
  const auto blocks = static_cast<unsigned int>(
      std::min<std::size_t>((count - 1) / 256 + 1, 65535));
  Blur<<<blocks, 256, 0, stream>>>(image, count);
  return cudaGetLastError();
}

cudaError_t GaussianBlur5x5(GaussianImageView image, cudaStream_t stream) {
  if (!image.src || !image.dst || image.src == image.dst || image.width <= 0 ||
      image.height <= 0)
    return cudaErrorInvalidValue;
  const auto w = static_cast<std::size_t>(image.width);
  const auto h = static_cast<std::size_t>(image.height);
  const auto limit = std::numeric_limits<std::size_t>::max();
  if (w > limit / 3 || w > limit / h ||
      !ValidStride(image.src_stride, w * 3, h) ||
      !ValidStride(image.dst_stride, w * 3, h))
    return cudaErrorInvalidValue;
  const auto count = w * h;
  const auto blocks = static_cast<unsigned int>(
      std::min<std::size_t>((count - 1) / 256 + 1, 65535));
  Blur5x5<<<blocks, 256, 0, stream>>>(image, count);
  return cudaGetLastError();
}
}  // namespace cv_cuda_pipeline
