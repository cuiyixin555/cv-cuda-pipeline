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

#include "cv-cuda-pipeline/calculators/image_process/cuda/copymakeborder_kernel.h"

namespace cv_cuda_pipeline {
namespace {
bool ValidStride(std::size_t stride, std::size_t row, std::size_t height) {
  return stride >= row &&
         height - 1 <= (std::numeric_limits<std::size_t>::max() - row) / stride;
}
__global__ void Pad(CopyMakeBorderView image, std::size_t w,
                    std::size_t pixels) {
  for (std::size_t i =
           static_cast<std::size_t>(blockIdx.x) * blockDim.x + threadIdx.x;
       i < pixels; i += static_cast<std::size_t>(gridDim.x) * blockDim.x) {
    const std::size_t x = i % w, y = i / w;
    auto* out = image.dst + y * image.dst_stride + x * 3;
    if (x >= static_cast<std::size_t>(image.left) &&
        x - image.left < static_cast<std::size_t>(image.width) &&
        y >= static_cast<std::size_t>(image.top) &&
        y - image.top < static_cast<std::size_t>(image.height)) {
      const auto* src =
          image.src + (y - image.top) * image.src_stride + (x - image.left) * 3;
      for (int c = 0; c < 3; ++c) out[c] = src[c];
    } else {
      for (int c = 0; c < 3; ++c) out[c] = image.color[c];
    }
  }
}
}  // namespace
cudaError_t CopyMakeBorder(CopyMakeBorderView image, cudaStream_t stream) {
  if (!image.src || !image.dst || image.src == image.dst || image.width <= 0 ||
      image.height <= 0 || image.top < 0 || image.bottom < 0 ||
      image.left < 0 || image.right < 0)
    return cudaErrorInvalidValue;
  const auto max = std::numeric_limits<std::size_t>::max();
  const auto width = static_cast<std::size_t>(image.width);
  const auto height = static_cast<std::size_t>(image.height);
  const auto left = static_cast<std::size_t>(image.left);
  const auto right = static_cast<std::size_t>(image.right);
  const auto top = static_cast<std::size_t>(image.top);
  const auto bottom = static_cast<std::size_t>(image.bottom);
  if (left > max - width || right > max - width - left || top > max - height ||
      bottom > max - height - top)
    return cudaErrorInvalidValue;
  const auto outw = width + left + right, outh = height + top + bottom;
  if (width > max / 3 || outw > max / 3 || outw > max / outh ||
      !ValidStride(image.src_stride, width * 3, height) ||
      !ValidStride(image.dst_stride, outw * 3, outh))
    return cudaErrorInvalidValue;
  const auto pixels = outw * outh;
  const auto blocks = static_cast<unsigned int>(
      std::min<std::size_t>((pixels - 1) / 256 + 1, 65535));
  Pad<<<blocks, 256, 0, stream>>>(image, outw, pixels);
  return cudaGetLastError();
}
}  // namespace cv_cuda_pipeline
