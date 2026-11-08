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
#include <limits>

#include "cv-cuda-pipeline/calculators/image_process/cuda/resize_kernel.h"

namespace cv_cuda_pipeline {
namespace {

template <typename Image>
bool ValidView(const Image& image, int batch_size) {
  if (image.data == nullptr || image.width <= 0 || image.height <= 0 ||
      batch_size <= 0) {
    return false;
  }
  const std::size_t max = std::numeric_limits<std::size_t>::max();
  const std::size_t width = static_cast<std::size_t>(image.width);
  const std::size_t height = static_cast<std::size_t>(image.height);
  if (width > max / 3 || image.row_stride < width * 3 ||
      (height - 1) > (max - width * 3) / image.row_stride) {
    return false;
  }
  const std::size_t image_bytes = (height - 1) * image.row_stride + width * 3;
  return image.batch_stride >= image_bytes &&
         static_cast<std::size_t>(batch_size - 1) <=
             (max - image_bytes) / image.batch_stride;
}

__global__ void ResizeLinearKernel(ResizeConstImageView src,
                                   ResizeImageView dst, std::size_t pixels,
                                   float scale_x, float scale_y) {
  const std::size_t plane = static_cast<std::size_t>(dst.width) * dst.height;
  for (std::size_t index =
           static_cast<std::size_t>(blockIdx.x) * blockDim.x + threadIdx.x;
       index < pixels;
       index += static_cast<std::size_t>(blockDim.x) * gridDim.x) {
    const std::size_t batch = index / plane;
    const std::size_t pixel = index % plane;
    const int x = static_cast<int>(pixel % dst.width);
    const int y = static_cast<int>(pixel / dst.width);

    const float fx = (static_cast<float>(x) + 0.5f) * scale_x - 0.5f;
    const float fy = (static_cast<float>(y) + 0.5f) * scale_y - 0.5f;
    const int ix = __float2int_rd(fx);
    const int iy = __float2int_rd(fy);
    const float wx = ix < 0 ? 0.f : (ix >= src.width - 1 ? 1.f : fx - ix);
    const float wy = iy < 0 ? 0.f : (iy >= src.height - 1 ? 1.f : fy - iy);
    const int x0 = max(0, min(ix, src.width - 1));
    const int y0 = max(0, min(iy, src.height - 1));
    const int x1 = min(x0 + 1, src.width - 1);
    const int y1 = min(y0 + 1, src.height - 1);
    const std::uint8_t* row0 =
        src.data + batch * src.batch_stride + y0 * src.row_stride;
    const std::uint8_t* row1 =
        src.data + batch * src.batch_stride + y1 * src.row_stride;
    std::uint8_t* out = dst.data + batch * dst.batch_stride +
                        y * dst.row_stride + static_cast<std::size_t>(x) * 3;
    for (int c = 0; c < 3; ++c) {
      const float value =
          row0[static_cast<std::size_t>(x0) * 3 + c] *
              ((1.f - wx) * (1.f - wy)) +
          row0[static_cast<std::size_t>(x1) * 3 + c] * (wx * (1.f - wy)) +
          row1[static_cast<std::size_t>(x0) * 3 + c] * ((1.f - wx) * wy) +
          row1[static_cast<std::size_t>(x1) * 3 + c] * (wx * wy);
      out[c] =
          static_cast<std::uint8_t>(max(0, min(255, __float2int_rn(value))));
    }
  }
}

}  // namespace

cudaError_t ResizeLinear(ResizeConstImageView src, ResizeImageView dst,
                         int batch_size, cudaStream_t stream) {
  if (!ValidView(src, batch_size) || !ValidView(dst, batch_size)) {
    return cudaErrorInvalidValue;
  }
  const std::size_t max = std::numeric_limits<std::size_t>::max();
  const std::size_t width = static_cast<std::size_t>(dst.width);
  const std::size_t height = static_cast<std::size_t>(dst.height);
  if (width > max / height || width * height > max / batch_size) {
    return cudaErrorInvalidValue;
  }
  const std::size_t pixels = width * height * batch_size;
  constexpr unsigned int kThreads = 256;
  const unsigned int blocks = static_cast<unsigned int>(
      std::min<std::size_t>((pixels - 1) / kThreads + 1, 65535));
  ResizeLinearKernel<<<blocks, kThreads, 0, stream>>>(
      src, dst, pixels, static_cast<float>(src.width) / dst.width,
      static_cast<float>(src.height) / dst.height);
  return cudaGetLastError();
}

}  // namespace cv_cuda_pipeline