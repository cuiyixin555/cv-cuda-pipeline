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

#ifndef CV_CUDA_PIPELINE_CALCULATORS_IMAGE_PROCESS_CUDA_COMPOSITE_KERNEL_H_
#define CV_CUDA_PIPELINE_CALCULATORS_IMAGE_PROCESS_CUDA_COMPOSITE_KERNEL_H_

#include <cuda_runtime_api.h>

#include <cstddef>
#include <cstdint>

namespace cv_cuda_pipeline {

struct CompositeImageView {
  const std::uint8_t* foreground;
  std::size_t foreground_stride;
  const std::uint8_t* background;
  std::size_t background_stride;
  const std::uint8_t* mask;
  std::size_t mask_stride;
  std::uint8_t* output;
  std::size_t output_stride;
  int width;
  int height;
  int output_channels;
};

// U8 interleaved RGB input and single-channel U8 alpha mask. Output has three
// channels, or four channels with alpha set to 255. Strides are in bytes.
// Output must not overlap any input. The inputs may overlap each other. Keep
// all buffers valid until stream completion and synchronize to detect errors.
cudaError_t Composite(CompositeImageView image, cudaStream_t stream = nullptr);

}  // namespace cv_cuda_pipeline
#endif  // CV_CUDA_PIPELINE_CALCULATORS_IMAGE_PROCESS_CUDA_COMPOSITE_KERNEL_H_
