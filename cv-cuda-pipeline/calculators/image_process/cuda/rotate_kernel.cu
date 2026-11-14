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

#include "cv-cuda-pipeline/calculators/image_process/cuda/rotate_kernel.h"

namespace cv_cuda_pipeline {
namespace {
struct Affine {
  float c, s, shift_x, shift_y;
};
bool ValidStride(std::size_t stride, std::size_t row, std::size_t height) {
  return stride >= row &&
         height - 1 <= (std::numeric_limits<std::size_t>::max() - row) / stride;
}

__device__ std::uint8_t Read(const RotateImageView& image, int x, int y,
                             int c) {
  if (x < 0 || x >= image.width || y < 0 || y >= image.height) return 0;
  return image.src[static_cast<std::size_t>(y) * image.src_stride +
                   static_cast<std::size_t>(x) * 3 + c];
}

__global__ void Rotate(RotateImageView image, Affine affine,
                       std::size_t pixels) {
  for (std::size_t i =
           static_cast<std::size_t>(blockIdx.x) * blockDim.x + threadIdx.x;
       i < pixels; i += static_cast<std::size_t>(gridDim.x) * blockDim.x) {
    const int x = static_cast<int>(i % image.width);
    const int y = static_cast<int>(i / image.width);
    const float dx = static_cast<float>(x) - affine.shift_x;
    const float dy = static_cast<float>(y) - affine.shift_y;
    const float sx = dx * affine.c - dy * affine.s;
    const float sy = dx * affine.s + dy * affine.c;
    auto* out = image.dst + static_cast<std::size_t>(y) * image.dst_stride +
                static_cast<std::size_t>(x) * 3;
    if (!(sx > -0.5f && sx < image.width && sy > -0.5f && sy < image.height)) {
      out[0] = out[1] = out[2] = 0;
      continue;
    }
    const int ix = static_cast<int>(floorf(sx));
    const int iy = static_cast<int>(floorf(sy));
    const float fx = sx - ix, fy = sy - iy;
    for (int c = 0; c < 3; ++c) {
      const float top =
          Read(image, ix, iy, c) * (1.f - fx) + Read(image, ix + 1, iy, c) * fx;
      const float bottom = Read(image, ix, iy + 1, c) * (1.f - fx) +
                           Read(image, ix + 1, iy + 1, c) * fx;
      const float value = top * (1.f - fy) + bottom * fy;
      out[c] = static_cast<std::uint8_t>(
          __float2int_rn(fminf(255.f, fmaxf(0.f, value))));
    }
  }
}
}  // namespace

cudaError_t Rotate45(RotateImageView image, cudaStream_t stream) {
  if (!image.src || !image.dst || image.src == image.dst || image.width <= 0 ||
      image.height <= 0)
    return cudaErrorInvalidValue;
  const auto width = static_cast<std::size_t>(image.width);
  const auto height = static_cast<std::size_t>(image.height);
  const auto limit = std::numeric_limits<std::size_t>::max();
  if (width > limit / 3 || width > limit / height ||
      !ValidStride(image.src_stride, width * 3, height) ||
      !ValidStride(image.dst_stride, width * 3, height))
    return cudaErrorInvalidValue;
  constexpr double kPi = 3.14159265358979323846;
  const double radians = 45.0 * kPi / 180.0;
  const double cosine = std::cos(radians), sine = std::sin(radians);
  const double cx = image.width / 2.0, cy = image.height / 2.0;
  const Affine affine{static_cast<float>(cosine), static_cast<float>(sine),
                      static_cast<float>(cx * (1.0 - cosine) - cy * sine),
                      static_cast<float>(cy * (1.0 - cosine) + cx * sine)};
  const auto pixels = width * height;
  const auto blocks = static_cast<unsigned int>(
      std::min<std::size_t>((pixels - 1) / 256 + 1, 65535));
  Rotate<<<blocks, 256, 0, stream>>>(image, affine, pixels);
  return cudaGetLastError();
}
}  // namespace cv_cuda_pipeline
