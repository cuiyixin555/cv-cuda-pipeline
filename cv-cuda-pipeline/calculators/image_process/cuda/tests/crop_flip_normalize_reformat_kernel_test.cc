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

#include "cv-cuda-pipeline/calculators/image_process/cuda/crop_flip_normalize_reformat_kernel.h"

#include <cuda_runtime.h>

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <vector>

namespace {
void Check(cudaError_t s) {
  if (s != cudaSuccess) throw std::runtime_error(cudaGetErrorString(s));
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
  void* get() const { return data_; }

 private:
  void* data_ = nullptr;
};
void Test(int sw, int sh, int x, int y, int w, int h) {
  const std::size_t ss = static_cast<std::size_t>(sw) * 3 + 5;
  const std::size_t row = static_cast<std::size_t>(w) + 2;
  const std::size_t plane = row * h + 3;
  std::vector<std::uint8_t> src(ss * sh, 0x5a);
  std::vector<float> output(plane * 3, -99.f);
  for (int yy = 0; yy < sh; ++yy)
    for (int xx = 0; xx < sw; ++xx)
      for (int c = 0; c < 3; ++c)
        src[yy * ss + xx * 3 + c] = (xx * 37 + yy * 61 + c * 83) % 256;
  Buffer in(src.size()), out(output.size() * sizeof(float));
  cudaStream_t stream;
  Check(cudaStreamCreate(&stream));
  Check(cudaMemcpyAsync(in.get(), src.data(), src.size(),
                        cudaMemcpyHostToDevice, stream));
  Check(cudaMemcpyAsync(out.get(), output.data(), output.size() * sizeof(float),
                        cudaMemcpyHostToDevice, stream));
  cv_cuda_pipeline::CropFlipNormalizeView view{
      static_cast<std::uint8_t*>(in.get()),
      ss,
      sw,
      sh,
      static_cast<float*>(out.get()),
      row * sizeof(float),
      plane * sizeof(float),
      x,
      y,
      w,
      h};
  Check(cv_cuda_pipeline::CropFlipNormalizeReformat(view, stream));
  Check(cudaMemcpyAsync(output.data(), out.get(), output.size() * sizeof(float),
                        cudaMemcpyDeviceToHost, stream));
  Check(cudaStreamSynchronize(stream));
  Check(cudaStreamDestroy(stream));
  const float base[] = {.485f, .456f, .406f}, scale[] = {.229f, .224f, .225f};
  for (int c = 0; c < 3; ++c) {
    for (int yy = 0; yy < h; ++yy) {
      for (int xx = 0; xx < w; ++xx) {
        const float v = src[(yy + y) * ss + (x + w - 1 - xx) * 3 + c];
        const float expected =
            (v - base[c]) / std::sqrt(scale[c] * scale[c] + 1e-8f) / 255.f;
        Require(std::abs(output[c * plane + yy * row + xx] - expected) < 1e-5f,
                "Output mismatch");
      }
      for (std::size_t xx = w; xx < row; ++xx)
        Require(output[c * plane + yy * row + xx] == -99.f,
                "Row padding modified");
    }
    for (std::size_t i = row * h; i < plane; ++i)
      Require(output[c * plane + i] == -99.f, "Plane padding modified");
  }
  auto bad = view;
  bad.src = nullptr;
  Require(
      cv_cuda_pipeline::CropFlipNormalizeReformat(bad) == cudaErrorInvalidValue,
      "Null input accepted");
  bad = view;
  bad.dst = nullptr;
  Require(
      cv_cuda_pipeline::CropFlipNormalizeReformat(bad) == cudaErrorInvalidValue,
      "Null output accepted");
  bad = view;
  bad.x = -1;
  Require(
      cv_cuda_pipeline::CropFlipNormalizeReformat(bad) == cudaErrorInvalidValue,
      "Negative origin accepted");
  bad = view;
  bad.width = 0;
  Require(
      cv_cuda_pipeline::CropFlipNormalizeReformat(bad) == cudaErrorInvalidValue,
      "Zero width accepted");
  bad = view;
  bad.y = sh;
  Require(
      cv_cuda_pipeline::CropFlipNormalizeReformat(bad) == cudaErrorInvalidValue,
      "Out-of-bounds crop accepted");
  bad = view;
  bad.dst_row_stride -= 1;
  Require(
      cv_cuda_pipeline::CropFlipNormalizeReformat(bad) == cudaErrorInvalidValue,
      "Misaligned stride accepted");
  bad = view;
  bad.dst_plane_stride = 4;
  if (w * h > 1)
    Require(cv_cuda_pipeline::CropFlipNormalizeReformat(bad) ==
                cudaErrorInvalidValue,
            "Short plane accepted");
  bad = view;
  bad.src_height = 2;
  bad.src_stride = std::numeric_limits<std::size_t>::max();
  bad.y = 0;
  bad.height = 1;
  Require(
      cv_cuda_pipeline::CropFlipNormalizeReformat(bad) == cudaErrorInvalidValue,
      "Overflow accepted");
}
}  // namespace
int main() {
  try {
    Test(1, 1, 0, 0, 1, 1);
    Test(31, 23, 3, 2, 24, 18);
    Test(720, 720, 72, 72, 576, 576);
    Test(2876, 1699, 287, 169, 2300, 1359);
    std::cout << "Crop flip normalize reformat tests passed\n";
    return 0;
  } catch (const std::exception& e) {
    std::cerr << "Crop flip normalize reformat test failed: " << e.what()
              << '\n';
    return 1;
  }
}
