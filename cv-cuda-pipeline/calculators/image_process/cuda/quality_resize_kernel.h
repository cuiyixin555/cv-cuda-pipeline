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

#ifndef CV_CUDA_PIPELINE_CALCULATORS_IMAGE_PROCESS_CUDA_QUALITY_RESIZE_KERNEL_H_
#define CV_CUDA_PIPELINE_CALCULATORS_IMAGE_PROCESS_CUDA_QUALITY_RESIZE_KERNEL_H_

#include <cuda_runtime_api.h>

#include <cstddef>
#include <cstdint>

namespace cv_cuda_pipeline {

enum class QualityResizeMode {
  // Lanczos-3 in both dimensions, including upscaling.
  kLanczos,
  // Lanczos-3 for downscaling and linear interpolation for upscaling.
  kLanczosDownLinearUp,
};

struct QualityResizeImageView {
  const std::uint8_t* src;
  std::size_t src_stride;
  float* intermediate;
  std::size_t intermediate_stride;
  std::uint8_t* dst;
  std::size_t dst_stride;
  int width;
  int height;
  int output_width;
  int output_height;
};

// Resizes an interleaved three-channel U8 image using separable filters.
// The temporary buffer has height rows and at least
// output_width * 3 * sizeof(float) bytes per row. Buffers must not overlap and
// must remain valid until stream completion.
cudaError_t QualityResize(QualityResizeImageView image, QualityResizeMode mode,
                          cudaStream_t stream = nullptr);

}  // namespace cv_cuda_pipeline

#endif  // CV_CUDA_PIPELINE_CALCULATORS_IMAGE_PROCESS_CUDA_QUALITY_RESIZE_KERNEL_H_
