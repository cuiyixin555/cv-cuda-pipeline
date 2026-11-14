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

#include "cv-cuda-pipeline/calculators/image_process/cuda/random_resized_crop.h"
#include "cv-cuda-pipeline/calculators/image_process/cuda/resize_kernel.h"
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
  std::string output = "random_resized_crop.png";
  int width = 224;
  int height = 224;
};
Options Parse(int argc, char** argv) {
  Options options;
  for (int i = 1; i < argc; ++i) {
    const std::string arg = argv[i];
    if (arg == "--help" || arg == "-h") {
      std::cout << "Usage: " << argv[0]
                << " --input IMAGE [--output IMAGE] [--width N] [--height N]\n";
      std::exit(0);
    }
    if (arg != "--input" && arg != "--output" && arg != "--width" &&
        arg != "--height")
      throw std::invalid_argument("Unknown option: " + arg);
    if (++i == argc) throw std::invalid_argument("Missing value for " + arg);
    if (arg == "--input")
      options.input = argv[i];
    else if (arg == "--output")
      options.output = argv[i];
    else if (arg == "--width")
      options.width = std::stoi(argv[i]);
    else
      options.height = std::stoi(argv[i]);
  }
  if (options.input.empty() || options.output.empty() || options.width <= 0 ||
      options.height <= 0)
    throw std::invalid_argument("Input/output and positive size are required");
  return options;
}
void Run(const Options& options) {
  const cv::Mat input = cv::imread(options.input, cv::IMREAD_COLOR);
  if (input.empty())
    throw std::runtime_error("Cannot read input: " + options.input);
  cv_cuda_pipeline::CropRectangle crop{};
  if (!cv_cuda_pipeline::SelectRandomResizedCrop(
          input.cols, input.rows, 0.08f, 1.0f, 0.75f, 1.3333333f, 42, &crop))
    throw std::runtime_error("Could not select a random crop");

  const std::size_t input_stride = input.step;
  const std::size_t output_stride = static_cast<std::size_t>(options.width) * 3;
  DeviceBuffer src(input_stride * input.rows),
      dst(output_stride * options.height);
  Check(cudaMemcpy2D(src.get(), input_stride, input.data, input.step,
                     static_cast<std::size_t>(input.cols) * 3, input.rows,
                     cudaMemcpyHostToDevice),
        "Upload input");
  const auto* crop_src = src.get() +
                         static_cast<std::size_t>(crop.y) * input_stride +
                         static_cast<std::size_t>(crop.x) * 3;
  const std::size_t crop_batch_stride =
      static_cast<std::size_t>(crop.height - 1) * input_stride +
      static_cast<std::size_t>(crop.width) * 3;
  cv_cuda_pipeline::ResizeConstImageView source{
      crop_src, crop.width, crop.height, input_stride, crop_batch_stride};
  cv_cuda_pipeline::ResizeImageView destination{dst.get(), options.width,
                                                options.height, output_stride,
                                                output_stride * options.height};
  Event start, stop;
  Check(cudaEventRecord(start.get()), "Record start event");
  Check(cv_cuda_pipeline::ResizeLinear(source, destination, 1),
        "Resize random crop");
  Check(cudaEventRecord(stop.get()), "Record stop event");
  Check(cudaEventSynchronize(stop.get()), "Wait for random resized crop");
  float elapsed_ms = 0;
  Check(cudaEventElapsedTime(&elapsed_ms, start.get(), stop.get()),
        "Measure random resized crop");
  cv::Mat output(options.height, options.width, CV_8UC3);
  Check(cudaMemcpy2D(output.data, output.step, dst.get(), output_stride,
                     output_stride, options.height, cudaMemcpyDeviceToHost),
        "Download output");
  if (!cv::imwrite(options.output, output))
    throw std::runtime_error("Cannot write output: " + options.output);
  std::cout << "Crop (" << crop.x << ", " << crop.y << ", " << crop.width << "x"
            << crop.height << ") resized to " << options.width << "x"
            << options.height << " at " << options.output << ", kernel latency "
            << elapsed_ms << " ms\n";
}
}  // namespace

int main(int argc, char** argv) {
  try {
    Run(Parse(argc, argv));
    return 0;
  } catch (const std::exception& e) {
    std::cerr << "random_resized_crop_demo: " << e.what() << '\n';
    return 1;
  }
}
