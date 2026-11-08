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

#include "cv-cuda-pipeline/calculators/image_process/cuda/resize_kernel.h"

#include <cuda_runtime.h>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

#include "cv-cuda-pipeline/framework/port/opencv_core_inc.h"
#include "cv-cuda-pipeline/framework/port/opencv_imgproc_inc.h"

namespace {

void Check(cudaError_t result) {
  if (result != cudaSuccess) {
    throw std::runtime_error(cudaGetErrorString(result));
  }
}

void Require(bool condition, const char* message) {
  if (!condition) throw std::runtime_error(message);
}

class Buffer {
 public:
  explicit Buffer(std::size_t size) {
    Check(cudaMalloc(reinterpret_cast<void**>(&data_), size));
  }
  ~Buffer() { cudaFree(data_); }
  Buffer(const Buffer&) = delete;
  Buffer& operator=(const Buffer&) = delete;
  std::uint8_t* get() const { return data_; }

 private:
  std::uint8_t* data_ = nullptr;
};

void TestResize(int sw, int sh, int dw, int dh, int count) {
  const std::size_t src_row = static_cast<std::size_t>(sw) * 3 + 7;
  const std::size_t dst_row = static_cast<std::size_t>(dw) * 3 + 11;
  const std::size_t src_batch = src_row * sh + 5;
  const std::size_t dst_batch = dst_row * dh + 9;
  std::vector<std::uint8_t> source(src_batch * count, 0x5a);
  std::vector<std::uint8_t> output(dst_batch * count, 0xa5);
  for (int n = 0; n < count; ++n) {
    for (int y = 0; y < sh; ++y) {
      for (int x = 0; x < sw * 3; ++x) {
        source[n * src_batch + y * src_row + x] =
            static_cast<std::uint8_t>((x * 37 + y * 61 + n * 79) % 256);
      }
    }
  }
  Buffer device_src(source.size());
  Buffer device_dst(output.size());
  cudaStream_t stream = nullptr;
  Check(cudaStreamCreate(&stream));
  Check(cudaMemcpyAsync(device_src.get(), source.data(), source.size(),
                        cudaMemcpyHostToDevice, stream));
  Check(cudaMemcpyAsync(device_dst.get(), output.data(), output.size(),
                        cudaMemcpyHostToDevice, stream));
  cv_cuda_pipeline::ResizeConstImageView src{device_src.get(), sw, sh, src_row,
                                             src_batch};
  cv_cuda_pipeline::ResizeImageView dst{device_dst.get(), dw, dh, dst_row,
                                        dst_batch};
  Check(cv_cuda_pipeline::ResizeLinear(src, dst, count, stream));
  Check(cudaMemcpyAsync(output.data(), device_dst.get(), output.size(),
                        cudaMemcpyDeviceToHost, stream));
  Check(cudaStreamSynchronize(stream));
  Check(cudaStreamDestroy(stream));

  for (int n = 0; n < count; ++n) {
    cv::Mat input(sh, sw, CV_8UC3, source.data() + n * src_batch, src_row);
    cv::Mat reference;
    cv::resize(input, reference, cv::Size(dw, dh), 0, 0, cv::INTER_LINEAR);
    cv::Mat actual(dh, dw, CV_8UC3, output.data() + n * dst_batch, dst_row);
    Require(cv::norm(reference, actual, cv::NORM_INF) <= 1,
            "GPU result differs from bilinear reference by more than 1");
    for (std::size_t i = 0; i < dst_batch; ++i) {
      if (i >= dst_row * dh ||
          i % dst_row >= static_cast<std::size_t>(dw) * 3) {
        Require(output[n * dst_batch + i] == 0xa5, "Output padding modified");
      }
    }
  }
  Require(cv_cuda_pipeline::ResizeLinear(src, dst, 0) == cudaErrorInvalidValue,
          "Zero batch must be rejected");
  auto invalid_src = src;
  invalid_src.data = nullptr;
  Require(cv_cuda_pipeline::ResizeLinear(invalid_src, dst, count) ==
              cudaErrorInvalidValue,
          "Null input must be rejected");
  invalid_src = src;
  invalid_src.height = 0;
  Require(cv_cuda_pipeline::ResizeLinear(invalid_src, dst, count) ==
              cudaErrorInvalidValue,
          "Zero input height must be rejected");
  invalid_src = src;
  invalid_src.row_stride = std::numeric_limits<std::size_t>::max();
  invalid_src.height = 2;
  Require(cv_cuda_pipeline::ResizeLinear(invalid_src, dst, count) ==
              cudaErrorInvalidValue,
          "Overflowing row stride must be rejected");
  src.row_stride = static_cast<std::size_t>(sw) * 3 - 1;
  Require(
      cv_cuda_pipeline::ResizeLinear(src, dst, count) == cudaErrorInvalidValue,
      "Short input row stride must be rejected");
  src.row_stride = src_row;
  dst.batch_stride = 0;
  Require(
      cv_cuda_pipeline::ResizeLinear(src, dst, count) == cudaErrorInvalidValue,
      "Short output batch stride must be rejected");
}

void TestRoundToEven() {
  const std::uint8_t source[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 6, 10};
  std::uint8_t result[3] = {};
  Buffer input(sizeof(source));
  Buffer output(sizeof(result));
  Check(
      cudaMemcpy(input.get(), source, sizeof(source), cudaMemcpyHostToDevice));
  Check(cv_cuda_pipeline::ResizeLinear({input.get(), 2, 2, 6, 12},
                                       {output.get(), 1, 1, 3, 3}, 1));
  Check(
      cudaMemcpy(result, output.get(), sizeof(result), cudaMemcpyDeviceToHost));
  Require(result[0] == 0 && result[1] == 2 && result[2] == 2,
          "Half-integer values must round to nearest even");
}

}  // namespace

int main() {
  try {
    TestResize(7, 5, 13, 9, 3);
    TestResize(17, 11, 4, 3, 2);
    TestResize(6, 8, 3, 4, 1);
    TestResize(3, 4, 6, 8, 1);
    TestResize(5, 7, 5, 7, 1);
    TestResize(1, 1, 5, 3, 2);
    TestResize(1, 7, 4, 3, 1);
    TestResize(7, 1, 3, 4, 1);
    TestResize(720, 720, 256, 256, 1);
    TestResize(2876, 1699, 640, 480, 1);
    TestRoundToEven();
    std::cout << "Resize tests passed\n";
    return 0;
  } catch (const std::exception& error) {
    std::cerr << "Resize test failed: " << error.what() << '\n';
    return 1;
  }
}
