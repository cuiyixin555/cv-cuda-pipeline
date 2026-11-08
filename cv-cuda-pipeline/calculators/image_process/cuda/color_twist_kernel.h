// SPDX-FileCopyrightText: Copyright (c) 2026 CUI Xin
// SPDX-License-Identifier: MIT

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

#ifndef CV_CUDA_PIPELINE_CALCULATORS_IMAGE_PROCESS_CUDA_COLOR_TWIST_KERNEL_H_
#define CV_CUDA_PIPELINE_CALCULATORS_IMAGE_PROCESS_CUDA_COLOR_TWIST_KERNEL_H_

#include <cuda_runtime_api.h>

#include <cstddef>
#include <cstdint>

namespace cv_cuda_pipeline {

struct ColorTwistImageView {
  const std::uint8_t* src;
  std::size_t src_stride;
  std::uint8_t* dst;
  std::size_t dst_stride;
  int width;
  int height;
};

struct ColorTwistMatrix {
  float rows[3][4];
};

// Interleaved U8 images; each output channel is an affine combination of the
// three input channels, rounded to nearest even and clamped to [0, 255].
// Each finite matrix row must satisfy
// 255*(abs(m0)+abs(m1)+abs(m2))+abs(m3) <= FLT_MAX/2. Strides are in bytes;
// input and output buffers must not overlap and must remain valid until stream
// completion. Synchronize the stream to detect asynchronous errors.
cudaError_t ColorTwist(ColorTwistImageView image, ColorTwistMatrix matrix,
                       cudaStream_t stream = nullptr);

}  // namespace cv_cuda_pipeline
#endif  // CV_CUDA_PIPELINE_CALCULATORS_IMAGE_PROCESS_CUDA_COLOR_TWIST_KERNEL_H_
