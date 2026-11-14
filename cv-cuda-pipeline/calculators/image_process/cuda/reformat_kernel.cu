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

#include "cv-cuda-pipeline/calculators/image_process/cuda/reformat_kernel.h"

namespace cv_cuda_pipeline {
namespace {
bool ValidStride(std::size_t stride, std::size_t row, std::size_t height) {
  return stride >= row &&
         height - 1 <= (std::numeric_limits<std::size_t>::max() - row) / stride;
}

__global__ void HwcToPlanar(ReformatImageView image, std::size_t pixels) {
  for (std::size_t i =
           static_cast<std::size_t>(blockIdx.x) * blockDim.x + threadIdx.x;
       i < pixels; i += static_cast<std::size_t>(gridDim.x) * blockDim.x) {
    const std::size_t x = i % image.width, y = i / image.width;
    const auto* src = image.src + y * image.src_row_stride + x * 3;
    for (std::size_t c = 0; c < 3; ++c)
      image.dst[c * image.dst_plane_stride + y * image.dst_row_stride + x] =
          src[c];
  }
}

__global__ void PlanarToHwc(ReformatImageView image, std::size_t pixels) {
  for (std::size_t i =
           static_cast<std::size_t>(blockIdx.x) * blockDim.x + threadIdx.x;
       i < pixels; i += static_cast<std::size_t>(gridDim.x) * blockDim.x) {
    const std::size_t x = i % image.width, y = i / image.width;
    auto* dst = image.dst + y * image.dst_row_stride + x * 3;
    for (std::size_t c = 0; c < 3; ++c)
      dst[c] =
          image.src[c * image.src_plane_stride + y * image.src_row_stride + x];
  }
}

cudaError_t Validate(const ReformatImageView& image, bool to_planar) {
  if (!image.src || !image.dst || image.src == image.dst || image.width <= 0 ||
      image.height <= 0)
    return cudaErrorInvalidValue;
  const auto width = static_cast<std::size_t>(image.width);
  const auto height = static_cast<std::size_t>(image.height);
  const auto limit = std::numeric_limits<std::size_t>::max();
  if (width > limit / 3 || width > limit / height) return cudaErrorInvalidValue;
  const auto interleaved_row = width * 3;
  if (to_planar) {
    if (!ValidStride(image.src_row_stride, interleaved_row, height) ||
        !ValidStride(image.dst_row_stride, width, height) ||
        height > limit / image.dst_row_stride ||
        image.dst_plane_stride < image.dst_row_stride * height ||
        image.dst_plane_stride > limit / 3)
      return cudaErrorInvalidValue;
  } else {
    if (!ValidStride(image.src_row_stride, width, height) ||
        height > limit / image.src_row_stride ||
        image.src_plane_stride < image.src_row_stride * height ||
        image.src_plane_stride > limit / 3 ||
        !ValidStride(image.dst_row_stride, interleaved_row, height))
      return cudaErrorInvalidValue;
  }
  return cudaSuccess;
}
}  // namespace

cudaError_t HwcToChw(ReformatImageView image, cudaStream_t stream) {
  const auto status = Validate(image, true);
  if (status != cudaSuccess) return status;
  const auto pixels = static_cast<std::size_t>(image.width) * image.height;
  const auto blocks = static_cast<unsigned int>(
      std::min<std::size_t>((pixels - 1) / 256 + 1, 65535));
  HwcToPlanar<<<blocks, 256, 0, stream>>>(image, pixels);
  return cudaGetLastError();
}

cudaError_t ChwToHwc(ReformatImageView image, cudaStream_t stream) {
  const auto status = Validate(image, false);
  if (status != cudaSuccess) return status;
  const auto pixels = static_cast<std::size_t>(image.width) * image.height;
  const auto blocks = static_cast<unsigned int>(
      std::min<std::size_t>((pixels - 1) / 256 + 1, 65535));
  PlanarToHwc<<<blocks, 256, 0, stream>>>(image, pixels);
  return cudaGetLastError();
}
}  // namespace cv_cuda_pipeline
