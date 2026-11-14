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

#include "cv-cuda-pipeline/calculators/image_process/cuda/resize_crop_convert_reformat_kernel.h"

namespace cv_cuda_pipeline {
namespace {
__global__ void ResizeCropConvertReformatKernel(
    ResizeCropConvertReformatView image, std::size_t pixels, float scale_x,
    float scale_y) {
  for (std::size_t i =
           static_cast<std::size_t>(blockIdx.x) * blockDim.x + threadIdx.x;
       i < pixels; i += static_cast<std::size_t>(gridDim.x) * blockDim.x) {
    const int x = static_cast<int>(i % image.crop_width);
    const int y = static_cast<int>(i / image.crop_width);
    const int resized_x = image.crop_x + x;
    const int resized_y = image.crop_y + y;
    const float fx = (resized_x + 0.5f) * scale_x - 0.5f;
    const float fy = (resized_y + 0.5f) * scale_y - 0.5f;
    const int ix = __float2int_rd(fx), iy = __float2int_rd(fy);
    const float wx =
        ix < 0 ? 0.0f : (ix >= image.src_width - 1 ? 1.0f : fx - ix);
    const float wy =
        iy < 0 ? 0.0f : (iy >= image.src_height - 1 ? 1.0f : fy - iy);
    const int x0 = max(0, min(ix, image.src_width - 1));
    const int y0 = max(0, min(iy, image.src_height - 1));
    const int x1 = min(x0 + 1, image.src_width - 1);
    const int y1 = min(y0 + 1, image.src_height - 1);
    const auto* row0 =
        image.src + static_cast<std::size_t>(y0) * image.src_stride;
    const auto* row1 =
        image.src + static_cast<std::size_t>(y1) * image.src_stride;
    for (int c = 0; c < 3; ++c) {
      const int source_channel = 2 - c;
      const float value =
          row0[static_cast<std::size_t>(x0) * 3 + source_channel] *
              ((1.0f - wx) * (1.0f - wy)) +
          row0[static_cast<std::size_t>(x1) * 3 + source_channel] *
              (wx * (1.0f - wy)) +
          row1[static_cast<std::size_t>(x0) * 3 + source_channel] *
              ((1.0f - wx) * wy) +
          row1[static_cast<std::size_t>(x1) * 3 + source_channel] * (wx * wy);
      auto* out_row = reinterpret_cast<float*>(
          reinterpret_cast<std::uint8_t*>(image.dst) +
          c * image.dst_plane_stride + y * image.dst_row_stride);
      out_row[x] = value;
    }
  }
}

bool ValidRows(std::size_t stride, std::size_t row, std::size_t height) {
  return stride >= row &&
         height - 1 <= (std::numeric_limits<std::size_t>::max() - row) / stride;
}
}  // namespace

cudaError_t ResizeCropConvertReformat(ResizeCropConvertReformatView image,
                                      cudaStream_t stream) {
  if (!image.src || !image.dst ||
      reinterpret_cast<const void*>(image.src) == image.dst ||
      image.src_width <= 0 || image.src_height <= 0 ||
      image.resize_width <= 0 || image.resize_height <= 0 || image.crop_x < 0 ||
      image.crop_y < 0 || image.crop_width <= 0 || image.crop_height <= 0 ||
      image.crop_x >= image.resize_width ||
      image.crop_y >= image.resize_height ||
      image.crop_width > image.resize_width - image.crop_x ||
      image.crop_height > image.resize_height - image.crop_y ||
      reinterpret_cast<std::uintptr_t>(image.dst) % alignof(float) ||
      image.dst_row_stride % alignof(float) ||
      image.dst_plane_stride % alignof(float))
    return cudaErrorInvalidValue;
  const auto sw = static_cast<std::size_t>(image.src_width);
  const auto sh = static_cast<std::size_t>(image.src_height);
  const auto cw = static_cast<std::size_t>(image.crop_width);
  const auto ch = static_cast<std::size_t>(image.crop_height);
  const auto max = std::numeric_limits<std::size_t>::max();
  if (sw > max / 3 || cw > max / sizeof(float) || cw > max / ch ||
      !ValidRows(image.src_stride, sw * 3, sh) ||
      !ValidRows(image.dst_row_stride, cw * sizeof(float), ch))
    return cudaErrorInvalidValue;
  const auto plane = (ch - 1) * image.dst_row_stride + cw * sizeof(float);
  if (image.dst_plane_stride < plane ||
      image.dst_plane_stride > (max - plane) / 2)
    return cudaErrorInvalidValue;
  const auto pixels = cw * ch;
  const auto blocks = static_cast<unsigned int>(
      std::min<std::size_t>((pixels - 1) / 256 + 1, 65535));
  ResizeCropConvertReformatKernel<<<blocks, 256, 0, stream>>>(
      image, pixels, static_cast<float>(image.src_width) / image.resize_width,
      static_cast<float>(image.src_height) / image.resize_height);
  return cudaGetLastError();
}
}  // namespace cv_cuda_pipeline
