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

#include "cv-cuda-pipeline/calculators/image_process/cuda/joint_bilateral_filter_kernel.h"

namespace cv_cuda_pipeline {
namespace {

constexpr int kRadius = 4;
constexpr int kChannels = 3;
constexpr unsigned int kThreads = 256;
constexpr float kInvTwoSigmaColorSquared = 1.0f / (2.0f * 75.0f * 75.0f);
constexpr float kInvTwoSigmaSpaceSquared = 1.0f / (2.0f * 75.0f * 75.0f);

bool ValidStride(std::size_t stride, std::size_t row_bytes, int height) {
  return stride >= row_bytes &&
         static_cast<std::size_t>(height - 1) <=
             (std::numeric_limits<std::size_t>::max() - row_bytes) / stride;
}

__device__ int Reflect(int coordinate, int length) {
  const int period = 2 * length;
  coordinate %= period;
  if (coordinate < 0) coordinate += period;
  return coordinate < length ? coordinate : period - 1 - coordinate;
}

__global__ void JointBilateralFilterKernel(JointBilateralFilterImageView image,
                                           std::size_t pixels) {
  for (std::size_t index =
           static_cast<std::size_t>(blockIdx.x) * blockDim.x + threadIdx.x;
       index < pixels;
       index += static_cast<std::size_t>(gridDim.x) * blockDim.x) {
    const int x = static_cast<int>(index % image.width);
    const int y = static_cast<int>(index / image.width);
    const auto* guide_center =
        image.guide + static_cast<std::size_t>(y) * image.guide_stride + x * 3;
    float sums[kChannels] = {0.0f, 0.0f, 0.0f};
    float weight_sum = 0.0f;
    for (int dy = -kRadius; dy <= kRadius; ++dy) {
      const int sy = Reflect(y + dy, image.height);
      for (int dx = -kRadius; dx <= kRadius; ++dx) {
        const int sx = Reflect(x + dx, image.width);
        const auto* guide = image.guide +
                            static_cast<std::size_t>(sy) * image.guide_stride +
                            sx * 3;
        const auto* source = image.src +
                             static_cast<std::size_t>(sy) * image.src_stride +
                             sx * 3;
        float color_distance = 0.0f;
        for (int c = 0; c < kChannels; ++c) {
          const float difference =
              static_cast<float>(guide[c]) - guide_center[c];
          color_distance += difference * difference;
        }
        const float space_distance = static_cast<float>(dx * dx + dy * dy);
        const float weight = expf(-color_distance * kInvTwoSigmaColorSquared -
                                  space_distance * kInvTwoSigmaSpaceSquared);
        weight_sum += weight;
        for (int c = 0; c < kChannels; ++c) sums[c] += weight * source[c];
      }
    }
    auto* output =
        image.dst + static_cast<std::size_t>(y) * image.dst_stride + x * 3;
    for (int c = 0; c < kChannels; ++c)
      output[c] =
          static_cast<std::uint8_t>(__float2int_rn(sums[c] / weight_sum));
  }
}

}  // namespace

cudaError_t JointBilateralFilter(JointBilateralFilterImageView image,
                                 cudaStream_t stream) {
  if (!image.src || !image.guide || !image.dst || image.width <= 0 ||
      image.height <= 0)
    return cudaErrorInvalidValue;
  const std::size_t width = static_cast<std::size_t>(image.width);
  const std::size_t height = static_cast<std::size_t>(image.height);
  const auto max = std::numeric_limits<std::size_t>::max();
  if (width > max / 3 || width > max / height) return cudaErrorInvalidValue;
  const std::size_t row_bytes = width * 3;
  if (!ValidStride(image.src_stride, row_bytes, image.height) ||
      !ValidStride(image.guide_stride, row_bytes, image.height) ||
      !ValidStride(image.dst_stride, row_bytes, image.height))
    return cudaErrorInvalidValue;
  const std::size_t pixels = width * height;
  const unsigned int blocks = static_cast<unsigned int>(
      std::min<std::size_t>((pixels - 1) / kThreads + 1, 65535));
  JointBilateralFilterKernel<<<blocks, kThreads, 0, stream>>>(image, pixels);
  return cudaGetLastError();
}

}  // namespace cv_cuda_pipeline
