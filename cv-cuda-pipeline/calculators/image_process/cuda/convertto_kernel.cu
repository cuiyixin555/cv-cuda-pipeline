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

#include "cv-cuda-pipeline/calculators/image_process/cuda/convertto_kernel.h"

namespace cv_cuda_pipeline {
namespace {
bool ValidStride(std::size_t stride, std::size_t row, std::size_t height) {
  return stride >= row &&
         height - 1 <= (std::numeric_limits<std::size_t>::max() - row) / stride;
}
__global__ void ToFloat(ConvertToImageView image, std::size_t samples) {
  for (std::size_t i =
           static_cast<std::size_t>(blockIdx.x) * blockDim.x + threadIdx.x;
       i < samples; i += static_cast<std::size_t>(gridDim.x) * blockDim.x) {
    const std::size_t row = static_cast<std::size_t>(image.width) * 3;
    const std::size_t y = i / row, x = i % row;
    auto* out = reinterpret_cast<float*>(
        reinterpret_cast<std::uint8_t*>(image.scratch) +
        y * image.scratch_stride);
    out[x] = image.src[y * image.src_stride + x] * (1.f / 255.f);
  }
}
__global__ void ToByte(ConvertToImageView image, std::size_t samples) {
  for (std::size_t i =
           static_cast<std::size_t>(blockIdx.x) * blockDim.x + threadIdx.x;
       i < samples; i += static_cast<std::size_t>(gridDim.x) * blockDim.x) {
    const std::size_t row = static_cast<std::size_t>(image.width) * 3;
    const std::size_t y = i / row, x = i % row;
    const auto* src = reinterpret_cast<const float*>(
        reinterpret_cast<const std::uint8_t*>(image.scratch) +
        y * image.scratch_stride);
    image.dst[y * image.dst_stride + x] = static_cast<std::uint8_t>(
        __float2int_rn(fminf(255.f, fmaxf(0.f, src[x] * 255.f))));
  }
}
}  // namespace
cudaError_t ConvertToRoundTrip(ConvertToImageView image, cudaStream_t stream) {
  if (!image.src || !image.dst || !image.scratch || image.src == image.dst ||
      reinterpret_cast<const void*>(image.src) == image.scratch ||
      reinterpret_cast<void*>(image.dst) == image.scratch || image.width <= 0 ||
      image.height <= 0 ||
      reinterpret_cast<std::uintptr_t>(image.scratch) % alignof(float) ||
      image.scratch_stride % alignof(float))
    return cudaErrorInvalidValue;
  const auto width = static_cast<std::size_t>(image.width);
  const auto height = static_cast<std::size_t>(image.height);
  const auto max = std::numeric_limits<std::size_t>::max();
  if (width > max / (3 * sizeof(float))) return cudaErrorInvalidValue;
  const auto row = width * 3;
  if (row > max / height || !ValidStride(image.src_stride, row, height) ||
      !ValidStride(image.dst_stride, row, height) ||
      !ValidStride(image.scratch_stride, row * sizeof(float), height))
    return cudaErrorInvalidValue;
  const auto samples = row * height;
  const auto blocks = static_cast<unsigned int>(
      std::min<std::size_t>((samples - 1) / 256 + 1, 65535));
  ToFloat<<<blocks, 256, 0, stream>>>(image, samples);
  auto status = cudaGetLastError();
  if (status != cudaSuccess) return status;
  ToByte<<<blocks, 256, 0, stream>>>(image, samples);
  return cudaGetLastError();
}
}  // namespace cv_cuda_pipeline
