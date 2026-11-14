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

#include <cstdlib>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>

#include "cv-cuda-pipeline/calculators/image_process/cuda/histogrameq_kernel.h"
#include "cv-cuda-pipeline/framework/port/opencv_core_inc.h"
#include "cv-cuda-pipeline/framework/port/opencv_imgcodecs_inc.h"
#include "cv-cuda-pipeline/framework/port/opencv_imgproc_inc.h"

namespace {
void Check(cudaError_t status, const char* operation) {
  if (status != cudaSuccess)
    throw std::runtime_error(std::string(operation) + ": " +
                             cudaGetErrorString(status));
}
class DeviceBuffer {
 public:
  explicit DeviceBuffer(std::size_t bytes) {
    Check(cudaMalloc(&data_, bytes), "cudaMalloc");
  }
  ~DeviceBuffer() { cudaFree(data_); }
  DeviceBuffer(const DeviceBuffer&) = delete;
  DeviceBuffer& operator=(const DeviceBuffer&) = delete;
  std::uint8_t* get() const { return static_cast<std::uint8_t*>(data_); }
  std::uint32_t* get32() const { return static_cast<std::uint32_t*>(data_); }

 private:
  void* data_ = nullptr;
};
class Event {
 public:
  Event() { Check(cudaEventCreate(&event_), "cudaEventCreate"); }
  ~Event() { cudaEventDestroy(event_); }
  Event(const Event&) = delete;
  Event& operator=(const Event&) = delete;
  cudaEvent_t get() const { return event_; }

 private:
  cudaEvent_t event_ = nullptr;
};
struct Options {
  std::string input;
  std::string output = "histogrameq.png";
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
  const cv::Mat color = cv::imread(options.input, cv::IMREAD_COLOR);
  if (color.empty())
    throw std::runtime_error("Cannot read input: " + options.input);
  cv::Mat gray;
  cv::cvtColor(color, gray, cv::COLOR_BGR2GRAY);
  const auto row_bytes = static_cast<std::size_t>(gray.cols);
  DeviceBuffer src(row_bytes * gray.rows), dst(row_bytes * gray.rows),
      histogram(256 * sizeof(std::uint32_t)), lookup_table(256);
  Check(cudaMemcpy2D(src.get(), row_bytes, gray.data, gray.step, row_bytes,
                     gray.rows, cudaMemcpyHostToDevice),
        "Upload grayscale image");
  Event start, stop;
  Check(cudaEventRecord(start.get()), "Record start event");
  Check(cv_cuda_pipeline::HistogramEqualize(
            {src.get(), row_bytes, dst.get(), row_bytes, histogram.get32(),
             lookup_table.get(), gray.cols, gray.rows}),
        "Launch histogram equalization");
  Check(cudaEventRecord(stop.get()), "Record stop event");
  Check(cudaEventSynchronize(stop.get()), "Wait for histogram equalization");
  float elapsed_ms = 0;
  Check(cudaEventElapsedTime(&elapsed_ms, start.get(), stop.get()),
        "Measure histogram equalization");
  cv::Mat equalized_gray(gray.rows, gray.cols, CV_8UC1);
  Check(cudaMemcpy2D(equalized_gray.data, equalized_gray.step, dst.get(),
                     row_bytes, row_bytes, gray.rows, cudaMemcpyDeviceToHost),
        "Download equalized image");
  cv::Mat output;
  cv::cvtColor(equalized_gray, output, cv::COLOR_GRAY2BGR);
  if (!cv::imwrite(options.output, output))
    throw std::runtime_error("Cannot write output: " + options.output);
  std::cout << "Saved grayscale histogram equalization to " << options.output
            << " (" << output.cols << "x" << output.rows
            << "), three-kernel latency " << elapsed_ms << " ms\n";
}
}  // namespace

int main(int argc, char** argv) {
  try {
    Run(Parse(argc, argv));
    return 0;
  } catch (const std::exception& e) {
    std::cerr << "histogrameq_demo: " << e.what() << '\n';
    return 1;
  }
}
