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

#include "cv-cuda-pipeline/calculators/image_process/cuda/remap_kernel.h"

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
  explicit Buffer(std::size_t size) { Check(cudaMalloc(&data_, size)); }
  ~Buffer() { cudaFree(data_); }
  Buffer(const Buffer&) = delete;
  Buffer& operator=(const Buffer&) = delete;
  void* get() const { return data_; }

 private:
  void* data_ = nullptr;
};

int Reference(const std::vector<std::uint8_t>& src, std::size_t stride,
              int width, int height, float2 p, int c) {
  const float x = std::clamp(p.x, 0.f, static_cast<float>(width - 1));
  const float y = std::clamp(p.y, 0.f, static_cast<float>(height - 1));
  const int x0 = static_cast<int>(std::floor(x));
  const int y0 = static_cast<int>(std::floor(y));
  const int x1 = std::min(x0 + 1, width - 1);
  const int y1 = std::min(y0 + 1, height - 1);
  const float wx = x - x0;
  const float wy = y - y0;
  const float value =
      src[y0 * stride + x0 * 3 + c] * ((1.f - wx) * (1.f - wy)) +
      src[y0 * stride + x1 * 3 + c] * (wx * (1.f - wy)) +
      src[y1 * stride + x0 * 3 + c] * ((1.f - wx) * wy) +
      src[y1 * stride + x1 * 3 + c] * (wx * wy);
  return static_cast<int>(std::nearbyint(value));
}

enum class MapType { kIdentity, kDisplaced, kWave };

void TestRemap(int width, int height, MapType type) {
  const std::size_t src_stride = static_cast<std::size_t>(width) * 3 + 5;
  const std::size_t dst_stride = static_cast<std::size_t>(width) * 3 + 7;
  const std::size_t map_elements_per_row = static_cast<std::size_t>(width) + 2;
  const std::size_t map_stride = map_elements_per_row * sizeof(float2);
  std::vector<std::uint8_t> src(src_stride * height, 0x5a);
  std::vector<std::uint8_t> dst(dst_stride * height, 0xa5);
  std::vector<float2> map(map_elements_per_row * height);
  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      for (int c = 0; c < 3; ++c) {
        src[y * src_stride + x * 3 + c] =
            static_cast<std::uint8_t>((x * 37 + y * 61 + c * 83) % 256);
      }
      float2 point;
      if (type == MapType::kIdentity) {
        point = make_float2(x, y);
      } else if (type == MapType::kWave) {
        constexpr float kTwoPi = 6.2831853071795864769f;
        const float amplitude = height * 0.04f;
        point = make_float2(x + amplitude * std::sin(kTwoPi * 3.f / height * y),
                            y + amplitude * std::sin(kTwoPi * 3.f / width * x));
      } else {
        point = make_float2(x + (y % 3 - 1) * 1.25f, y + (x % 3 - 1) * 1.75f);
      }
      map[y * map_elements_per_row + x] = point;
    }
  }
  if (type == MapType::kDisplaced) {
    map[0] = make_float2(-1000000.f, 1000000.f);
    map[width - 1] = make_float2(1000000.f, -1000000.f);
  }
  Buffer device_src(src.size());
  Buffer device_dst(dst.size());
  Buffer device_map(map.size() * sizeof(float2));
  cudaStream_t stream = nullptr;
  Check(cudaStreamCreate(&stream));
  Check(cudaMemcpyAsync(device_src.get(), src.data(), src.size(),
                        cudaMemcpyHostToDevice, stream));
  Check(cudaMemcpyAsync(device_dst.get(), dst.data(), dst.size(),
                        cudaMemcpyHostToDevice, stream));
  Check(cudaMemcpyAsync(device_map.get(), map.data(),
                        map.size() * sizeof(float2), cudaMemcpyHostToDevice,
                        stream));
  const cv_cuda_pipeline::RemapImageView view{
      static_cast<const std::uint8_t*>(device_src.get()),
      src_stride,
      static_cast<std::uint8_t*>(device_dst.get()),
      dst_stride,
      static_cast<const float2*>(device_map.get()),
      map_stride,
      width,
      height};
  Check(cv_cuda_pipeline::RemapLinear(view, stream));
  Check(cudaMemcpyAsync(dst.data(), device_dst.get(), dst.size(),
                        cudaMemcpyDeviceToHost, stream));
  Check(cudaStreamSynchronize(stream));
  Check(cudaStreamDestroy(stream));
  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      for (int c = 0; c < 3; ++c) {
        const int expected = Reference(src, src_stride, width, height,
                                       map[y * map_elements_per_row + x], c);
        const int difference = std::abs(
            static_cast<int>(dst[y * dst_stride + x * 3 + c]) - expected);
        Require(difference <= (type == MapType::kIdentity ? 0 : 1),
                "Output differs from bilinear reference");
      }
    }
    for (std::size_t i = static_cast<std::size_t>(width) * 3; i < dst_stride;
         ++i) {
      Require(dst[y * dst_stride + i] == 0xa5, "Output padding modified");
    }
  }
  auto invalid = view;
  invalid.src = nullptr;
  Require(cv_cuda_pipeline::RemapLinear(invalid) == cudaErrorInvalidValue,
          "Null source accepted");
  invalid = view;
  invalid.width = 0;
  Require(cv_cuda_pipeline::RemapLinear(invalid) == cudaErrorInvalidValue,
          "Zero width accepted");
  invalid = view;
  invalid.width = (1 << 23) + 1;
  Require(cv_cuda_pipeline::RemapLinear(invalid) == cudaErrorInvalidValue,
          "Oversized width accepted");
  invalid = view;
  invalid.src_stride = static_cast<std::size_t>(width) * 3 - 1;
  Require(cv_cuda_pipeline::RemapLinear(invalid) == cudaErrorInvalidValue,
          "Short source stride accepted");
  invalid = view;
  invalid.map_stride = map_stride - 1;
  Require(cv_cuda_pipeline::RemapLinear(invalid) == cudaErrorInvalidValue,
          "Misaligned map stride accepted");
  invalid = view;
  invalid.dst_stride = std::numeric_limits<std::size_t>::max();
  invalid.height = 2;
  Require(cv_cuda_pipeline::RemapLinear(invalid) == cudaErrorInvalidValue,
          "Overflowing stride accepted");
}

}  // namespace

int main() {
  try {
    TestRemap(17, 11, MapType::kIdentity);
    TestRemap(17, 11, MapType::kDisplaced);
    TestRemap(1, 1, MapType::kDisplaced);
    TestRemap(1, 9, MapType::kDisplaced);
    TestRemap(9, 1, MapType::kDisplaced);
    TestRemap(720, 720, MapType::kWave);
    std::cout << "Remap tests passed\n";
    return 0;
  } catch (const std::exception& error) {
    std::cerr << "Remap test failed: " << error.what() << '\n';
    return 1;
  }
}
