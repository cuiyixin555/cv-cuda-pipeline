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

#include "cv-cuda-pipeline/calculators/image_process/cuda/rotate_kernel.h"

#include <cuda_runtime.h>

#include <algorithm>
#include <cmath>
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
  explicit Buffer(std::size_t bytes) { Check(cudaMalloc(&data_, bytes)); }
  ~Buffer() { cudaFree(data_); }
  Buffer(const Buffer&) = delete;
  Buffer& operator=(const Buffer&) = delete;
  std::uint8_t* get() const { return static_cast<std::uint8_t*>(data_); }

 private:
  void* data_ = nullptr;
};
std::uint8_t Sample(const std::vector<std::uint8_t>& src, std::size_t stride,
                    int width, int height, int x, int y, int c) {
  if (x < 0 || x >= width || y < 0 || y >= height) return 0;
  return src[static_cast<std::size_t>(y) * stride + x * 3 + c];
}
void Test(int width, int height, int pattern) {
  const std::size_t row = static_cast<std::size_t>(width) * 3;
  const std::size_t src_stride = row + 5, dst_stride = row + 7;
  std::vector<std::uint8_t> src(src_stride * height, 0x5a);
  std::vector<std::uint8_t> dst(dst_stride * height, 0xa5);
  for (int y = 0; y < height; ++y)
    for (int x = 0; x < width; ++x)
      for (int c = 0; c < 3; ++c)
        src[y * src_stride + x * 3 + c] =
            pattern == 0   ? 127
            : pattern == 1 ? ((x == width / 2 && y == height / 2) ? 255 : 0)
                           : (x * 37 + y * 61 + c * 83) % 256;
  constexpr double kPi = 3.14159265358979323846;
  const double radians = 45.0 * kPi / 180.0;
  const float cosine = static_cast<float>(std::cos(radians));
  const float sine = static_cast<float>(std::sin(radians));
  const double cx = width / 2.0, cy = height / 2.0;
  const float shift_x = static_cast<float>(cx * (1.0 - std::cos(radians)) -
                                           cy * std::sin(radians));
  const float shift_y = static_cast<float>(cy * (1.0 - std::cos(radians)) +
                                           cx * std::sin(radians));
  std::vector<std::uint8_t> expected(dst_stride * height, 0xa5);
  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      const float dx = static_cast<float>(x) - shift_x;
      const float dy = static_cast<float>(y) - shift_y;
      const float sx = dx * cosine - dy * sine;
      const float sy = dx * sine + dy * cosine;
      auto* out = expected.data() + y * dst_stride + x * 3;
      if (!(sx > -0.5f && sx < width && sy > -0.5f && sy < height)) {
        out[0] = out[1] = out[2] = 0;
        continue;
      }
      const int ix = static_cast<int>(std::floor(sx));
      const int iy = static_cast<int>(std::floor(sy));
      const float fx = sx - ix, fy = sy - iy;
      for (int c = 0; c < 3; ++c) {
        const float top =
            Sample(src, src_stride, width, height, ix, iy, c) * (1.f - fx) +
            Sample(src, src_stride, width, height, ix + 1, iy, c) * fx;
        const float bottom =
            Sample(src, src_stride, width, height, ix, iy + 1, c) * (1.f - fx) +
            Sample(src, src_stride, width, height, ix + 1, iy + 1, c) * fx;
        out[c] = static_cast<std::uint8_t>(std::clamp(
            static_cast<int>(std::nearbyint(top * (1.f - fy) + bottom * fy)), 0,
            255));
      }
    }
  }
  Buffer in(src.size()), out(dst.size());
  cudaStream_t stream;
  Check(cudaStreamCreate(&stream));
  Check(cudaMemcpyAsync(in.get(), src.data(), src.size(),
                        cudaMemcpyHostToDevice, stream));
  Check(cudaMemcpyAsync(out.get(), dst.data(), dst.size(),
                        cudaMemcpyHostToDevice, stream));
  cv_cuda_pipeline::RotateImageView view{in.get(),   src_stride, out.get(),
                                         dst_stride, width,      height};
  Check(cv_cuda_pipeline::Rotate45(view, stream));
  Check(cudaMemcpyAsync(dst.data(), out.get(), dst.size(),
                        cudaMemcpyDeviceToHost, stream));
  Check(cudaStreamSynchronize(stream));
  Check(cudaStreamDestroy(stream));
  for (int y = 0; y < height; ++y) {
    for (std::size_t x = 0; x < row; ++x)
      Require(std::abs(static_cast<int>(dst[y * dst_stride + x]) -
                       static_cast<int>(expected[y * dst_stride + x])) <= 1,
              "Rotation mismatch");
    for (std::size_t x = row; x < dst_stride; ++x)
      Require(dst[y * dst_stride + x] == 0xa5, "Padding modified");
  }
  auto bad = view;
  bad.src = nullptr;
  Require(cv_cuda_pipeline::Rotate45(bad) == cudaErrorInvalidValue,
          "Null input accepted");
  bad = view;
  bad.dst = nullptr;
  Require(cv_cuda_pipeline::Rotate45(bad) == cudaErrorInvalidValue,
          "Null output accepted");
  bad = view;
  bad.dst = in.get();
  Require(cv_cuda_pipeline::Rotate45(bad) == cudaErrorInvalidValue,
          "In-place accepted");
  bad = view;
  bad.width = 0;
  Require(cv_cuda_pipeline::Rotate45(bad) == cudaErrorInvalidValue,
          "Invalid width accepted");
  bad = view;
  bad.height = -1;
  Require(cv_cuda_pipeline::Rotate45(bad) == cudaErrorInvalidValue,
          "Invalid height accepted");
  bad = view;
  bad.src_stride = row - 1;
  Require(cv_cuda_pipeline::Rotate45(bad) == cudaErrorInvalidValue,
          "Short input stride accepted");
  bad = view;
  bad.dst_stride = row - 1;
  Require(cv_cuda_pipeline::Rotate45(bad) == cudaErrorInvalidValue,
          "Short output stride accepted");
  bad = view;
  bad.height = 2;
  bad.dst_stride = std::numeric_limits<std::size_t>::max();
  Require(cv_cuda_pipeline::Rotate45(bad) == cudaErrorInvalidValue,
          "Stride overflow accepted");
}
}  // namespace
int main() {
  try {
    for (int pattern = 0; pattern < 3; ++pattern) {
      Test(1, 1, pattern);
      Test(1, 5, pattern);
      Test(5, 1, pattern);
      Test(2, 3, pattern);
      Test(31, 23, pattern);
    }
    Test(720, 720, 2);
    Test(2876, 1699, 2);
    std::cout << "Rotate tests passed\n";
    return 0;
  } catch (const std::exception& e) {
    std::cerr << "Rotate test failed: " << e.what() << '\n';
    return 1;
  }
}
