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

#ifndef CV_CUDA_PIPELINE_CALCULATORS_IMAGE_PROCESS_CUDA_RESIZE_KERNEL_H_
#define CV_CUDA_PIPELINE_CALCULATORS_IMAGE_PROCESS_CUDA_RESIZE_KERNEL_H_

#include <cuda_runtime_api.h>

#include <cstddef>
#include <cstdint>

namespace cv_cuda_pipeline {

// Interleaved three-channel uint8 images in device memory. Strides are in
// bytes; each batch member has the same width, height, and strides.
struct ResizeImageView {
  std::uint8_t* data;
  int width;
  int height;
  std::size_t row_stride;
  std::size_t batch_stride;
};

struct ResizeConstImageView {
  const std::uint8_t* data;
  int width;
  int height;
  std::size_t row_stride;
  std::size_t batch_stride;
};

// Enqueues bilinear resize on stream. Source and destination must not overlap.
// The caller owns the buffers and must keep them alive until stream completes.
// Returns launch/argument errors; asynchronous errors are reported when the
// caller synchronizes the stream.
cudaError_t ResizeLinear(ResizeConstImageView src, ResizeImageView dst,
                         int batch_size, cudaStream_t stream = nullptr);

}  // namespace cv_cuda_pipeline

#endif  // CV_CUDA_PIPELINE_CALCULATORS_IMAGE_PROCESS_CUDA_RESIZE_KERNEL_H_