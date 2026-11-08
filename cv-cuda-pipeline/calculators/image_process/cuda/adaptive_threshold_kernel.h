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

#ifndef CV_CUDA_PIPELINE_CALCULATORS_IMAGE_PROCESS_CUDA_ADAPTIVE_THRESHOLD_KERNEL_H_
#define CV_CUDA_PIPELINE_CALCULATORS_IMAGE_PROCESS_CUDA_ADAPTIVE_THRESHOLD_KERNEL_H_

#include <cuda_runtime_api.h>

#include <cstddef>
#include <cstdint>

namespace cv_cuda_pipeline {

struct AdaptiveThresholdView {
  const std::uint8_t* src;
  std::size_t src_stride;
  std::uint8_t* gray_scratch;
  std::size_t gray_stride;
  std::uint8_t* dst;
  std::size_t dst_stride;
  int width;
  int height;
};

// RGB8 input/output, 11x11 Gaussian, replicate border, binary max 255, C=2.
// Scratch is a grayscale device image. Buffers must not overlap and must stay
// valid until the caller synchronizes the stream. Returns argument/launch
// errors; asynchronous errors are reported by stream synchronization.
cudaError_t AdaptiveThreshold(AdaptiveThresholdView image,
                              cudaStream_t stream = nullptr);

}  // namespace cv_cuda_pipeline

#endif  // CV_CUDA_PIPELINE_CALCULATORS_IMAGE_PROCESS_CUDA_ADAPTIVE_THRESHOLD_KERNEL_H_
