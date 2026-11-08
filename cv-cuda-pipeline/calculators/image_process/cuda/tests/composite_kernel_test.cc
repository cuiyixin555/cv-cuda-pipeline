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

#include "cv-cuda-pipeline/calculators/image_process/cuda/composite_kernel.h"

#include <cuda_runtime.h>

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <vector>

namespace {
void Check(cudaError_t status) {
  if (status != cudaSuccess)
    throw std::runtime_error(cudaGetErrorString(status));
}
void Require(bool condition, const char* message) {
  if (!condition) throw std::runtime_error(message);
}
class Buffer {
 public:
  explicit Buffer(std::size_t size) { Check(cudaMalloc(&ptr_, size)); }
  ~Buffer() { cudaFree(ptr_); }
  Buffer(const Buffer&) = delete;
  Buffer& operator=(const Buffer&) = delete;
  std::uint8_t* get() const { return static_cast<std::uint8_t*>(ptr_); }

 private:
  void* ptr_ = nullptr;
};
void Test(int width, int height, int channels) {
  const std::size_t w = width, h = height;
  const std::size_t fs = w * 3 + 5, bs = w * 3 + 7, ms = w + 3;
  const std::size_t os = w * channels + 11;
  std::vector<std::uint8_t> fg(fs * h, 0x11), bg(bs * h, 0x22);
  std::vector<std::uint8_t> mask(ms * h, 0x33), output(os * h, 0xa5);
  for (std::size_t y = 0; y < h; ++y)
    for (std::size_t x = 0; x < w; ++x) {
      mask[y * ms + x] = (x + y * 31) % 256;
      for (int c = 0; c < 3; ++c) {
        fg[y * fs + x * 3 + c] = (x * 19 + y * 61 + c * 83) % 256;
        bg[y * bs + x * 3 + c] = (x * 71 + y * 23 + c * 37 + 17) % 256;
      }
    }
  Buffer df(fg.size()), db(bg.size()), dm(mask.size()), dout(output.size());
  cudaStream_t stream;
  Check(cudaStreamCreate(&stream));
  Check(cudaMemcpyAsync(df.get(), fg.data(), fg.size(), cudaMemcpyHostToDevice,
                        stream));
  Check(cudaMemcpyAsync(db.get(), bg.data(), bg.size(), cudaMemcpyHostToDevice,
                        stream));
  Check(cudaMemcpyAsync(dm.get(), mask.data(), mask.size(),
                        cudaMemcpyHostToDevice, stream));
  Check(cudaMemcpyAsync(dout.get(), output.data(), output.size(),
                        cudaMemcpyHostToDevice, stream));
  cv_cuda_pipeline::CompositeImageView view{df.get(), fs,     db.get(),   bs,
                                            dm.get(), ms,     dout.get(), os,
                                            width,    height, channels};
  Check(cv_cuda_pipeline::Composite(view, stream));
  Check(cudaMemcpyAsync(output.data(), dout.get(), output.size(),
                        cudaMemcpyDeviceToHost, stream));
  Check(cudaStreamSynchronize(stream));
  Check(cudaStreamDestroy(stream));
  for (std::size_t y = 0; y < h; ++y) {
    for (std::size_t x = 0; x < w; ++x) {
      for (int c = 0; c < 3; ++c) {
        const int a = mask[y * ms + x];
        const double value =
            (fg[y * fs + x * 3 + c] * a + bg[y * bs + x * 3 + c] * (255 - a)) /
            255.0;
        Require(
            output[y * os + x * channels + c] == static_cast<int>(value + .5),
            "Composited channel mismatch");
      }
      if (channels == 4)
        Require(output[y * os + x * channels + 3] == 255,
                "Output alpha mismatch");
    }
    for (std::size_t x = w * channels; x < os; ++x)
      Require(output[y * os + x] == 0xa5, "Output padding modified");
  }
  auto bad = view;
  bad.foreground = nullptr;
  Require(cv_cuda_pipeline::Composite(bad) == cudaErrorInvalidValue,
          "Null foreground accepted");
  bad = view;
  bad.background = nullptr;
  Require(cv_cuda_pipeline::Composite(bad) == cudaErrorInvalidValue,
          "Null background accepted");
  bad = view;
  bad.mask = nullptr;
  Require(cv_cuda_pipeline::Composite(bad) == cudaErrorInvalidValue,
          "Null mask accepted");
  bad = view;
  bad.output = nullptr;
  Require(cv_cuda_pipeline::Composite(bad) == cudaErrorInvalidValue,
          "Null output accepted");
  bad = view;
  bad.output = df.get();
  Require(cv_cuda_pipeline::Composite(bad) == cudaErrorInvalidValue,
          "Aliased output accepted");
  bad = view;
  bad.output = db.get();
  Require(cv_cuda_pipeline::Composite(bad) == cudaErrorInvalidValue,
          "Aliased background accepted");
  bad = view;
  bad.output = dm.get();
  Require(cv_cuda_pipeline::Composite(bad) == cudaErrorInvalidValue,
          "Aliased mask accepted");
  bad = view;
  bad.width = 0;
  Require(cv_cuda_pipeline::Composite(bad) == cudaErrorInvalidValue,
          "Zero width accepted");
  bad = view;
  bad.height = -1;
  Require(cv_cuda_pipeline::Composite(bad) == cudaErrorInvalidValue,
          "Negative height accepted");
  bad = view;
  bad.output_channels = 2;
  Require(cv_cuda_pipeline::Composite(bad) == cudaErrorInvalidValue,
          "Invalid channels accepted");
  bad.output_channels = 5;
  Require(cv_cuda_pipeline::Composite(bad) == cudaErrorInvalidValue,
          "Invalid channels accepted");
  bad = view;
  bad.foreground_stride = w * 3 - 1;
  Require(cv_cuda_pipeline::Composite(bad) == cudaErrorInvalidValue,
          "Short foreground stride accepted");
  bad = view;
  bad.background_stride = w * 3 - 1;
  Require(cv_cuda_pipeline::Composite(bad) == cudaErrorInvalidValue,
          "Short background stride accepted");
  bad = view;
  bad.mask_stride = w - 1;
  Require(cv_cuda_pipeline::Composite(bad) == cudaErrorInvalidValue,
          "Short mask stride accepted");
  bad = view;
  bad.output_stride = w * channels - 1;
  Require(cv_cuda_pipeline::Composite(bad) == cudaErrorInvalidValue,
          "Short output stride accepted");
  bad = view;
  bad.height = 2;
  bad.mask_stride = std::numeric_limits<std::size_t>::max();
  Require(cv_cuda_pipeline::Composite(bad) == cudaErrorInvalidValue,
          "Overflow accepted");
  bad = view;
  bad.height = 2;
  bad.foreground_stride = std::numeric_limits<std::size_t>::max();
  Require(cv_cuda_pipeline::Composite(bad) == cudaErrorInvalidValue,
          "Foreground overflow accepted");
  bad = view;
  bad.height = 2;
  bad.background_stride = std::numeric_limits<std::size_t>::max();
  Require(cv_cuda_pipeline::Composite(bad) == cudaErrorInvalidValue,
          "Background overflow accepted");
  bad = view;
  bad.height = 2;
  bad.output_stride = std::numeric_limits<std::size_t>::max();
  Require(cv_cuda_pipeline::Composite(bad) == cudaErrorInvalidValue,
          "Output overflow accepted");
  auto same_input = view;
  same_input.background = same_input.foreground;
  same_input.background_stride = same_input.foreground_stride;
  Check(cv_cuda_pipeline::Composite(same_input));
  Check(cudaMemcpy(output.data(), dout.get(), output.size(),
                   cudaMemcpyDeviceToHost));
  for (std::size_t y = 0; y < h; ++y)
    for (std::size_t x = 0; x < w; ++x)
      for (int c = 0; c < 3; ++c)
        Require(output[y * os + x * channels + c] == fg[y * fs + x * 3 + c],
                "Shared inputs differ from output");
}
}  // namespace
int main() {
  try {
    for (int channels : {3, 4}) {
      Test(1, 1, channels);
      Test(1, 257, channels);
      Test(256, 1, channels);
      Test(31, 23, channels);
      Test(720, 720, channels);
      Test(2876, 1699, channels);
    }
    std::cout << "Composite tests passed\n";
    return 0;
  } catch (const std::exception& e) {
    std::cerr << "Composite test failed: " << e.what() << '\n';
    return 1;
  }
}
