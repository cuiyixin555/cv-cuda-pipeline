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
#include <cstdlib>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "cv-cuda-pipeline/calculators/image_process/cuda/resize_crop_convert_reformat_kernel.h"
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
  void* get() const { return data_; }

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
  std::string output = "resize_crop_convert_reformat.png";
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
  const std::size_t src_stride = static_cast<std::size_t>(input.cols) * 3;
  const std::size_t row_stride =
      static_cast<std::size_t>(options.width) * sizeof(float);
  const std::size_t plane_stride =
      row_stride * static_cast<std::size_t>(options.height);
  DeviceBuffer src(src_stride * input.rows), dst(plane_stride * 3);
  Check(cudaMemcpy2D(src.get(), src_stride, input.data, input.step, src_stride,
                     input.rows, cudaMemcpyHostToDevice),
        "Upload input");
  Event start, stop;
  Check(cudaEventRecord(start.get()), "Record start event");
  Check(
      cv_cuda_pipeline::ResizeCropConvertReformat(
          {static_cast<const std::uint8_t*>(src.get()), src_stride, input.cols,
           input.rows, static_cast<float*>(dst.get()), row_stride, plane_stride,
           options.width, options.height, 0, 0, options.width, options.height}),
      "Resize, crop, convert and reformat");
  Check(cudaEventRecord(stop.get()), "Record stop event");
  Check(cudaEventSynchronize(stop.get()), "Wait for fused transform");
  float elapsed_ms = 0;
  Check(cudaEventElapsedTime(&elapsed_ms, start.get(), stop.get()),
        "Measure fused transform");
  std::vector<float> nchw(plane_stride * 3 / sizeof(float));
  Check(cudaMemcpy(nchw.data(), dst.get(), plane_stride * 3,
                   cudaMemcpyDeviceToHost),
        "Download NCHW result");
  cv::Mat output(options.height, options.width, CV_8UC3);
  const std::size_t pixels =
      static_cast<std::size_t>(options.width) * options.height;
  for (int y = 0; y < options.height; ++y)
    for (int x = 0; x < options.width; ++x)
      for (int c = 0; c < 3; ++c) {
        const float value =
            nchw[(2 - c) * pixels +
                 static_cast<std::size_t>(y) * options.width + x];
        output.at<cv::Vec3b>(y, x)[c] =
            static_cast<std::uint8_t>(std::clamp(value, 0.0f, 255.0f));
      }
  if (!cv::imwrite(options.output, output))
    throw std::runtime_error("Cannot write output: " + options.output);
  std::cout << "Saved resize/crop/convert/reformat output to " << options.output
            << " (" << output.cols << "x" << output.rows << "), kernel latency "
            << elapsed_ms << " ms\n";
}
}  // namespace

int main(int argc, char** argv) {
  try {
    Run(Parse(argc, argv));
    return 0;
  } catch (const std::exception& e) {
    std::cerr << "resize_crop_convert_reformat_demo: " << e.what() << '\n';
    return 1;
  }
}
