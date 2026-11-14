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

#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

#include "cv-cuda-pipeline/calculators/image_process/cuda/histogrameq_kernel.h"
#include "cv-cuda-pipeline/calculators/image_process/cuda/threshold_kernel.h"
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
  void* get() const { return data_; }

 private:
  void* data_ = nullptr;
};
struct Options {
  std::string input;
  std::string output = "cat_labeled.jpg";
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
cv::Mat ColorizeLabels(const cv::Mat& labels, int label_count) {
  std::vector<cv::Vec3b> colors(static_cast<std::size_t>(label_count));
  for (int label = 0; label < label_count; ++label) {
    std::mt19937 generator(static_cast<std::uint32_t>(label));
    std::uniform_int_distribution<int> byte(0, 255);
    colors[label] = cv::Vec3b(static_cast<std::uint8_t>(byte(generator)),
                              static_cast<std::uint8_t>(byte(generator)),
                              static_cast<std::uint8_t>(byte(generator)));
  }
  cv::Mat output(labels.rows, labels.cols, CV_8UC3);
  for (int y = 0; y < labels.rows; ++y) {
    const auto* label_row = labels.ptr<int>(y);
    auto* output_row = output.ptr<cv::Vec3b>(y);
    for (int x = 0; x < labels.cols; ++x) output_row[x] = colors[label_row[x]];
  }
  return output;
}
void Run(const Options& options) {
  const cv::Mat input = cv::imread(options.input, cv::IMREAD_COLOR);
  if (input.empty())
    throw std::runtime_error("Cannot read input: " + options.input);
  cv::Mat gray;
  cv::cvtColor(input, gray, cv::COLOR_BGR2GRAY);
  const std::size_t row = static_cast<std::size_t>(input.cols);
  const std::size_t image_bytes = row * input.rows;
  DeviceBuffer device_gray(image_bytes), equalized_device(image_bytes);
  DeviceBuffer histogram(256 * sizeof(std::uint32_t)), lookup_table(256);
  Check(cudaMemcpy2D(device_gray.get(), row, gray.data, gray.step, row,
                     gray.rows, cudaMemcpyHostToDevice),
        "Upload grayscale input");
  Check(cv_cuda_pipeline::HistogramEqualize(
            {static_cast<const std::uint8_t*>(device_gray.get()), row,
             static_cast<std::uint8_t*>(equalized_device.get()), row,
             static_cast<std::uint32_t*>(histogram.get()),
             static_cast<std::uint8_t*>(lookup_table.get()), input.cols,
             input.rows}),
        "Histogram equalization");
  cv::Mat equalized(input.rows, input.cols, CV_8UC1);
  Check(cudaMemcpy2D(equalized.data, equalized.step, equalized_device.get(),
                     row, row, input.rows, cudaMemcpyDeviceToHost),
        "Download equalized grayscale");

  cv::Mat equalized_bgr, thresholded(input.rows, input.cols, CV_8UC3);
  cv::cvtColor(equalized, equalized_bgr, cv::COLOR_GRAY2BGR);
  const std::size_t bgr_row = row * 3;
  const std::size_t bgr_bytes = bgr_row * input.rows;
  DeviceBuffer threshold_input(bgr_bytes), threshold_output(bgr_bytes);
  Check(cudaMemcpy2D(threshold_input.get(), bgr_row, equalized_bgr.data,
                     equalized_bgr.step, bgr_row, input.rows,
                     cudaMemcpyHostToDevice),
        "Upload threshold input");
  Check(cv_cuda_pipeline::Threshold(
            {static_cast<const std::uint8_t*>(threshold_input.get()), bgr_row,
             static_cast<std::uint8_t*>(threshold_output.get()), bgr_row,
             input.cols, input.rows}),
        "Binary threshold");
  Check(cudaMemcpy2D(thresholded.data, thresholded.step, threshold_output.get(),
                     bgr_row, bgr_row, input.rows, cudaMemcpyDeviceToHost),
        "Download threshold image");
  cv::Mat binary;
  cv::extractChannel(thresholded, binary, 0);

  cv::Mat labels;
  const auto start = std::chrono::steady_clock::now();
  const int label_count = cv::connectedComponents(binary, labels, 8, CV_32S);
  const auto stop = std::chrono::steady_clock::now();
  cv::Mat visualization = ColorizeLabels(labels, label_count);
  if (!cv::imwrite(options.output, visualization))
    throw std::runtime_error("Cannot write output: " + options.output);
  const auto elapsed =
      std::chrono::duration<double, std::milli>(stop - start).count();
  std::cout << "Saved " << label_count - 1 << " foreground components to "
            << options.output << "; CPU connected-components labeling took "
            << elapsed << " ms\n";
}
}  // namespace

int main(int argc, char** argv) {
  try {
    Run(Parse(argc, argv));
    return 0;
  } catch (const std::exception& e) {
    std::cerr << "label_demo: " << e.what() << '\n';
    return 1;
  }
}
