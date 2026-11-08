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

#include "cv-cuda-pipeline/calculators/image_process/cuda/bilateral_filter_kernel.h"

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

__device__ int Reflect(long long coordinate, int length) {
  const long long period = 2LL * length;
  coordinate %= period;
  if (coordinate < 0) coordinate += period;
  return static_cast<int>(coordinate < length ? coordinate : period - 1 - coordinate);
}

__global__ void BilateralFilterKernel(BilateralFilterImageView image,
                                      std::size_t pixels) {
  for (std::size_t i = static_cast<std::size_t>(blockIdx.x) * blockDim.x +
                       threadIdx.x;
       i < pixels; i += static_cast<std::size_t>(gridDim.x) * blockDim.x) {
    const int x = static_cast<int>(i % image.width);
    const int y = static_cast<int>(i / image.width);
    const auto* center = image.src + static_cast<std::size_t>(y) * image.src_stride +
                         static_cast<std::size_t>(x) * 3;
    const int c0 = center[0], c1 = center[1], c2 = center[2];
    float total = 0, sum0 = 0, sum1 = 0, sum2 = 0;
    for (int dy = -4; dy <= 4; ++dy) {
      const int sy = Reflect(static_cast<long long>(y) + dy, image.height);
      for (int dx = -4; dx <= 4; ++dx) {
        const int distance = dx * dx + dy * dy;
        if (distance > 16) continue;
        const int sx = Reflect(static_cast<long long>(x) + dx, image.width);
        const auto* p = image.src + static_cast<std::size_t>(sy) * image.src_stride +
                        static_cast<std::size_t>(sx) * 3;
        const int color = abs(static_cast<int>(p[0]) - c0) +
                          abs(static_cast<int>(p[1]) - c1) +
                          abs(static_cast<int>(p[2]) - c2);
        const float weight = expf(-(distance + color * color) / 11250.0f);
        total += weight;
        sum0 += weight * p[0];
        sum1 += weight * p[1];
        sum2 += weight * p[2];
      }
    }
    auto* out = image.dst + static_cast<std::size_t>(y) * image.dst_stride +
                static_cast<std::size_t>(x) * 3;
    out[0] = static_cast<std::uint8_t>(__float2int_rn(sum0 / total));
    out[1] = static_cast<std::uint8_t>(__float2int_rn(sum1 / total));
    out[2] = static_cast<std::uint8_t>(__float2int_rn(sum2 / total));
  }
}

}  // namespace

cudaError_t BilateralFilter(BilateralFilterImageView image, cudaStream_t stream) {
  if (!image.src || !image.dst || image.width <= 0 || image.height <= 0)
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
  BilateralFilterKernel<<<blocks, kThreads, 0, stream>>>(image, pixels);
  return cudaGetLastError();
}

}  // namespace cv_cuda_pipeline
