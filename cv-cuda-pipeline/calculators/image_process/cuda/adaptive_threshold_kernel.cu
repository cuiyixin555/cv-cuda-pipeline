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

#include <cuda_runtime.h>

#include <algorithm>
#include <cmath>
#include <limits>

#include "cv-cuda-pipeline/calculators/image_process/cuda/adaptive_threshold_kernel.h"

namespace cv_cuda_pipeline {
namespace {

struct Coefficients {
  float values[121];
};

bool ValidStride(std::size_t stride, std::size_t row_bytes, int height) {
  return stride >= row_bytes &&
         static_cast<std::size_t>(height - 1) <=
             (std::numeric_limits<std::size_t>::max() - row_bytes) / stride;
}

__global__ void ToGray(AdaptiveThresholdView image, std::size_t pixels) {
  for (std::size_t i =
           static_cast<std::size_t>(blockIdx.x) * blockDim.x + threadIdx.x;
       i < pixels; i += static_cast<std::size_t>(gridDim.x) * blockDim.x) {
    const int x = i % image.width;
    const int y = i / image.width;
    const auto* rgb =
        image.src + y * image.src_stride + static_cast<std::size_t>(x) * 3;
    image.gray_scratch[y * image.gray_stride + x] =
        (rgb[0] * 9798 + rgb[1] * 19235 + rgb[2] * 3735 + 16384) >> 15;
  }
}

__global__ void Threshold(AdaptiveThresholdView image, std::size_t pixels,
                          Coefficients weights) {
  for (std::size_t i =
           static_cast<std::size_t>(blockIdx.x) * blockDim.x + threadIdx.x;
       i < pixels; i += static_cast<std::size_t>(gridDim.x) * blockDim.x) {
    const int x = i % image.width;
    const int y = i / image.width;
    float sum = 0.f;
    for (int dy = -5; dy <= 5; ++dy) {
      const int yy = max(0, min(image.height - 1, y + dy));
      for (int dx = -5; dx <= 5; ++dx) {
        const int xx = max(0, min(image.width - 1, x + dx));
        sum += image.gray_scratch[yy * image.gray_stride + xx] *
               weights.values[(dy + 5) * 11 + dx + 5];
      }
    }
    const int mean = max(0, min(255, __float2int_rn(sum)));
    const std::uint8_t result =
        image.gray_scratch[y * image.gray_stride + x] + 2 > mean ? 255 : 0;
    auto* out =
        image.dst + y * image.dst_stride + static_cast<std::size_t>(x) * 3;
    out[0] = out[1] = out[2] = result;
  }
}

}  // namespace

cudaError_t AdaptiveThreshold(AdaptiveThresholdView image,
                              cudaStream_t stream) {
  if (!image.src || !image.dst || !image.gray_scratch || image.width <= 0 ||
      image.height <= 0 || image.width > 0x7ffffffa ||
      image.height > 0x7ffffffa) {
    return cudaErrorInvalidValue;
  }
  const std::size_t width = static_cast<std::size_t>(image.width);
  const std::size_t height = static_cast<std::size_t>(image.height);
  const std::size_t max = std::numeric_limits<std::size_t>::max();
  if (width > max / 3 || width > max / height ||
      !ValidStride(image.src_stride, width * 3, image.height) ||
      !ValidStride(image.dst_stride, width * 3, image.height) ||
      !ValidStride(image.gray_stride, width, image.height)) {
    return cudaErrorInvalidValue;
  }
  // The sample's 11x11 Gaussian uses sigma = 0.3*(5-1)+0.8 = 2.
  constexpr float sigma2 = 8.f;
  constexpr float scale = 8.f * 3.14159265358979323846f;
  float normalizer = 0.f;
  for (int y = -5; y <= 5; ++y)
    for (int x = -5; x <= 5; ++x)
      normalizer += std::exp(-(x * x + y * y) / sigma2) / scale;
  Coefficients weights{};
  for (int y = -5; y <= 5; ++y)
    for (int x = -5; x <= 5; ++x)
      weights.values[(y + 5) * 11 + x + 5] =
          std::exp(-(x * x + y * y) / sigma2) / (scale * normalizer);
  const std::size_t pixels = width * height;
  constexpr unsigned int threads = 256;
  const auto blocks = static_cast<unsigned int>(
      std::min<std::size_t>((pixels - 1) / threads + 1, 65535));
  ToGray<<<blocks, threads, 0, stream>>>(image, pixels);
  cudaError_t status = cudaGetLastError();
  if (status != cudaSuccess) return status;
  Threshold<<<blocks, threads, 0, stream>>>(image, pixels, weights);
  return cudaGetLastError();
}

}  // namespace cv_cuda_pipeline
