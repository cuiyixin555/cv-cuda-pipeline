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

#include "cv-cuda-pipeline/calculators/image_process/cuda/clahe_kernel.h"

namespace cv_cuda_pipeline {
namespace {

bool ValidImage(ClaheImageView image, std::size_t src_channels) {
  if (!image.src || !image.dst || image.src == image.dst || image.width <= 0 ||
      image.height <= 0)
    return false;
  const auto w = static_cast<std::size_t>(image.width);
  const auto h = static_cast<std::size_t>(image.height);
  const auto max = std::numeric_limits<std::size_t>::max();
  if (w > max / src_channels || w > max / h) return false;
  const std::size_t src_row = w * src_channels;
  return image.src_stride >= src_row && image.dst_stride >= w &&
         h - 1 <= (max - src_row) / image.src_stride &&
         h - 1 <= (max - w) / image.dst_stride;
}

__device__ int Reflect101(long long p, int length) {
  if (length == 1) return 0;
  const long long period = 2LL * (length - 1);
  p %= period;
  return static_cast<int>(p < length ? p : period - p);
}

__global__ void ToGray(ClaheImageView image, std::size_t count) {
  for (std::size_t i =
           static_cast<std::size_t>(blockIdx.x) * blockDim.x + threadIdx.x;
       i < count; i += static_cast<std::size_t>(gridDim.x) * blockDim.x) {
    const std::size_t x = i % image.width, y = i / image.width;
    const auto* p = image.src + y * image.src_stride + 3 * x;
    image.dst[y * image.dst_stride + x] =
        (p[0] * 9798 + p[1] * 19235 + p[2] * 3735 + 16384) >> 15;
  }
}

__global__ void BuildLuts(ClaheImageView image, std::uint8_t* luts, int tile_w,
                          int tile_h, unsigned int area) {
  __shared__ unsigned int hist[256];
  __shared__ unsigned int excess;
  const unsigned int c = threadIdx.x;
  hist[c] = 0;
  if (c == 0) excess = 0;
  __syncthreads();
  const int tx = blockIdx.x, ty = blockIdx.y;
  for (unsigned int i = c; i < area; i += blockDim.x) {
    const int x = Reflect101(static_cast<long long>(tx) * tile_w + i % tile_w,
                             image.width);
    const int y = Reflect101(static_cast<long long>(ty) * tile_h + i / tile_w,
                             image.height);
    atomicAdd(
        &hist[image.src[static_cast<std::size_t>(y) * image.src_stride + x]],
        1);
  }
  __syncthreads();
  const unsigned int limit = max(area / 128, 1U);
  if (hist[c] > limit) {
    atomicAdd(&excess, hist[c] - limit);
    hist[c] = limit;
  }
  __syncthreads();
  const unsigned int remainder = excess % 256;
  hist[c] += excess / 256;
  if (remainder) {
    const unsigned int step = 256 / remainder;
    if (c % step == 0 && c / step < remainder) ++hist[c];
  }
  __syncthreads();
  // Inclusive scan in shared memory; each iteration reads before any write.
  for (unsigned int offset = 1; offset < 256; offset *= 2) {
    const unsigned int add = c >= offset ? hist[c - offset] : 0;
    __syncthreads();
    hist[c] += add;
    __syncthreads();
  }
  const auto scaled =
      (static_cast<unsigned long long>(hist[c]) * 255 + area / 2) / area;
  luts[(ty * 8 + tx) * 256 + c] =
      static_cast<std::uint8_t>(min(scaled, 255ULL));
}

__global__ void Apply(ClaheImageView image, const std::uint8_t* luts,
                      int tile_w, int tile_h, std::size_t count) {
  for (std::size_t i =
           static_cast<std::size_t>(blockIdx.x) * blockDim.x + threadIdx.x;
       i < count; i += static_cast<std::size_t>(gridDim.x) * blockDim.x) {
    const std::size_t x = i % image.width, y = i / image.width;
    const float gx =
        fminf(fmaxf(static_cast<float>(x) * (1.f / tile_w) - .5f, 0.f), 7.f);
    const float gy =
        fminf(fmaxf(static_cast<float>(y) * (1.f / tile_h) - .5f, 0.f), 7.f);
    const int x0 = static_cast<int>(gx), y0 = static_cast<int>(gy);
    const int x1 = min(x0 + 1, 7), y1 = min(y0 + 1, 7);
    const float fx = gx - x0, fy = gy - y0;
    const auto v = image.src[y * image.src_stride + x];
    const float result = (1 - fx) * (1 - fy) * luts[(y0 * 8 + x0) * 256 + v] +
                         fx * (1 - fy) * luts[(y0 * 8 + x1) * 256 + v] +
                         (1 - fx) * fy * luts[(y1 * 8 + x0) * 256 + v] +
                         fx * fy * luts[(y1 * 8 + x1) * 256 + v];
    image.dst[y * image.dst_stride + x] =
        static_cast<std::uint8_t>(fminf(result + .5f, 255.f));
  }
}

unsigned int Blocks(std::size_t pixels) {
  return static_cast<unsigned int>(
      std::min<std::size_t>((pixels - 1) / 256 + 1, 65535));
}
}  // namespace

cudaError_t ClaheRgbToGray(ClaheImageView image, cudaStream_t stream) {
  if (!ValidImage(image, 3)) return cudaErrorInvalidValue;
  const auto count = static_cast<std::size_t>(image.width) * image.height;
  ToGray<<<Blocks(count), 256, 0, stream>>>(image, count);
  return cudaGetLastError();
}

cudaError_t Clahe(ClaheImageView image, std::uint8_t* scratch,
                  cudaStream_t stream) {
  if (!ValidImage(image, 1) || !scratch || scratch == image.src ||
      scratch == image.dst)
    return cudaErrorInvalidValue;
  const auto tile_w = (static_cast<std::size_t>(image.width) + 7) / 8;
  const auto tile_h = (static_cast<std::size_t>(image.height) + 7) / 8;
  const auto max_area = std::numeric_limits<unsigned int>::max() - 255U;
  if (tile_w > max_area / tile_h) return cudaErrorInvalidValue;
  const auto area = static_cast<unsigned int>(tile_w * tile_h);
  BuildLuts<<<dim3(8, 8), 256, 0, stream>>>(image, scratch, tile_w, tile_h,
                                            area);
  auto status = cudaGetLastError();
  if (status != cudaSuccess) return status;
  const auto count = static_cast<std::size_t>(image.width) * image.height;
  Apply<<<Blocks(count), 256, 0, stream>>>(image, scratch, tile_w, tile_h,
                                           count);
  return cudaGetLastError();
}
}  // namespace cv_cuda_pipeline
