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

#include "cv-cuda-pipeline/calculators/image_process/cuda/channel_reorder_kernel.h"

#include <cuda_runtime.h>

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <vector>

namespace {
void Check(cudaError_t status) {
  if (status != cudaSuccess) throw std::runtime_error(cudaGetErrorString(status));
}
void Require(bool condition, const char* message) {
  if (!condition) throw std::runtime_error(message);
}
class Buffer {
 public:
  explicit Buffer(std::size_t bytes) { Check(cudaMalloc(&data_, bytes)); }
  ~Buffer() { cudaFree(data_); }
  Buffer(const Buffer&) = delete;
  Buffer& operator=(const Buffer&) = delete;
  std::uint8_t* get() const { return static_cast<std::uint8_t*>(data_); }
 private:
  void* data_ = nullptr;
};
void Test(int width, int height, cv_cuda_pipeline::ChannelOrder order) {
  const std::size_t row = static_cast<std::size_t>(width) * 3;
  const std::size_t src_stride = row + 5, dst_stride = row + 7;
  std::vector<std::uint8_t> src(src_stride * height, 0x5a);
  std::vector<std::uint8_t> dst(dst_stride * height, 0xa5);
  for (int y = 0; y < height; ++y)
    for (int x = 0; x < width; ++x)
      for (int c = 0; c < 3; ++c)
        src[y * src_stride + x * 3 + c] = (x * 37 + y * 61 + c * 83) % 256;
  Buffer in(src.size()), out(dst.size());
  cudaStream_t stream;
  Check(cudaStreamCreate(&stream));
  Check(cudaMemcpyAsync(in.get(), src.data(), src.size(), cudaMemcpyHostToDevice, stream));
  Check(cudaMemcpyAsync(out.get(), dst.data(), dst.size(), cudaMemcpyHostToDevice, stream));
  cv_cuda_pipeline::ChannelReorderImageView view{in.get(), src_stride, out.get(),
                                                 dst_stride, width, height};
  Check(cv_cuda_pipeline::ChannelReorder(view, order, stream));
  Check(cudaMemcpyAsync(dst.data(), out.get(), dst.size(), cudaMemcpyDeviceToHost, stream));
  Check(cudaStreamSynchronize(stream));
  Check(cudaStreamDestroy(stream));
  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x)
      for (int c = 0; c < 3; ++c) {
        const int expected = order.channel[c] < 0 ? 0 :
            src[y * src_stride + x * 3 + order.channel[c]];
        Require(dst[y * dst_stride + x * 3 + c] == expected, "Channel mismatch");
      }
    for (std::size_t x = row; x < dst_stride; ++x)
      Require(dst[y * dst_stride + x] == 0xa5, "Output padding modified");
  }
  if (order.channel[0] == 2 && order.channel[1] == 1 && order.channel[2] == 0)
    Require(dst[0] == 166 && dst[1] == 83 && dst[2] == 0,
            "Known RGB swap mismatch");
  auto invalid = view;
  invalid.src = nullptr;
  Require(cv_cuda_pipeline::ChannelReorder(invalid, order) == cudaErrorInvalidValue, "Null source accepted");
  invalid = view; invalid.dst = nullptr;
  Require(cv_cuda_pipeline::ChannelReorder(invalid, order) == cudaErrorInvalidValue, "Null destination accepted");
  invalid = view; invalid.dst = in.get();
  Require(cv_cuda_pipeline::ChannelReorder(invalid, order) == cudaErrorInvalidValue, "In-place buffers accepted");
  invalid = view; invalid.width = 0;
  Require(cv_cuda_pipeline::ChannelReorder(invalid, order) == cudaErrorInvalidValue, "Zero width accepted");
  invalid = view; invalid.height = -1;
  Require(cv_cuda_pipeline::ChannelReorder(invalid, order) == cudaErrorInvalidValue, "Negative height accepted");
  invalid = view; invalid.src_stride = row - 1;
  Require(cv_cuda_pipeline::ChannelReorder(invalid, order) == cudaErrorInvalidValue, "Short input stride accepted");
  invalid = view; invalid.dst_stride = row - 1;
  Require(cv_cuda_pipeline::ChannelReorder(invalid, order) == cudaErrorInvalidValue, "Short output stride accepted");
  invalid = view; invalid.height = 2; invalid.src_stride = std::numeric_limits<std::size_t>::max();
  Require(cv_cuda_pipeline::ChannelReorder(invalid, order) == cudaErrorInvalidValue, "Input stride overflow accepted");
  invalid = view; invalid.height = 2; invalid.dst_stride = std::numeric_limits<std::size_t>::max();
  Require(cv_cuda_pipeline::ChannelReorder(invalid, order) == cudaErrorInvalidValue, "Output stride overflow accepted");
  for (int c = 0; c < 3; ++c) {
    auto invalid_order = order; invalid_order.channel[c] = 3;
    Require(cv_cuda_pipeline::ChannelReorder(view, invalid_order) == cudaErrorInvalidValue,
            "Invalid channel index accepted");
  }
}
}  // namespace

int main() {
  try {
    std::array<int, 3> order = {0, 1, 2};
    do {
      Test(31, 23, {{order[0], order[1], order[2]}});
    } while (std::next_permutation(order.begin(), order.end()));
    Test(1, 1, {{2, 1, 0}});
    Test(1, 9, {{-1, 2, 2}});
    Test(9, 1, {{0, -9, 0}});
    Test(720, 720, {{2, 1, 0}});
    Test(2876, 1699, {{2, 1, 0}});
    std::cout << "Channel reorder tests passed\n";
    return 0;
  } catch (const std::exception& e) {
    std::cerr << "Channel reorder test failed: " << e.what() << '\n';
    return 1;
  }
}
