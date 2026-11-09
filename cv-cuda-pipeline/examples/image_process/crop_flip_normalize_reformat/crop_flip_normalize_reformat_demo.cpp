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

#include <cuda_runtime.h>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "cv-cuda-pipeline/calculators/image_process/cuda/crop_flip_normalize_reformat_kernel.h"
#include "cv-cuda-pipeline/framework/port/opencv_core_inc.h"
#include "cv-cuda-pipeline/framework/port/opencv_imgcodecs_inc.h"
#include "cv-cuda-pipeline/framework/port/opencv_imgproc_inc.h"

namespace {
void Check(cudaError_t status, const char* operation) {
  if (status != cudaSuccess)
    throw std::runtime_error(std::string(operation) + ": " +
                             cudaGetErrorString(status));
}
class Buffer {
 public:
  explicit Buffer(std::size_t bytes) {
    Check(cudaMalloc(&data_, bytes), "cudaMalloc");
  }
  ~Buffer() { cudaFree(data_); }
  Buffer(const Buffer&) = delete;
  Buffer& operator=(const Buffer&) = delete;
  void* get() const { return data_; }

 private:
  void* data_ = nullptr;
};
struct Options {
  std::string input, output = "cat_crop_flip_normalize_reformat.png";
};
Options Parse(int argc, char** argv) {
  Options options;
  for (int i = 1; i < argc; ++i) {
    const std::string arg = argv[i];
    if (arg == "--help" || arg == "-h") {
      std::cout << "Usage: " << argv[0] << " --input IMAGE [--output IMAGE]\n";
      std::exit(0);
    }
    if (arg != "--input" && arg != "--output")
      throw std::invalid_argument("Unknown option: " + arg);
    if (++i == argc) throw std::invalid_argument("Missing value for " + arg);
    if (arg == "--input") options.input = argv[i];
    if (arg == "--output") options.output = argv[i];
  }
  if (options.input.empty() || options.output.empty())
    throw std::invalid_argument("--input and --output must not be empty");
  return options;
}
void Run(const Options& options) {
  const cv::Mat input = cv::imread(options.input, cv::IMREAD_COLOR);
  if (input.empty())
    throw std::runtime_error("Cannot read input: " + options.input);
  const int width = static_cast<int>(input.cols * .8);
  const int height = static_cast<int>(input.rows * .8);
  if (width <= 0 || height <= 0)
    throw std::runtime_error("Input is too small for 80% crop");
  const int x = static_cast<int>(input.cols * .1),
            y = static_cast<int>(input.rows * .1);
  cv::Mat rgb;
  cv::cvtColor(input, rgb, cv::COLOR_BGR2RGB);
  const std::size_t srcrow = static_cast<std::size_t>(rgb.cols) * 3;
  const std::size_t row = static_cast<std::size_t>(width) * sizeof(float);
  const std::size_t plane = row * height;
  Buffer src(srcrow * rgb.rows), dst(plane * 3);
  Check(cudaMemcpy2D(src.get(), srcrow, rgb.data, rgb.step, srcrow, rgb.rows,
                     cudaMemcpyHostToDevice),
        "Upload input");
  Check(cv_cuda_pipeline::CropFlipNormalizeReformat(
            {static_cast<std::uint8_t*>(src.get()), srcrow, rgb.cols, rgb.rows,
             static_cast<float*>(dst.get()), row, plane, x, y, width, height}),
        "Crop, flip and normalize");
  std::vector<float> normalized(static_cast<std::size_t>(width) * height * 3);
  Check(cudaMemcpy(normalized.data(), dst.get(), plane * 3,
                   cudaMemcpyDeviceToHost),
        "Download output");
  const float base[] = {.485f, .456f, .406f}, scale[] = {.229f, .224f, .225f};
  cv::Mat rgb_output(height, width, CV_8UC3), result;
  const std::size_t pixels = static_cast<std::size_t>(width) * height;
  for (int yy = 0; yy < height; ++yy)
    for (int xx = 0; xx < width; ++xx)
      for (int c = 0; c < 3; ++c) {
        const float value =
            (normalized[c * pixels + static_cast<std::size_t>(yy) * width +
                        xx] *
                 scale[c] +
             base[c]) *
            255.f;
        rgb_output.at<cv::Vec3b>(yy, xx)[c] =
            static_cast<std::uint8_t>(std::clamp(value, 0.f, 255.f));
      }
  cv::cvtColor(rgb_output, result, cv::COLOR_RGB2BGR);
  if (!cv::imwrite(options.output, result))
    throw std::runtime_error("Cannot write output: " + options.output);
  std::cout << "Saved image to " << options.output << " (" << width << "x"
            << height << ")\n";
}
}  // namespace
int main(int argc, char** argv) {
  try {
    Run(Parse(argc, argv));
    return 0;
  } catch (const std::exception& e) {
    std::cerr << "crop_flip_normalize_reformat_demo: " << e.what() << '\n';
    return 1;
  }
}
