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

#include "cv-cuda-pipeline/calculators/image_process/cuda/box_blur_kernel.h"

#include <cuda_runtime.h>

#include <algorithm>
#include <limits>

namespace cv_cuda_pipeline {
namespace {

struct Region {
  int left, top, width, height, radius;
};
struct Regions {
  Region values[3];
  int count;
};

bool ValidStride(std::size_t stride, std::size_t row_bytes, int height) {
  return stride >= row_bytes &&
         static_cast<std::size_t>(height - 1) <=
             (std::numeric_limits<std::size_t>::max() - row_bytes) / stride;
}

__global__ void BlurCrops(BoxBlurImageView image, Regions regions,
                          std::uint8_t* scratch) {
  __shared__ uchar3 crop[32][32];
  const Region box = regions.values[blockIdx.x];
  const int x = threadIdx.x, y = threadIdx.y;
  const int sx = static_cast<int>(min(
      static_cast<long long>(x / 32.0f * box.width + 0.5f + box.left),
      static_cast<long long>(image.width - 1)));
  const int sy = static_cast<int>(min(
      static_cast<long long>(y / 32.0f * box.height + 0.5f + box.top),
      static_cast<long long>(image.height - 1)));
  const auto* src = image.src + static_cast<std::size_t>(sy) * image.src_stride +
                    static_cast<std::size_t>(sx) * 3;
  crop[y][x] = make_uchar3(src[0], src[1], src[2]);
  __syncthreads();
  int sums[3] = {0, 0, 0};
  int count = 0;
  for (int iy = max(0, y - box.radius); iy <= min(31, y + box.radius); ++iy)
    for (int ix = max(0, x - box.radius); ix <= min(31, x + box.radius); ++ix) {
      const auto p = crop[iy][ix];
      sums[0] += p.x;
      sums[1] += p.y;
      sums[2] += p.z;
      ++count;
    }
  auto* out = scratch + (static_cast<std::size_t>(blockIdx.x) * 1024 +
                         static_cast<std::size_t>(y) * 32 + x) * 3;
  for (int c = 0; c < 3; ++c) out[c] = sums[c] / count;
}

__global__ void Compose(BoxBlurImageView image, Regions regions,
                        const std::uint8_t* scratch, std::size_t pixels) {
  for (std::size_t i = static_cast<std::size_t>(blockIdx.x) * blockDim.x +
                       threadIdx.x;
       i < pixels; i += static_cast<std::size_t>(gridDim.x) * blockDim.x) {
    const int x = static_cast<int>(i % image.width);
    const int y = static_cast<int>(i / image.width);
    const auto* src = image.src + static_cast<std::size_t>(y) * image.src_stride +
                      static_cast<std::size_t>(x) * 3;
    for (int j = 0; j < regions.count; ++j) {
      const auto box = regions.values[j];
      const long long dx = static_cast<long long>(x) - box.left;
      const long long dy = static_cast<long long>(y) - box.top;
      if (dx >= 0 && dx < box.width && dy >= 0 && dy < box.height) {
        const int cx = static_cast<int>(dx / static_cast<float>(box.width) * 32.0f);
        const int cy = static_cast<int>(dy / static_cast<float>(box.height) * 32.0f);
        if (cx < 32 && cy < 32)
          src = scratch + (static_cast<std::size_t>(j) * 1024 + cy * 32 + cx) * 3;
      }
    }
    auto* out = image.dst + static_cast<std::size_t>(y) * image.dst_stride +
                static_cast<std::size_t>(x) * 3;
    out[0] = src[0];
    out[1] = src[1];
    out[2] = src[2];
  }
}

}  // namespace

cudaError_t BoxBlur(BoxBlurImageView image, BoxBlurBoxes boxes,
                    std::uint8_t* scratch, cudaStream_t stream) {
  if (!image.src || !image.dst || !scratch || image.src == image.dst ||
      image.src == scratch || image.dst == scratch || image.width <= 0 ||
      image.height <= 0 || boxes.count < 0 || boxes.count > 3)
    return cudaErrorInvalidValue;
  const auto width = static_cast<std::size_t>(image.width);
  const auto height = static_cast<std::size_t>(image.height);
  const auto max = std::numeric_limits<std::size_t>::max();
  if (width > max / 3 || width > max / height ||
      !ValidStride(image.src_stride, width * 3, image.height) ||
      !ValidStride(image.dst_stride, width * 3, image.height))
    return cudaErrorInvalidValue;
  Regions regions{};
  for (int i = 0; i < boxes.count; ++i) {
    const auto box = boxes.boxes[i];
    if (box.kernel_size < 1 || box.width < 0 || box.height < 0)
      return cudaErrorInvalidValue;
    if (box.width < 3 || box.height < 3) continue;
    const auto left = std::clamp<long long>(box.x, 0, image.width - 1);
    const auto top = std::clamp<long long>(box.y, 0, image.height - 1);
    const auto right = std::clamp(left + box.width - 1LL, 0LL,
                                  static_cast<long long>(image.width - 1));
    const auto bottom = std::clamp(top + box.height - 1LL, 0LL,
                                   static_cast<long long>(image.height - 1));
    if (left == right || top == bottom) continue;
    regions.values[regions.count++] = {static_cast<int>(left), static_cast<int>(top),
        static_cast<int>(right - left), static_cast<int>(bottom - top),
        std::min(box.kernel_size / 2, 31)};
  }
  if (regions.count) {
    BlurCrops<<<regions.count, dim3(32, 32), 0, stream>>>(image, regions, scratch);
    const auto status = cudaGetLastError();
    if (status != cudaSuccess) return status;
  }
  const std::size_t pixels = width * height;
  constexpr unsigned int kThreads = 256;
  const auto blocks = static_cast<unsigned int>(
      std::min<std::size_t>((pixels - 1) / kThreads + 1, 65535));
  Compose<<<blocks, kThreads, 0, stream>>>(image, regions, scratch, pixels);
  return cudaGetLastError();
}

}  // namespace cv_cuda_pipeline
