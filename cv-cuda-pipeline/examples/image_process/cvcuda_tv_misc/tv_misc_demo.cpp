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

#include "cv-cuda-pipeline/calculators/image_process/cuda/gaussian_kernel.h"
#include "cv-cuda-pipeline/calculators/image_process/cuda/gaussiannoise_kernel.h"
#include "cv-cuda-pipeline/calculators/image_process/cuda/normalize_kernel.h"
#include "cv-cuda-pipeline/framework/port/opencv_core_inc.h"
#include "cv-cuda-pipeline/framework/port/opencv_imgcodecs_inc.h"

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
  float* get_float() const { return static_cast<float*>(data_); }

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
  std::string output = "cvcuda_tv_misc.png";
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
  const std::size_t row = static_cast<std::size_t>(input.cols) * 3;
  const std::size_t bytes = row * input.rows;
  const std::size_t float_row = row * sizeof(float);
  DeviceBuffer src(bytes), blurred(bytes), noisy(bytes),
      normalized(float_row * input.rows);
  Check(cudaMemcpy2D(src.get(), row, input.data, input.step, row, input.rows,
                     cudaMemcpyHostToDevice),
        "Upload input");
  cudaStream_t stream;
  Check(cudaStreamCreate(&stream), "cudaStreamCreate");
  Event start, stop;
  Check(cudaEventRecord(start.get(), stream), "Record start event");
  Check(cv_cuda_pipeline::NormalizeImageNet(
            {src.get(), row, normalized.get_float(), float_row, input.cols,
             input.rows},
            stream),
        "Normalize image for preprocessing");
  Check(
      cv_cuda_pipeline::GaussianBlur5x5(
          {src.get(), row, blurred.get(), row, input.cols, input.rows}, stream),
      "Apply 5x5 Gaussian blur");
  Check(cv_cuda_pipeline::AddGaussianNoise(
            {blurred.get(), row, noisy.get(), row, input.cols, input.rows, 3,
             0.0f, 0.03f * 255.0f, 42},
            stream),
        "Add Gaussian noise");
  Check(cudaEventRecord(stop.get(), stream), "Record stop event");
  Check(cudaEventSynchronize(stop.get()), "Wait for TV miscellaneous ops");
  float elapsed_ms = 0;
  Check(cudaEventElapsedTime(&elapsed_ms, start.get(), stop.get()),
        "Measure TV miscellaneous ops");
  Check(cudaStreamDestroy(stream), "Destroy stream");
  cv::Mat output(input.rows, input.cols, CV_8UC3);
  Check(cudaMemcpy2D(output.data, output.step, noisy.get(), row, row,
                     input.rows, cudaMemcpyDeviceToHost),
        "Download output");
  if (!cv::imwrite(options.output, output))
    throw std::runtime_error("Cannot write output: " + options.output);
  std::cout << "Saved 5x5 blur + seeded Gaussian noise to " << options.output
            << " (" << output.cols << "x" << output.rows
            << "), three-kernel latency " << elapsed_ms << " ms\n";
}
}  // namespace

int main(int argc, char** argv) {
  try {
    Run(Parse(argc, argv));
    return 0;
  } catch (const std::exception& e) {
    std::cerr << "tv_misc_demo: " << e.what() << '\n';
    return 1;
  }
}
