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

#include "cv-cuda-pipeline/calculators/image_process/cuda/median_blur_kernel.h"

namespace cv_cuda_pipeline {
namespace {
bool ValidStride(std::size_t stride, std::size_t row, std::size_t height) {
  return stride >= row &&
         height - 1 <= (std::numeric_limits<std::size_t>::max() - row) / stride;
}

__device__ std::uint8_t SelectMedian(std::uint8_t* values) {
  int first = 0, last = 49;
  constexpr int target = 24;
  while (last - first > 1) {
    const int middle = first + (last - first) / 2;
    const auto a = values[first], b = values[middle], c = values[last - 1];
    const auto pivot =
        a < b ? (b < c ? b : (a < c ? c : a)) : (a < c ? a : (b < c ? c : b));
    int lower = first, current = first, upper = last;
    while (current < upper) {
      if (values[current] < pivot) {
        const auto temporary = values[lower];
        values[lower++] = values[current];
        values[current++] = temporary;
      } else if (values[current] > pivot) {
        const auto temporary = values[--upper];
        values[upper] = values[current];
        values[current] = temporary;
      } else {
        ++current;
      }
    }
    if (target < lower) {
      last = lower;
    } else if (target >= upper) {
      first = upper;
    } else {
      return pivot;
    }
  }
  return values[first];
}

__global__ void MedianBlur(MedianBlurImageView image, std::size_t pixels) {
  for (std::size_t i =
           static_cast<std::size_t>(blockIdx.x) * blockDim.x + threadIdx.x;
       i < pixels; i += static_cast<std::size_t>(gridDim.x) * blockDim.x) {
    const int x = static_cast<int>(i % image.width);
    const int y = static_cast<int>(i / image.width);
    auto* out = image.dst + static_cast<std::size_t>(y) * image.dst_stride +
                static_cast<std::size_t>(x) * 3;
    for (int c = 0; c < 3; ++c) {
      std::uint8_t values[49];
      int index = 0;
      for (int dy = -3; dy <= 3; ++dy) {
        const int sy = max(0, min(image.height - 1, y + dy));
        for (int dx = -3; dx <= 3; ++dx) {
          const int sx = max(0, min(image.width - 1, x + dx));
          values[index++] =
              image.src[static_cast<std::size_t>(sy) * image.src_stride +
                        static_cast<std::size_t>(sx) * 3 + c];
        }
      }
      out[c] = SelectMedian(values);
    }
  }
}
}  // namespace

cudaError_t MedianBlur7x7(MedianBlurImageView image, cudaStream_t stream) {
  if (!image.src || !image.dst || image.src == image.dst || image.width <= 0 ||
      image.height <= 0)
    return cudaErrorInvalidValue;
  const auto width = static_cast<std::size_t>(image.width);
  const auto height = static_cast<std::size_t>(image.height);
  const auto limit = std::numeric_limits<std::size_t>::max();
  if (width > limit / 3 || width > limit / height ||
      !ValidStride(image.src_stride, width * 3, height) ||
      !ValidStride(image.dst_stride, width * 3, height))
    return cudaErrorInvalidValue;
  const auto pixels = width * height;
  const auto blocks = static_cast<unsigned int>(
      std::min<std::size_t>((pixels - 1) / 128 + 1, 65535));
  MedianBlur<<<blocks, 128, 0, stream>>>(image, pixels);
  return cudaGetLastError();
}
}  // namespace cv_cuda_pipeline
