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
#include <charconv>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>

#include "cv-cuda-pipeline/calculators/image_process/cuda/center_crop_kernel.h"
#include "cv-cuda-pipeline/framework/port/opencv_core_inc.h"
#include "cv-cuda-pipeline/framework/port/opencv_imgcodecs_inc.h"

namespace {
void Check(cudaError_t status, const char* operation) {
  if (status != cudaSuccess)
    throw std::runtime_error(std::string(operation) + ": " + cudaGetErrorString(status));
}
class Buffer {
 public:
  explicit Buffer(std::size_t bytes) { Check(cudaMalloc(&data_, bytes), "cudaMalloc"); }
  ~Buffer() { cudaFree(data_); }
  Buffer(const Buffer&) = delete;
  Buffer& operator=(const Buffer&) = delete;
  std::uint8_t* get() const { return static_cast<std::uint8_t*>(data_); }
 private:
  void* data_ = nullptr;
};
struct Options {
  std::string input;
  std::string output = "cat_center_crop.png";
  int width = 224;
  int height = 224;
};
int ParsePositiveInt(const std::string& value, const std::string& option) {
  int result = 0;
  const auto parsed = std::from_chars(value.data(), value.data() + value.size(), result);
  if (parsed.ec != std::errc() || parsed.ptr != value.data() + value.size() || result <= 0)
    throw std::invalid_argument(option + " must be a positive integer");
  return result;
}
Options Parse(int argc, char** argv) {
  Options options;
  for (int i = 1; i < argc; ++i) {
    const std::string arg = argv[i];
    if (arg == "--help" || arg == "-h") {
      std::cout << "Usage: " << argv[0]
                << " --input IMAGE [--output IMAGE] [--width N] [--height N]\n";
      std::exit(0);
    }
    if (arg != "--input" && arg != "--output" && arg != "--width" && arg != "--height")
      throw std::invalid_argument("Unknown option: " + arg);
    if (++i == argc) throw std::invalid_argument("Missing value for " + arg);
    const std::string value = argv[i];
    if (arg == "--input") options.input = value;
    if (arg == "--output") options.output = value;
    if (arg == "--width") options.width = ParsePositiveInt(value, arg);
    if (arg == "--height") options.height = ParsePositiveInt(value, arg);
  }
  if (options.input.empty() || options.output.empty())
    throw std::invalid_argument("--input and --output must not be empty");
  return options;
}
void Run(const Options& options) {
  const cv::Mat input = cv::imread(options.input, cv::IMREAD_COLOR);
  if (input.empty()) throw std::runtime_error("Cannot read input: " + options.input);
  const int width = std::min(options.width, input.cols);
  const int height = std::min(options.height, input.rows);
  cv::Mat output(height, width, CV_8UC3);
  const std::size_t src_row = static_cast<std::size_t>(input.cols) * 3;
  const std::size_t dst_row = static_cast<std::size_t>(width) * 3;
  Buffer src(src_row * input.rows), dst(dst_row * height);
  Check(cudaMemcpy2D(src.get(), src_row, input.data, input.step, src_row, input.rows,
                     cudaMemcpyHostToDevice), "Upload input");
  Check(cv_cuda_pipeline::CenterCrop({src.get(), src_row, input.cols, input.rows,
                                     dst.get(), dst_row, width, height}),
        "Center crop");
  Check(cudaMemcpy2D(output.data, output.step, dst.get(), dst_row, dst_row, height,
                     cudaMemcpyDeviceToHost), "Download output");
  if (!cv::imwrite(options.output, output))
    throw std::runtime_error("Cannot write output: " + options.output);
  std::cout << "Saved image to " << options.output << " (" << output.cols
            << "x" << output.rows << ")\n";
}
}  // namespace

int main(int argc, char** argv) {
  try {
    Run(Parse(argc, argv));
    return 0;
  } catch (const std::exception& e) {
    std::cerr << "center_crop_demo: " << e.what() << '\n';
    return 1;
  }
}
