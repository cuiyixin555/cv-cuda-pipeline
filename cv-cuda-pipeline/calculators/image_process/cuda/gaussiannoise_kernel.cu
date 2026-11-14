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
#include <cmath>
#include <limits>

#include "cv-cuda-pipeline/calculators/image_process/cuda/gaussiannoise_kernel.h"

namespace cv_cuda_pipeline {
namespace {
__device__ std::uint32_t Mix(std::uint32_t value) {
  value ^= value >> 16;
  value *= 0x7feb352dU;
  value ^= value >> 15;
  value *= 0x846ca68bU;
  value ^= value >> 16;
  return value;
}

__device__ float Uniform(std::uint32_t value) {
  const float unit =
      (static_cast<float>(value) + 1.0f) * 2.3283064365386963e-10f;
  return fminf(1.0f, fmaxf(1.0e-7f, unit));
}

__global__ void AddNoise(GaussianNoiseImageView image, std::size_t pixels) {
  for (std::size_t i =
           static_cast<std::size_t>(blockIdx.x) * blockDim.x + threadIdx.x;
       i < pixels; i += static_cast<std::size_t>(gridDim.x) * blockDim.x) {
    const auto index = static_cast<std::uint64_t>(i);
    const std::uint32_t low = static_cast<std::uint32_t>(index);
    const std::uint32_t high = static_cast<std::uint32_t>(index >> 32);
    const auto seed = static_cast<std::uint32_t>(image.seed) ^
                      Mix(static_cast<std::uint32_t>(image.seed >> 32));
    const float u1 = Uniform(Mix(low ^ Mix(high) ^ seed));
    const float u2 =
        Uniform(Mix(low ^ Mix(high + 0x9e3779b9U) ^ (seed + 0x85ebca6bU)));
    constexpr float kTwoPi = 6.2831853071795864769f;
    const float normal = sqrtf(-2.0f * logf(u1)) * cosf(kTwoPi * u2);
    const float noise = image.mean + image.sigma * normal;
    const int x = static_cast<int>(i % image.width);
    const int y = static_cast<int>(i / image.width);
    const auto* src = image.src +
                      static_cast<std::size_t>(y) * image.src_stride +
                      static_cast<std::size_t>(x) * image.channels;
    auto* dst = image.dst + static_cast<std::size_t>(y) * image.dst_stride +
                static_cast<std::size_t>(x) * image.channels;
    for (int c = 0; c < image.channels; ++c) {
      const int value = __float2int_rn(static_cast<float>(src[c]) + noise);
      dst[c] = static_cast<std::uint8_t>(max(0, min(255, value)));
    }
  }
}

bool ValidStride(std::size_t stride, std::size_t row, std::size_t height) {
  return stride >= row &&
         height - 1 <= (std::numeric_limits<std::size_t>::max() - row) / stride;
}
}  // namespace

cudaError_t AddGaussianNoise(GaussianNoiseImageView image,
                             cudaStream_t stream) {
  if (!image.src || !image.dst || image.src == image.dst || image.width <= 0 ||
      image.height <= 0 ||
      !(image.channels == 1 || image.channels == 3 || image.channels == 4) ||
      !std::isfinite(image.mean) || !std::isfinite(image.sigma) ||
      image.sigma < 0.0f)
    return cudaErrorInvalidValue;
  const auto width = static_cast<std::size_t>(image.width);
  const auto height = static_cast<std::size_t>(image.height);
  const auto channels = static_cast<std::size_t>(image.channels);
  const auto limit = std::numeric_limits<std::size_t>::max();
  if (width > limit / channels || width > limit / height ||
      !ValidStride(image.src_stride, width * channels, height) ||
      !ValidStride(image.dst_stride, width * channels, height))
    return cudaErrorInvalidValue;
  const auto pixels = width * height;
  const auto blocks = static_cast<unsigned int>(
      std::min<std::size_t>((pixels - 1) / 128 + 1, 65535));
  AddNoise<<<blocks, 128, 0, stream>>>(image, pixels);
  return cudaGetLastError();
}
}  // namespace cv_cuda_pipeline
