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

#include "cv-cuda-pipeline/calculators/image_process/cuda/laplacian_kernel.h"

namespace cv_cuda_pipeline {
namespace {
bool ValidStride(std::size_t stride, std::size_t row, std::size_t height) {
  return stride >= row &&
         height - 1 <= (std::numeric_limits<std::size_t>::max() - row) / stride;
}

__global__ void Laplacian(LaplacianImageView image, std::size_t pixels) {
  for (std::size_t i =
           static_cast<std::size_t>(blockIdx.x) * blockDim.x + threadIdx.x;
       i < pixels; i += static_cast<std::size_t>(gridDim.x) * blockDim.x) {
    const auto x = static_cast<int>(i % image.width);
    const auto y = static_cast<int>(i / image.width);
    const int left = x > 0 ? x - 1 : 0;
    const int right = x + 1 < image.width ? x + 1 : image.width - 1;
    const int top = y > 0 ? y - 1 : 0;
    const int bottom = y + 1 < image.height ? y + 1 : image.height - 1;
    const auto* tl = image.src +
                     static_cast<std::size_t>(top) * image.src_stride +
                     static_cast<std::size_t>(left) * 3;
    const auto* tr = image.src +
                     static_cast<std::size_t>(top) * image.src_stride +
                     static_cast<std::size_t>(right) * 3;
    const auto* bl = image.src +
                     static_cast<std::size_t>(bottom) * image.src_stride +
                     static_cast<std::size_t>(left) * 3;
    const auto* br = image.src +
                     static_cast<std::size_t>(bottom) * image.src_stride +
                     static_cast<std::size_t>(right) * 3;
    const auto* center = image.src +
                         static_cast<std::size_t>(y) * image.src_stride +
                         static_cast<std::size_t>(x) * 3;
    auto* out = image.dst + static_cast<std::size_t>(y) * image.dst_stride +
                static_cast<std::size_t>(x) * 3;
    for (int c = 0; c < 3; ++c) {
      const float response =
          2.f * (tl[c] + tr[c] + bl[c] + br[c]) - 8.f * center[c];
      out[c] = static_cast<std::uint8_t>(
          __float2int_rn(fminf(255.f, fmaxf(0.f, response))));
    }
  }
}
}  // namespace

cudaError_t Laplacian3x3(LaplacianImageView image, cudaStream_t stream) {
  if (!image.src || !image.dst ||
      reinterpret_cast<const void*>(image.src) ==
          reinterpret_cast<const void*>(image.dst) ||
      image.width <= 0 || image.height <= 0)
    return cudaErrorInvalidValue;
  const auto width = static_cast<std::size_t>(image.width);
  const auto height = static_cast<std::size_t>(image.height);
  const auto limit = std::numeric_limits<std::size_t>::max();
  if (width > limit / 3 || width > limit / height) return cudaErrorInvalidValue;
  const auto src_row = width * 3;
  const auto dst_row = width * 3;
  if (!ValidStride(image.src_stride, src_row, height) ||
      !ValidStride(image.dst_stride, dst_row, height))
    return cudaErrorInvalidValue;
  const auto count = width * height;
  const auto blocks = static_cast<unsigned int>(
      std::min<std::size_t>((count - 1) / 256 + 1, 65535));
  Laplacian<<<blocks, 256, 0, stream>>>(image, count);
  return cudaGetLastError();
}
}  // namespace cv_cuda_pipeline
