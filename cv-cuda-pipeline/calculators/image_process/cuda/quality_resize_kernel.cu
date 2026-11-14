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

#include "cv-cuda-pipeline/calculators/image_process/cuda/quality_resize_kernel.h"

namespace cv_cuda_pipeline {
namespace {

constexpr float kPi = 3.14159265358979323846f;
constexpr int kChannels = 3;
constexpr unsigned int kThreads = 256;

bool ValidStride(std::size_t stride, std::size_t row_bytes, int height) {
  return stride >= row_bytes &&
         static_cast<std::size_t>(height - 1) <=
             (std::numeric_limits<std::size_t>::max() - row_bytes) / stride;
}

__device__ float Lanczos(float value) {
  const float x = fabsf(value);
  if (x < 1.0e-6f) return 1.0f;
  if (x >= 3.0f) return 0.0f;
  const float pix = kPi * x;
  return (sinf(pix) / pix) * (sinf(pix / 3.0f) / (pix / 3.0f));
}

__device__ bool UseLanczos(QualityResizeMode mode, float scale) {
  return mode == QualityResizeMode::kLanczos || scale < 1.0f;
}

__device__ float FilterWeight(int sample, float center, float scale,
                              bool lanczos) {
  const float distance = (sample - center) * fminf(scale, 1.0f);
  if (lanczos) return Lanczos(distance);
  return fmaxf(0.0f, 1.0f - fabsf(distance));
}

__global__ void HorizontalResize(const std::uint8_t* src,
                                 std::size_t src_stride, float* intermediate,
                                 std::size_t intermediate_stride, int width,
                                 int height, int output_width,
                                 QualityResizeMode mode,
                                 std::size_t output_pixels) {
  const float scale = static_cast<float>(output_width) / width;
  const bool lanczos = UseLanczos(mode, scale);
  const float support = (lanczos ? 3.0f : 1.0f) / fminf(scale, 1.0f);
  for (std::size_t index =
           static_cast<std::size_t>(blockIdx.x) * blockDim.x + threadIdx.x;
       index < output_pixels;
       index += static_cast<std::size_t>(gridDim.x) * blockDim.x) {
    const int y = static_cast<int>(index / output_width);
    const int x = static_cast<int>(index % output_width);
    const float center = (x + 0.5f) / scale - 0.5f;
    const int first = static_cast<int>(ceilf(center - support));
    const int last = static_cast<int>(floorf(center + support));
    float sums[kChannels] = {0.0f, 0.0f, 0.0f};
    float weight_sum = 0.0f;
    for (int sx = first; sx <= last; ++sx) {
      const float weight = FilterWeight(sx, center, scale, lanczos);
      if (weight == 0.0f) continue;
      const int clamped_x = min(max(sx, 0), width - 1);
      const auto* pixel =
          src + static_cast<std::size_t>(y) * src_stride + clamped_x * 3;
      for (int c = 0; c < kChannels; ++c) sums[c] += weight * pixel[c];
      weight_sum += weight;
    }
    auto* row = reinterpret_cast<float*>(
        reinterpret_cast<std::uint8_t*>(intermediate) +
        static_cast<std::size_t>(y) * intermediate_stride);
    for (int c = 0; c < kChannels; ++c)
      row[x * kChannels + c] = sums[c] / weight_sum;
  }
}

__global__ void VerticalResize(const float* intermediate,
                               std::size_t intermediate_stride,
                               std::uint8_t* dst, std::size_t dst_stride,
                               int output_width, int height, int output_height,
                               QualityResizeMode mode,
                               std::size_t output_pixels) {
  const float scale = static_cast<float>(output_height) / height;
  const bool lanczos = UseLanczos(mode, scale);
  const float support = (lanczos ? 3.0f : 1.0f) / fminf(scale, 1.0f);
  for (std::size_t index =
           static_cast<std::size_t>(blockIdx.x) * blockDim.x + threadIdx.x;
       index < output_pixels;
       index += static_cast<std::size_t>(gridDim.x) * blockDim.x) {
    const int y = static_cast<int>(index / output_width);
    const int x = static_cast<int>(index % output_width);
    const float center = (y + 0.5f) / scale - 0.5f;
    const int first = static_cast<int>(ceilf(center - support));
    const int last = static_cast<int>(floorf(center + support));
    float sums[kChannels] = {0.0f, 0.0f, 0.0f};
    float weight_sum = 0.0f;
    for (int sy = first; sy <= last; ++sy) {
      const float weight = FilterWeight(sy, center, scale, lanczos);
      if (weight == 0.0f) continue;
      const int clamped_y = min(max(sy, 0), height - 1);
      const auto* row = reinterpret_cast<const float*>(
          reinterpret_cast<const std::uint8_t*>(intermediate) +
          static_cast<std::size_t>(clamped_y) * intermediate_stride);
      for (int c = 0; c < kChannels; ++c)
        sums[c] += weight * row[x * kChannels + c];
      weight_sum += weight;
    }
    auto* output = dst + static_cast<std::size_t>(y) * dst_stride + x * 3;
    for (int c = 0; c < kChannels; ++c) {
      const int value = __float2int_rn(sums[c] / weight_sum);
      output[c] = static_cast<std::uint8_t>(min(max(value, 0), 255));
    }
  }
}

}  // namespace

cudaError_t QualityResize(QualityResizeImageView image, QualityResizeMode mode,
                          cudaStream_t stream) {
  if (!image.src || !image.intermediate || !image.dst || image.width <= 0 ||
      image.height <= 0 || image.output_width <= 0 ||
      image.output_height <= 0 ||
      (mode != QualityResizeMode::kLanczos &&
       mode != QualityResizeMode::kLanczosDownLinearUp))
    return cudaErrorInvalidValue;

  const auto max = std::numeric_limits<std::size_t>::max();
  const std::size_t width = static_cast<std::size_t>(image.width);
  const std::size_t height = static_cast<std::size_t>(image.height);
  const std::size_t output_width = static_cast<std::size_t>(image.output_width);
  const std::size_t output_height =
      static_cast<std::size_t>(image.output_height);
  if (width > max / 3 || output_width > max / (3 * sizeof(float)) ||
      width > max / height || output_width > max / height ||
      output_width > max / output_height)
    return cudaErrorInvalidValue;
  const std::size_t src_row = width * 3;
  const std::size_t dst_row = output_width * 3;
  const std::size_t intermediate_row = dst_row * sizeof(float);
  if (!ValidStride(image.src_stride, src_row, image.height) ||
      !ValidStride(image.dst_stride, dst_row, image.output_height) ||
      !ValidStride(image.intermediate_stride, intermediate_row, image.height) ||
      image.intermediate_stride % alignof(float) != 0)
    return cudaErrorInvalidValue;

  const std::size_t pixels = output_width * output_height;
  const std::size_t horizontal_pixels = output_width * height;
  const std::size_t max_pixels = std::max(pixels, horizontal_pixels);
  const unsigned int blocks = static_cast<unsigned int>(
      std::min<std::size_t>((max_pixels - 1) / kThreads + 1, 65535));
  HorizontalResize<<<blocks, kThreads, 0, stream>>>(
      image.src, image.src_stride, image.intermediate,
      image.intermediate_stride, image.width, image.height, image.output_width,
      mode, output_width * static_cast<std::size_t>(image.height));
  cudaError_t status = cudaGetLastError();
  if (status != cudaSuccess) return status;
  VerticalResize<<<blocks, kThreads, 0, stream>>>(
      image.intermediate, image.intermediate_stride, image.dst,
      image.dst_stride, image.output_width, image.height, image.output_height,
      mode, pixels);
  return cudaGetLastError();
}

}  // namespace cv_cuda_pipeline
