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

#ifndef CV_CUDA_PIPELINE_CALCULATORS_IMAGE_PROCESS_CUDA_CROP_FLIP_NORMALIZE_REFORMAT_KERNEL_H_
#define CV_CUDA_PIPELINE_CALCULATORS_IMAGE_PROCESS_CUDA_CROP_FLIP_NORMALIZE_REFORMAT_KERNEL_H_

#include <cuda_runtime_api.h>

#include <cstddef>
#include <cstdint>

namespace cv_cuda_pipeline {
struct CropFlipNormalizeView {
  const std::uint8_t* src;
  std::size_t src_stride;
  int src_width;
  int src_height;
  float* dst;
  std::size_t dst_row_stride;
  std::size_t dst_plane_stride;
  int x;
  int y;
  int width;
  int height;
};
// RGB8 input to planar RGB F32. Crop must fit the input; the crop is flipped
// horizontally. Each channel is (pixel - base) / sqrt(stddev^2 + 1e-8) / 255.
// The source and destination must not overlap. Strides are in bytes; output
// pointers and strides must be float-aligned. Keep buffers alive and
// synchronize the stream to detect asynchronous errors.
cudaError_t CropFlipNormalizeReformat(CropFlipNormalizeView image,
                                      cudaStream_t stream = nullptr);
}  // namespace cv_cuda_pipeline
#endif  // CV_CUDA_PIPELINE_CALCULATORS_IMAGE_PROCESS_CUDA_CROP_FLIP_NORMALIZE_REFORMAT_KERNEL_H_
