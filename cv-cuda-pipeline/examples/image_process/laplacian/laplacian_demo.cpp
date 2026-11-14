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

#include "cv-cuda-pipeline/calculators/image_process/cuda/laplacian_kernel.h"
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
  void* get() const { return data_; }

 private:
  void* data_ = nullptr;
};
struct Options {
  std::string input, output = "cat_laplacian.png";
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
    if (arg == "--input")
      options.input = argv[i];
    else
      options.output = argv[i];
  }
  if (options.input.empty() || options.output.empty())
    throw std::invalid_argument("--input and --output must not be empty");
  return options;
}
void Run(const Options& options) {
  const cv::Mat input = cv::imread(options.input, cv::IMREAD_COLOR);
  if (input.empty())
    throw std::runtime_error("Cannot read input: " + options.input);
  const std::size_t input_row = static_cast<std::size_t>(input.cols) * 3;
  const std::size_t output_row = input_row;
  Buffer src(input_row * input.rows), dst(output_row * input.rows);
  Check(cudaMemcpy2D(src.get(), input_row, input.data, input.step, input_row,
                     input.rows, cudaMemcpyHostToDevice),
        "Upload input");
  Check(cv_cuda_pipeline::Laplacian3x3(
            {static_cast<const std::uint8_t*>(src.get()), input_row,
             static_cast<std::uint8_t*>(dst.get()), output_row, input.cols,
             input.rows}),
        "Laplacian");
  std::vector<std::uint8_t> response(static_cast<std::size_t>(input.cols) *
                                     input.rows * 3);
  Check(cudaMemcpy2D(response.data(), output_row, dst.get(), output_row,
                     output_row, input.rows, cudaMemcpyDeviceToHost),
        "Download response");
  const auto limits = std::minmax_element(response.begin(), response.end());
  cv::Mat output(input.rows, input.cols, CV_8UC3, cv::Scalar(0, 0, 0));
  if (*limits.second > *limits.first) {
    const float scale =
        255.f / (static_cast<float>(*limits.second) - *limits.first);
    for (int y = 0; y < input.rows; ++y) {
      const auto* src_row =
          response.data() + static_cast<std::size_t>(y) * input.cols * 3;
      auto* dst_row = output.ptr<std::uint8_t>(y);
      for (int x = 0; x < input.cols * 3; ++x) {
        const float value =
            (static_cast<float>(src_row[x]) - *limits.first) * scale;
        dst_row[x] = static_cast<std::uint8_t>(
            std::clamp(static_cast<int>(value), 0, 255));
      }
    }
  }
  if (!cv::imwrite(options.output, output))
    throw std::runtime_error("Cannot write output: " + options.output);
  std::cout << "Saved image to " << options.output << " (" << output.cols << "x"
            << output.rows << ")\n";
}
}  // namespace
int main(int argc, char** argv) {
  try {
    Run(Parse(argc, argv));
    return 0;
  } catch (const std::exception& e) {
    std::cerr << "laplacian_demo: " << e.what() << '\n';
    return 1;
  }
}
