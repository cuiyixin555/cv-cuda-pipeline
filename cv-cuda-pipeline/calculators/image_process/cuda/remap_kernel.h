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

#ifndef CV_CUDA_PIPELINE_CALCULATORS_IMAGE_PROCESS_CUDA_REMAP_KERNEL_H_
#define CV_CUDA_PIPELINE_CALCULATORS_IMAGE_PROCESS_CUDA_REMAP_KERNEL_H_

#include <cuda_runtime.h>

#include <cstddef>
#include <cstdint>

namespace cv_cuda_pipeline {

// Device buffers for a three-channel uint8 image and a same-size float2 map.
// Map elements contain absolute source coordinates (x, y). All strides are
// in bytes; map pointers and map row strides must be float2-aligned. Each
// dimension must not exceed 2^23, so pixel indices are exactly representable.
struct RemapImageView {
  const std::uint8_t* src;
  std::size_t src_stride;
  std::uint8_t* dst;
  std::size_t dst_stride;
  const float2* map;
  std::size_t map_stride;
  int width;
  int height;
};

// Enqueues bilinear sampling with replicated borders. Map coordinates must be
// finite, and the source, map, and destination buffers must not overlap.
// Buffers remain owned by the caller and must outlive the stream operation.
// Returns argument or launch errors; synchronize to detect asynchronous errors.
cudaError_t RemapLinear(RemapImageView image, cudaStream_t stream = nullptr);

}  // namespace cv_cuda_pipeline

#endif  // CV_CUDA_PIPELINE_CALCULATORS_IMAGE_PROCESS_CUDA_REMAP_KERNEL_H_
