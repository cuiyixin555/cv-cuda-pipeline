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

#include "cv-cuda-pipeline/calculators/image_process/cuda/normalize_kernel.h"
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
  std::string input, output = "cat_normalize.png";
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
  const std::size_t src_row = static_cast<std::size_t>(input.cols) * 3;
  const std::size_t dst_row = src_row * sizeof(float);
  const std::size_t count =
      static_cast<std::size_t>(input.cols) * input.rows * 3;
  Buffer src(src_row * input.rows), dst(dst_row * input.rows);
  Check(cudaMemcpy2D(src.get(), src_row, input.data, input.step, src_row,
                     input.rows, cudaMemcpyHostToDevice),
        "Upload input");
  Check(cv_cuda_pipeline::NormalizeImageNet(
            {static_cast<const std::uint8_t*>(src.get()), src_row,
             static_cast<float*>(dst.get()), dst_row, input.cols, input.rows}),
        "Normalize");
  std::vector<float> normalized(count);
  Check(cudaMemcpy2D(normalized.data(), dst_row, dst.get(), dst_row, dst_row,
                     input.rows, cudaMemcpyDeviceToHost),
        "Download normalized tensor");
  const auto limits = std::minmax_element(normalized.begin(), normalized.end());
  const float range =
      *limits.second != *limits.first ? *limits.second - *limits.first : 1.f;
  const float scale = 255.f / range;
  cv::Mat output(input.rows, input.cols, CV_8UC3);
  for (int y = 0; y < input.rows; ++y) {
    auto* out = output.ptr<std::uint8_t>(y);
    const auto* in =
        normalized.data() + static_cast<std::size_t>(y) * input.cols * 3;
    for (int x = 0; x < input.cols * 3; ++x) {
      const float value = (in[x] - *limits.first) * scale;
      out[x] = static_cast<std::uint8_t>(
          std::clamp(static_cast<int>(value), 0, 255));
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
    std::cerr << "normalize_demo: " << e.what() << '\n';
    return 1;
  }
}
