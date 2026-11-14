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

#include <cmath>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>

#include "cv-cuda-pipeline/calculators/image_process/cuda/resize_kernel.h"
#include "cv-cuda-pipeline/calculators/image_process/cuda/warp_kernel.h"
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
  std::string output = "cvcuda_tv_geometry.png";
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
cv_cuda_pipeline::WarpTransform Rotation(int width, int height) {
  constexpr float kPi = 3.14159265358979323846f;
  const float angle = 15.0f * kPi / 180.0f;
  const float cosine = std::cos(angle), sine = std::sin(angle);
  const float cx = width / 2.0f, cy = height / 2.0f;
  return {{cosine, sine, (1.0f - cosine) * cx - sine * cy, -sine, cosine,
           sine * cx + (1.0f - cosine) * cy, 0.0f, 0.0f, 1.0f}};
}
void Run(const Options& options) {
  const cv::Mat input = cv::imread(options.input, cv::IMREAD_COLOR);
  if (input.empty())
    throw std::runtime_error("Cannot read input: " + options.input);
  const std::size_t input_row =
      static_cast<std::size_t>(input.cols) * input.elemSize();
  const std::size_t output_row = static_cast<std::size_t>(options.width) * 3;
  const std::size_t output_bytes = output_row * options.height;
  DeviceBuffer src(input_row * input.rows), resized(output_bytes),
      rotated(output_bytes);
  Check(cudaMemcpy2D(src.get(), input_row, input.data, input.step, input_row,
                     input.rows, cudaMemcpyHostToDevice),
        "Upload input");
  Event start, stop;
  Check(cudaEventRecord(start.get()), "Record start event");
  Check(
      cv_cuda_pipeline::ResizeLinear({src.get(), input.cols, input.rows,
                                      input_row, input_row * input.rows},
                                     {resized.get(), options.width,
                                      options.height, output_row, output_bytes},
                                     1),
      "Resize image");
  Check(cv_cuda_pipeline::WarpLinear(
            {resized.get(), output_row, rotated.get(), output_row,
             options.width, options.height, 3},
            Rotation(options.width, options.height)),
        "Rotate image");
  Check(cudaEventRecord(stop.get()), "Record stop event");
  Check(cudaEventSynchronize(stop.get()), "Wait for geometry transforms");
  float elapsed_ms = 0;
  Check(cudaEventElapsedTime(&elapsed_ms, start.get(), stop.get()),
        "Measure geometry transforms");
  cv::Mat output(options.height, options.width, CV_8UC3);
  Check(cudaMemcpy2D(output.data, output.step, rotated.get(), output_row,
                     output_row, options.height, cudaMemcpyDeviceToHost),
        "Download output");
  if (!cv::imwrite(options.output, output))
    throw std::runtime_error("Cannot write output: " + options.output);
  std::cout << "Saved resize + 15-degree rotation to " << options.output << " ("
            << output.cols << "x" << output.rows << "), two-kernel latency "
            << elapsed_ms << " ms\n";
}
}  // namespace

int main(int argc, char** argv) {
  try {
    Run(Parse(argc, argv));
    return 0;
  } catch (const std::exception& e) {
    std::cerr << "tv_geometry_demo: " << e.what() << '\n';
    return 1;
  }
}
