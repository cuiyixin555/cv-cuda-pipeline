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

#include "cv-cuda-pipeline/calculators/image_process/cuda/composite_kernel.h"
#include "cv-cuda-pipeline/calculators/image_process/cuda/resize_kernel.h"
#include "cv-cuda-pipeline/framework/port/opencv_core_inc.h"
#include "cv-cuda-pipeline/framework/port/opencv_imgcodecs_inc.h"

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
  std::uint8_t* get() const { return static_cast<std::uint8_t*>(data_); }

 private:
  void* data_ = nullptr;
};
struct Options {
  std::string input, background;
  std::string output = "cat_composite.png";
};
Options Parse(int argc, char** argv) {
  Options options;
  for (int i = 1; i < argc; ++i) {
    const std::string arg = argv[i];
    if (arg == "--help" || arg == "-h") {
      std::cout << "Usage: " << argv[0]
                << " --input IMAGE --background IMAGE [--output IMAGE]\n";
      std::exit(0);
    }
    if (arg != "--input" && arg != "--background" && arg != "--output")
      throw std::invalid_argument("Unknown option: " + arg);
    if (++i == argc) throw std::invalid_argument("Missing value for " + arg);
    if (arg == "--input") options.input = argv[i];
    if (arg == "--background") options.background = argv[i];
    if (arg == "--output") options.output = argv[i];
  }
  if (options.input.empty() || options.background.empty() ||
      options.output.empty())
    throw std::invalid_argument(
        "--input, --background and --output must not be empty");
  return options;
}
void Run(const Options& options) {
  const cv::Mat foreground = cv::imread(options.input, cv::IMREAD_COLOR);
  if (foreground.empty())
    throw std::runtime_error("Cannot read foreground: " + options.input);
  const cv::Mat background = cv::imread(options.background, cv::IMREAD_COLOR);
  if (background.empty())
    throw std::runtime_error("Cannot read background: " + options.background);
  const std::size_t width = foreground.cols, height = foreground.rows;
  const std::size_t row = width * 3, bytes = row * height;
  const std::size_t bg_row = static_cast<std::size_t>(background.cols) * 3;
  const std::size_t bg_bytes = bg_row * background.rows;
  std::vector<std::uint8_t> mask(width * height);
  const std::int64_t cx = foreground.cols / 2, cy = foreground.rows / 2;
  const std::int64_t radius = std::min(foreground.cols, foreground.rows) / 3;
  for (int y = 0; y < foreground.rows; ++y)
    for (int x = 0; x < foreground.cols; ++x) {
      const auto dx = x - cx, dy = y - cy;
      mask[static_cast<std::size_t>(y) * width + x] =
          dx * dx + dy * dy <= radius * radius ? 255 : 0;
    }
  Buffer fg(bytes), bg_source(bg_bytes), bg(bytes), device_mask(mask.size()),
      dst(bytes);
  Check(cudaMemcpy2D(fg.get(), row, foreground.data, foreground.step, row,
                     height, cudaMemcpyHostToDevice),
        "Upload foreground");
  Check(cudaMemcpy2D(bg_source.get(), bg_row, background.data, background.step,
                     bg_row, background.rows, cudaMemcpyHostToDevice),
        "Upload background");
  Check(cudaMemcpy(device_mask.get(), mask.data(), mask.size(),
                   cudaMemcpyHostToDevice),
        "Upload mask");
  Check(
      cv_cuda_pipeline::ResizeLinear(
          {bg_source.get(), background.cols, background.rows, bg_row, bg_bytes},
          {bg.get(), foreground.cols, foreground.rows, row, bytes}, 1),
      "Resize background");
  Check(cv_cuda_pipeline::Composite({fg.get(), row, bg.get(), row,
                                     device_mask.get(), width, dst.get(), row,
                                     foreground.cols, foreground.rows, 3}),
        "Composite");
  cv::Mat result(foreground.rows, foreground.cols, CV_8UC3);
  Check(cudaMemcpy2D(result.data, result.step, dst.get(), row, row, height,
                     cudaMemcpyDeviceToHost),
        "Download output");
  if (!cv::imwrite(options.output, result))
    throw std::runtime_error("Cannot write output: " + options.output);
  std::cout << "Saved image to " << options.output << " (" << result.cols << "x"
            << result.rows << ")\n";
}
}  // namespace
int main(int argc, char** argv) {
  try {
    Run(Parse(argc, argv));
    return 0;
  } catch (const std::exception& e) {
    std::cerr << "composite_demo: " << e.what() << '\n';
    return 1;
  }
}
