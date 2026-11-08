// SPDX-FileCopyrightText: Copyright (c) 2023-2026 NVIDIA CORPORATION & AFFILIATES
// SPDX-License-Identifier: Apache-2.0

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

#include "cv-cuda-pipeline/calculators/image_process/cuda/adv_cvt_color_kernel.h"

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

__device__ int Descale(int value) { return (value + 8192) >> 14; }
__device__ std::uint8_t Clamp(int value) {
  return static_cast<std::uint8_t>(max(0, min(255, value)));
}

template <bool kForward>
__global__ void AdvColorKernel(AdvColorImageView image, std::size_t pixels) {
  for (std::size_t i = static_cast<std::size_t>(blockIdx.x) * blockDim.x +
                       threadIdx.x;
       i < pixels; i += static_cast<std::size_t>(gridDim.x) * blockDim.x) {
    const std::size_t x = i % image.width;
    const std::size_t y = i / image.width;
    const auto* src = image.src + y * image.src_stride + x * 3;
    auto* dst = image.dst + y * image.dst_stride + x * 3;
    if constexpr (kForward) {
      const int y_value = Descale(src[0] * 3483 + src[1] * 11718 + src[2] * 1183);
      dst[0] = Clamp(y_value);
      dst[1] = Clamp(Descale((src[2] - y_value) * 8829 + (128 << 14)));
      dst[2] = Clamp(Descale((src[0] - y_value) * 10404 + (128 << 14)));
    } else {
      const int y_value = src[0], u = src[1] - 128, v = src[2] - 128;
      dst[0] = Clamp(y_value + Descale(v * 25802));
      dst[1] = Clamp(y_value + Descale(u * -3069 + v * -7670));
      dst[2] = Clamp(y_value + Descale(u * 30402));
    }
  }
}

}  // namespace

cudaError_t AdvCvtColor(AdvColorImageView image, AdvColorDirection direction,
                        cudaStream_t stream) {
  if (!image.src || !image.dst || image.width <= 0 || image.height <= 0 ||
      (direction != AdvColorDirection::kRgbToYuv &&
       direction != AdvColorDirection::kYuvToRgb)) {
    return cudaErrorInvalidValue;
  }
  const std::size_t width = static_cast<std::size_t>(image.width);
  const std::size_t height = static_cast<std::size_t>(image.height);
  const auto max = std::numeric_limits<std::size_t>::max();
  if (width > max / 3 || width > max / height ||
      !ValidStride(image.src_stride, width * 3, image.height) ||
      !ValidStride(image.dst_stride, width * 3, image.height))
    return cudaErrorInvalidValue;
  constexpr unsigned int threads = 256;
  const std::size_t pixels = width * height;
  const auto blocks = static_cast<unsigned int>(
      std::min<std::size_t>((pixels - 1) / threads + 1, 65535));
  if (direction == AdvColorDirection::kRgbToYuv)
    AdvColorKernel<true><<<blocks, threads, 0, stream>>>(image, pixels);
  else
    AdvColorKernel<false><<<blocks, threads, 0, stream>>>(image, pixels);
  return cudaGetLastError();
}

}  // namespace cv_cuda_pipeline
