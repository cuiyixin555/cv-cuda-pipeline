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

#include "cv-cuda-pipeline/calculators/image_process/cuda/resize_crop_convert_reformat_kernel.h"

#include <cuda_runtime.h>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
void Check(cudaError_t status, const char* operation) {
  if (status != cudaSuccess)
    throw std::runtime_error(std::string(operation) + ": " +
                             cudaGetErrorString(status));
}
void Require(bool condition, const char* message) {
  if (!condition) throw std::runtime_error(message);
}
class Buffer {
 public:
  explicit Buffer(std::size_t size) {
    Check(cudaMalloc(&data_, size), "cudaMalloc");
  }
  ~Buffer() { cudaFree(data_); }
  Buffer(const Buffer&) = delete;
  Buffer& operator=(const Buffer&) = delete;
  void* get() const { return data_; }

 private:
  void* data_ = nullptr;
};

void Test(int src_width, int src_height, int resize_width, int resize_height,
          int crop_x, int crop_y, int crop_width, int crop_height) {
  const std::size_t src_stride = static_cast<std::size_t>(src_width) * 3 + 5;
  const std::size_t dst_row_stride =
      (static_cast<std::size_t>(crop_width) + 2) * sizeof(float);
  const std::size_t dst_plane_stride =
      dst_row_stride * crop_height + 3 * sizeof(float);
  const std::size_t dst_bytes = dst_plane_stride * 3;
  std::vector<std::uint8_t> input(src_stride * src_height, 0x39);
  std::vector<float> output(dst_bytes / sizeof(float), -999.0f);
  for (int y = 0; y < src_height; ++y)
    for (int x = 0; x < src_width; ++x)
      for (int c = 0; c < 3; ++c)
        input[static_cast<std::size_t>(y) * src_stride +
              static_cast<std::size_t>(x) * 3 + c] =
            static_cast<std::uint8_t>((x * 37 + y * 61 + c * 83) % 256);
  Buffer device_input(input.size()), device_output(dst_bytes);
  Check(cudaMemcpy(device_input.get(), input.data(), input.size(),
                   cudaMemcpyHostToDevice),
        "upload input");
  Check(cudaMemcpy(device_output.get(), output.data(), dst_bytes,
                   cudaMemcpyHostToDevice),
        "initialize output");
  Check(cv_cuda_pipeline::ResizeCropConvertReformat(
            {static_cast<const std::uint8_t*>(device_input.get()), src_stride,
             src_width, src_height, static_cast<float*>(device_output.get()),
             dst_row_stride, dst_plane_stride, resize_width, resize_height,
             crop_x, crop_y, crop_width, crop_height}),
        "launch fused transform");
  Check(cudaMemcpy(output.data(), device_output.get(), dst_bytes,
                   cudaMemcpyDeviceToHost),
        "download output");

  const float scale_x = static_cast<float>(src_width) / resize_width;
  const float scale_y = static_cast<float>(src_height) / resize_height;
  for (int c = 0; c < 3; ++c) {
    for (int y = 0; y < crop_height; ++y) {
      for (int x = 0; x < crop_width; ++x) {
        const int resized_x = crop_x + x, resized_y = crop_y + y;
        const float fx = (resized_x + 0.5f) * scale_x - 0.5f;
        const float fy = (resized_y + 0.5f) * scale_y - 0.5f;
        const int ix = static_cast<int>(std::floor(fx));
        const int iy = static_cast<int>(std::floor(fy));
        const float wx = ix < 0 ? 0.0f : (ix >= src_width - 1 ? 1.0f : fx - ix);
        const float wy =
            iy < 0 ? 0.0f : (iy >= src_height - 1 ? 1.0f : fy - iy);
        const int x0 = std::clamp(ix, 0, src_width - 1);
        const int y0 = std::clamp(iy, 0, src_height - 1);
        const int x1 = std::min(x0 + 1, src_width - 1);
        const int y1 = std::min(y0 + 1, src_height - 1);
        const int source_channel = 2 - c;
        const auto at = [&](int px, int py) {
          return input[static_cast<std::size_t>(py) * src_stride +
                       static_cast<std::size_t>(px) * 3 + source_channel];
        };
        const float expected = at(x0, y0) * ((1.0f - wx) * (1.0f - wy)) +
                               at(x1, y0) * (wx * (1.0f - wy)) +
                               at(x0, y1) * ((1.0f - wx) * wy) +
                               at(x1, y1) * (wx * wy);
        const auto* row = reinterpret_cast<const float*>(
            reinterpret_cast<const std::uint8_t*>(output.data()) +
            c * dst_plane_stride + y * dst_row_stride);
        Require(std::abs(row[x] - expected) < 1e-3f,
                "resized/cropped planar pixel mismatch");
      }
      const auto* row = reinterpret_cast<const float*>(
          reinterpret_cast<const std::uint8_t*>(output.data()) +
          c * dst_plane_stride + y * dst_row_stride);
      for (std::size_t x = crop_width; x < dst_row_stride / sizeof(float); ++x)
        Require(row[x] == -999.0f, "destination row padding was modified");
    }
    const auto plane_elements = dst_plane_stride / sizeof(float);
    for (std::size_t i = dst_row_stride * crop_height / sizeof(float);
         i < plane_elements; ++i)
      Require(output[c * plane_elements + i] == -999.0f,
              "destination plane padding was modified");
  }
}

void TestInvalidInputs() {
  std::uint8_t src = 0;
  float dst = 0;
  const cv_cuda_pipeline::ResizeCropConvertReformatView valid{
      &src, 3, 1, 1, &dst, sizeof(float), sizeof(float), 1, 1, 0, 0, 1, 1};
  auto bad = valid;
  bad.src = nullptr;
  Require(
      cv_cuda_pipeline::ResizeCropConvertReformat(bad) == cudaErrorInvalidValue,
      "null source accepted");
  bad = valid;
  bad.dst = nullptr;
  Require(
      cv_cuda_pipeline::ResizeCropConvertReformat(bad) == cudaErrorInvalidValue,
      "null destination accepted");
  bad = valid;
  bad.crop_x = 1;
  Require(
      cv_cuda_pipeline::ResizeCropConvertReformat(bad) == cudaErrorInvalidValue,
      "crop outside resized image accepted");
  bad = valid;
  bad.dst_row_stride = 1;
  Require(
      cv_cuda_pipeline::ResizeCropConvertReformat(bad) == cudaErrorInvalidValue,
      "unaligned output stride accepted");
}
}  // namespace

int main() {
  try {
    Test(7, 5, 4, 3, 1, 1, 2, 2);
    Test(31, 23, 19, 13, 0, 0, 19, 13);
    TestInvalidInputs();
    std::cout << "Resize/crop/convert/reformat tests passed\n";
    return 0;
  } catch (const std::exception& e) {
    std::cerr << "Resize/crop/convert/reformat test failed: " << e.what()
              << '\n';
    return 1;
  }
}
