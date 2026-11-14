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

#include "cv-cuda-pipeline/calculators/image_process/cuda/histogrameq_kernel.h"

namespace cv_cuda_pipeline {
namespace {
__global__ void BuildHistogram(HistogramEqualizeImageView image,
                               std::size_t pixels) {
  for (std::size_t i =
           static_cast<std::size_t>(blockIdx.x) * blockDim.x + threadIdx.x;
       i < pixels; i += static_cast<std::size_t>(gridDim.x) * blockDim.x) {
    const auto x = i % image.width;
    const auto y = i / image.width;
    const auto value = image.src[y * image.src_stride + x];
    atomicAdd(&image.histogram[value], 1U);
  }
}

__global__ void BuildLookupTable(HistogramEqualizeImageView image,
                                 std::uint32_t pixels) {
  if (blockIdx.x || threadIdx.x) return;
  std::uint32_t cdf_min = 0;
  for (int i = 0; i < 256; ++i) {
    if (image.histogram[i] != 0) {
      cdf_min = image.histogram[i];
      break;
    }
  }
  const float denominator = static_cast<float>(pixels - cdf_min);
  const float scale = denominator > 0.0f ? 255.0f / denominator : 0.0f;
  std::uint32_t cdf = 0;
  for (int i = 0; i < 256; ++i) {
    cdf += image.histogram[i];
    const float value =
        cdf > cdf_min ? static_cast<float>(cdf - cdf_min) * scale : 0.0f;
    image.lookup_table[i] =
        static_cast<std::uint8_t>(max(0, min(255, __float2int_rn(value))));
  }
}

__global__ void ApplyLookupTable(HistogramEqualizeImageView image,
                                 std::size_t pixels) {
  for (std::size_t i =
           static_cast<std::size_t>(blockIdx.x) * blockDim.x + threadIdx.x;
       i < pixels; i += static_cast<std::size_t>(gridDim.x) * blockDim.x) {
    const auto x = i % image.width;
    const auto y = i / image.width;
    const auto value = image.src[y * image.src_stride + x];
    image.dst[y * image.dst_stride + x] = image.lookup_table[value];
  }
}

bool ValidStride(std::size_t stride, std::size_t row, std::size_t height) {
  return stride >= row &&
         height - 1 <= (std::numeric_limits<std::size_t>::max() - row) / stride;
}
}  // namespace

cudaError_t HistogramEqualize(HistogramEqualizeImageView image,
                              cudaStream_t stream) {
  if (!image.src || !image.dst || image.src == image.dst || !image.histogram ||
      !image.lookup_table || image.width <= 0 || image.height <= 0)
    return cudaErrorInvalidValue;
  const auto width = static_cast<std::size_t>(image.width);
  const auto height = static_cast<std::size_t>(image.height);
  const auto limit = std::numeric_limits<std::size_t>::max();
  if (width > limit / height || width * height > UINT32_MAX ||
      !ValidStride(image.src_stride, width, height) ||
      !ValidStride(image.dst_stride, width, height))
    return cudaErrorInvalidValue;
  const auto pixels = width * height;
  auto status =
      cudaMemsetAsync(image.histogram, 0, 256 * sizeof(std::uint32_t), stream);
  if (status != cudaSuccess) return status;
  const auto blocks = static_cast<unsigned int>(
      std::min<std::size_t>((pixels - 1) / 256 + 1, 65535));
  BuildHistogram<<<blocks, 256, 0, stream>>>(image, pixels);
  status = cudaGetLastError();
  if (status != cudaSuccess) return status;
  BuildLookupTable<<<1, 1, 0, stream>>>(image,
                                        static_cast<std::uint32_t>(pixels));
  status = cudaGetLastError();
  if (status != cudaSuccess) return status;
  ApplyLookupTable<<<blocks, 256, 0, stream>>>(image, pixels);
  return cudaGetLastError();
}
}  // namespace cv_cuda_pipeline
