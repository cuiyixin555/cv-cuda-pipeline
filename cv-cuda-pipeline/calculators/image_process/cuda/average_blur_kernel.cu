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

#include "cv-cuda-pipeline/calculators/image_process/cuda/average_blur_kernel.h"

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

__device__ int Reflect101(long long coordinate, int length) {
  if (length == 1) return 0;
  if (coordinate >= 0 && coordinate < length)
    return static_cast<int>(coordinate);
  const long long period = 2LL * (length - 1);
  coordinate %= period;
  if (coordinate < 0) coordinate += period;
  return static_cast<int>(coordinate < length ? coordinate : period - coordinate);
}

__global__ void AverageBlurKernel(AverageBlurImageView image,
                                  std::size_t pixels) {
  for (std::size_t i = static_cast<std::size_t>(blockIdx.x) * blockDim.x +
                       threadIdx.x;
       i < pixels; i += static_cast<std::size_t>(gridDim.x) * blockDim.x) {
    const int x = static_cast<int>(i % image.width);
    const int y = static_cast<int>(i / image.width);
    int sum[3] = {0, 0, 0};
    for (int dy = -3; dy <= 3; ++dy) {
      const int sy = Reflect101(static_cast<long long>(y) + dy, image.height);
      for (int dx = -3; dx <= 3; ++dx) {
        const int sx = Reflect101(static_cast<long long>(x) + dx, image.width);
        const auto* p = image.src + static_cast<std::size_t>(sy) * image.src_stride +
                        static_cast<std::size_t>(sx) * 3;
        sum[0] += p[0];
        sum[1] += p[1];
        sum[2] += p[2];
      }
    }
    auto* out = image.dst + static_cast<std::size_t>(y) * image.dst_stride +
                static_cast<std::size_t>(x) * 3;
    for (int c = 0; c < 3; ++c) out[c] = (sum[c] + 24) / 49;
  }
}

}  // namespace

cudaError_t AverageBlur(AverageBlurImageView image, cudaStream_t stream) {
  if (!image.src || !image.dst || image.width <= 0 || image.height <= 0)
    return cudaErrorInvalidValue;
  const std::size_t width = static_cast<std::size_t>(image.width);
  const std::size_t height = static_cast<std::size_t>(image.height);
  const std::size_t max = std::numeric_limits<std::size_t>::max();
  if (width > max / 3 || width > max / height ||
      !ValidStride(image.src_stride, width * 3, image.height) ||
      !ValidStride(image.dst_stride, width * 3, image.height))
    return cudaErrorInvalidValue;
  constexpr unsigned int kThreads = 256;
  const std::size_t pixels = width * height;
  const auto blocks = static_cast<unsigned int>(
      std::min<std::size_t>((pixels - 1) / kThreads + 1, 65535));
  AverageBlurKernel<<<blocks, kThreads, 0, stream>>>(image, pixels);
  return cudaGetLastError();
}

}  // namespace cv_cuda_pipeline
