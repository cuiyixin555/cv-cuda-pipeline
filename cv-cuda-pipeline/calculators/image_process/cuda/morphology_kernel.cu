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

#include "cv-cuda-pipeline/calculators/image_process/cuda/morphology_kernel.h"

namespace cv_cuda_pipeline {
namespace {
bool ValidStride(std::size_t stride, std::size_t row, std::size_t height) {
  return stride >= row &&
         height - 1 <= (std::numeric_limits<std::size_t>::max() - row) / stride;
}
template <bool Dilate>
__global__ void Morph(const std::uint8_t* src, std::size_t ss,
                      std::uint8_t* dst, std::size_t ds, int width, int height,
                      std::size_t count) {
  for (std::size_t i =
           static_cast<std::size_t>(blockIdx.x) * blockDim.x + threadIdx.x;
       i < count; i += static_cast<std::size_t>(gridDim.x) * blockDim.x) {
    const auto x = static_cast<std::int64_t>(i % width);
    const auto y = static_cast<std::int64_t>(i / width);
    int values[] = {Dilate ? 0 : 255, Dilate ? 0 : 255, Dilate ? 0 : 255};
    for (int dy = -2; dy <= 2; ++dy) {
      const auto sy = y + dy < 0 ? 0 : (y + dy >= height ? height - 1 : y + dy);
      for (int dx = -2; dx <= 2; ++dx) {
        const auto sx = x + dx < 0 ? 0 : (x + dx >= width ? width - 1 : x + dx);
        const auto* p = src + static_cast<std::size_t>(sy) * ss +
                        static_cast<std::size_t>(sx) * 3;
        for (int c = 0; c < 3; ++c)
          values[c] = Dilate ? max(values[c], static_cast<int>(p[c]))
                             : min(values[c], static_cast<int>(p[c]));
      }
    }
    auto* out = dst + static_cast<std::size_t>(y) * ds +
                static_cast<std::size_t>(x) * 3;
    for (int c = 0; c < 3; ++c) out[c] = static_cast<std::uint8_t>(values[c]);
  }
}
}  // namespace
cudaError_t MorphologyClose(MorphologyImageView image, cudaStream_t stream) {
  if (!image.src || !image.scratch || !image.dst ||
      image.src == image.scratch || image.src == image.dst ||
      image.scratch == image.dst || image.width <= 0 || image.height <= 0)
    return cudaErrorInvalidValue;
  const auto w = static_cast<std::size_t>(image.width);
  const auto h = static_cast<std::size_t>(image.height);
  const auto limit = std::numeric_limits<std::size_t>::max();
  if (w > limit / 3 || w > limit / h ||
      !ValidStride(image.src_stride, w * 3, h) ||
      !ValidStride(image.scratch_stride, w * 3, h) ||
      !ValidStride(image.dst_stride, w * 3, h))
    return cudaErrorInvalidValue;
  const auto count = w * h;
  const auto blocks = static_cast<unsigned int>(
      std::min<std::size_t>((count - 1) / 256 + 1, 65535));
  Morph<true><<<blocks, 256, 0, stream>>>(image.src, image.src_stride,
                                          image.scratch, image.scratch_stride,
                                          image.width, image.height, count);
  const auto status = cudaGetLastError();
  if (status != cudaSuccess) return status;
  Morph<false><<<blocks, 256, 0, stream>>>(image.scratch, image.scratch_stride,
                                           image.dst, image.dst_stride,
                                           image.width, image.height, count);
  return cudaGetLastError();
}
}  // namespace cv_cuda_pipeline
