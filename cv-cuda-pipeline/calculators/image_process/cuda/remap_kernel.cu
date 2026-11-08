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

#include <algorithm>
#include <cstdint>
#include <limits>

#include "cv-cuda-pipeline/calculators/image_process/cuda/remap_kernel.h"

namespace cv_cuda_pipeline {
namespace {

bool ValidStride(std::size_t stride, std::size_t row_bytes, int height) {
  const std::size_t max = std::numeric_limits<std::size_t>::max();
  return stride >= row_bytes &&
         static_cast<std::size_t>(height - 1) <= (max - row_bytes) / stride;
}

__global__ void RemapLinearKernel(RemapImageView image, std::size_t pixels) {
  for (std::size_t index =
           static_cast<std::size_t>(blockIdx.x) * blockDim.x + threadIdx.x;
       index < pixels;
       index += static_cast<std::size_t>(blockDim.x) * gridDim.x) {
    const int x = static_cast<int>(index % image.width);
    const int y = static_cast<int>(index / image.width);
    const auto* map_row = reinterpret_cast<const float2*>(
        reinterpret_cast<const std::uint8_t*>(image.map) +
        y * image.map_stride);
    const float2 pos = map_row[x];
    const float sx =
        fminf(fmaxf(pos.x, 0.f), static_cast<float>(image.width - 1));
    const float sy =
        fminf(fmaxf(pos.y, 0.f), static_cast<float>(image.height - 1));
    const int x0 = __float2int_rd(sx);
    const int y0 = __float2int_rd(sy);
    const int x1 = min(x0 + 1, image.width - 1);
    const int y1 = min(y0 + 1, image.height - 1);
    const float wx = sx - x0;
    const float wy = sy - y0;
    const std::uint8_t* row0 = image.src + y0 * image.src_stride;
    const std::uint8_t* row1 = image.src + y1 * image.src_stride;
    std::uint8_t* out =
        image.dst + y * image.dst_stride + static_cast<std::size_t>(x) * 3;
    for (int c = 0; c < 3; ++c) {
      const float value =
          row0[static_cast<std::size_t>(x0) * 3 + c] *
              ((1.f - wx) * (1.f - wy)) +
          row0[static_cast<std::size_t>(x1) * 3 + c] * (wx * (1.f - wy)) +
          row1[static_cast<std::size_t>(x0) * 3 + c] * ((1.f - wx) * wy) +
          row1[static_cast<std::size_t>(x1) * 3 + c] * (wx * wy);
      out[c] =
          static_cast<std::uint8_t>(max(0, min(255, __float2int_rn(value))));
    }
  }
}

}  // namespace

cudaError_t RemapLinear(RemapImageView image, cudaStream_t stream) {
  if (!image.src || !image.dst || !image.map || image.width <= 0 ||
      image.height <= 0 || image.width > (1 << 23) ||
      image.height > (1 << 23)) {
    return cudaErrorInvalidValue;
  }
  const std::size_t max = std::numeric_limits<std::size_t>::max();
  const std::size_t width = static_cast<std::size_t>(image.width);
  const std::size_t height = static_cast<std::size_t>(image.height);
  if (width > max / sizeof(float2) || width > max / 3 || width > max / height ||
      reinterpret_cast<std::uintptr_t>(image.map) % alignof(float2) != 0 ||
      image.map_stride % alignof(float2) != 0 ||
      !ValidStride(image.src_stride, width * 3, image.height) ||
      !ValidStride(image.dst_stride, width * 3, image.height) ||
      !ValidStride(image.map_stride, width * sizeof(float2), image.height)) {
    return cudaErrorInvalidValue;
  }
  const std::size_t pixels = width * height;
  constexpr unsigned int kThreads = 256;
  const unsigned int blocks = static_cast<unsigned int>(
      std::min<std::size_t>((pixels - 1) / kThreads + 1, 65535));
  RemapLinearKernel<<<blocks, kThreads, 0, stream>>>(image, pixels);
  return cudaGetLastError();
}

}  // namespace cv_cuda_pipeline
