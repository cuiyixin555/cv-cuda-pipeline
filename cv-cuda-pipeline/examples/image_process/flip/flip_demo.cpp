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

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>

#include "cv-cuda-pipeline/calculators/image_process/cuda/flip_kernel.h"
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
  std::string input;
  std::string output = "cat_flip.png";
  int code = 1;
};
Options Parse(int argc, char** argv) {
  Options options;
  for (int i = 1; i < argc; ++i) {
    const std::string arg = argv[i];
    if (arg == "--help" || arg == "-h") {
      std::cout << "Usage: " << argv[0]
                << " --input IMAGE [--output IMAGE] [--code -1|0|1]\n";
      std::exit(0);
    }
    if (arg != "--input" && arg != "--output" && arg != "--code")
      throw std::invalid_argument("Unknown option: " + arg);
    if (++i == argc) throw std::invalid_argument("Missing value for " + arg);
    const std::string value = argv[i];
    if (arg == "--input") options.input = value;
    if (arg == "--output") options.output = value;
    if (arg == "--code") {
      if (value == "-1")
        options.code = -1;
      else if (value == "0")
        options.code = 0;
      else if (value == "1")
        options.code = 1;
      else
        throw std::invalid_argument("--code must be -1, 0, or 1");
    }
  }
  if (options.input.empty() || options.output.empty())
    throw std::invalid_argument("--input and --output must not be empty");
  return options;
}
void Run(const Options& options) {
  const cv::Mat input = cv::imread(options.input, cv::IMREAD_COLOR);
  if (input.empty())
    throw std::runtime_error("Cannot read input: " + options.input);
  cv::Mat output(input.rows, input.cols, CV_8UC3);
  const std::size_t row_bytes = static_cast<std::size_t>(input.cols) * 3;
  const std::size_t size = row_bytes * input.rows;
  Buffer src(size), dst(size);
  Check(cudaMemcpy2D(src.get(), row_bytes, input.data, input.step, row_bytes,
                     input.rows, cudaMemcpyHostToDevice),
        "Upload input");
  Check(cv_cuda_pipeline::Flip({src.get(), row_bytes, dst.get(), row_bytes,
                                input.cols, input.rows},
                               options.code),
        "Flip");
  Check(cudaMemcpy2D(output.data, output.step, dst.get(), row_bytes, row_bytes,
                     output.rows, cudaMemcpyDeviceToHost),
        "Download output");
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
    std::cerr << "flip_demo: " << e.what() << '\n';
    return 1;
  }
}
