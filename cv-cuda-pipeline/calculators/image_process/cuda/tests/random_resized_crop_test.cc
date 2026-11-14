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

#include "cv-cuda-pipeline/calculators/image_process/cuda/random_resized_crop.h"

#include <iostream>
#include <stdexcept>

namespace {
void Require(bool condition, const char* message) {
  if (!condition) throw std::runtime_error(message);
}
void TestDeterministicCrop() {
  cv_cuda_pipeline::CropRectangle first{}, repeated{};
  Require(cv_cuda_pipeline::SelectRandomResizedCrop(
              720, 480, 0.08f, 1.0f, 0.75f, 1.3333333f, 42, &first),
          "valid random crop failed");
  Require(cv_cuda_pipeline::SelectRandomResizedCrop(
              720, 480, 0.08f, 1.0f, 0.75f, 1.3333333f, 42, &repeated),
          "repeated random crop failed");
  Require(first.x == repeated.x && first.y == repeated.y &&
              first.width == repeated.width && first.height == repeated.height,
          "same seed produced different crop");
  Require(first.x >= 0 && first.y >= 0 && first.width > 0 && first.height > 0 &&
              first.x + first.width <= 720 && first.y + first.height <= 480,
          "random crop exceeds image bounds");
  const double area =
      static_cast<double>(first.width) * first.height / (720.0 * 480.0);
  const double ratio = static_cast<double>(first.width) / first.height;
  Require(area >= 0.08 && area <= 1.0 && ratio >= 0.75 && ratio <= 1.333334,
          "random crop outside requested scale or ratio bounds");
}
void TestCenteredFallback() {
  cv_cuda_pipeline::CropRectangle crop{};
  Require(cv_cuda_pipeline::SelectRandomResizedCrop(100, 100, 1.0f, 1.0f, 2.0f,
                                                    3.0f, 42, &crop),
          "fallback crop failed");
  Require(crop.x == 0 && crop.y == 25 && crop.width == 100 && crop.height == 50,
          "aspect-ratio fallback is incorrect");
}
void TestInvalidArguments() {
  cv_cuda_pipeline::CropRectangle crop{};
  Require(!cv_cuda_pipeline::SelectRandomResizedCrop(10, 10, 0.0f, 1.0f, 0.75f,
                                                     1.333f, 1, &crop),
          "zero minimum scale accepted");
  Require(!cv_cuda_pipeline::SelectRandomResizedCrop(10, 10, 0.1f, 1.0f, 2.0f,
                                                     1.0f, 1, &crop),
          "reversed aspect range accepted");
  Require(!cv_cuda_pipeline::SelectRandomResizedCrop(10, 10, 0.1f, 1.0f, 0.75f,
                                                     1.333f, 1, nullptr),
          "null output accepted");
}
}  // namespace

int main() {
  try {
    TestDeterministicCrop();
    TestCenteredFallback();
    TestInvalidArguments();
    std::cout << "Random resized crop tests passed\n";
    return 0;
  } catch (const std::exception& e) {
    std::cerr << "Random resized crop test failed: " << e.what() << '\n';
    return 1;
  }
}
