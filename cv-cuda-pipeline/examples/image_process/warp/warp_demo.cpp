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

#include <array>
#include <cmath>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>

#include "cv-cuda-pipeline/calculators/image_process/cuda/warp_kernel.h"
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
  std::string output;
  std::string mode = "affine";
};
Options Parse(int argc, char** argv) {
  Options options;
  for (int i = 1; i < argc; ++i) {
    const std::string arg = argv[i];
    if (arg == "--help" || arg == "-h") {
      std::cout
          << "Usage: " << argv[0]
          << " --input IMAGE [--mode affine|perspective] [--output IMAGE]\n";
      std::exit(0);
    }
    if (arg != "--input" && arg != "--output" && arg != "--mode")
      throw std::invalid_argument("Unknown option: " + arg);
    if (++i == argc) throw std::invalid_argument("Missing value for " + arg);
    if (arg == "--input")
      options.input = argv[i];
    else if (arg == "--output")
      options.output = argv[i];
    else
      options.mode = argv[i];
  }
  if (options.input.empty())
    throw std::invalid_argument("--input must not be empty");
  if (options.mode != "affine" && options.mode != "perspective")
    throw std::invalid_argument("--mode must be affine or perspective");
  if (options.output.empty()) options.output = "warp_" + options.mode + ".png";
  return options;
}

cv_cuda_pipeline::WarpTransform AffineTransform(int width, int height) {
  constexpr float kPi = 3.14159265358979323846f;
  const float angle = 15.0f * kPi / 180.0f;
  const float cosine = std::cos(angle), sine = std::sin(angle);
  const float cx = width / 2.0f, cy = height / 2.0f;
  return {{cosine, sine, (1.0f - cosine) * cx - sine * cy + 20.0f, -sine,
           cosine, sine * cx + (1.0f - cosine) * cy, 0.0f, 0.0f, 1.0f}};
}

cv_cuda_pipeline::WarpTransform PerspectiveTransform(int width, int height) {
  const std::array<cv::Point2f, 4> source = {
      cv::Point2f(0.0f, 0.0f), cv::Point2f(static_cast<float>(width), 0.0f),
      cv::Point2f(static_cast<float>(width), static_cast<float>(height)),
      cv::Point2f(0.0f, static_cast<float>(height))};
  const std::array<cv::Point2f, 4> destination = {
      cv::Point2f(width * 0.1f, height * 0.05f),
      cv::Point2f(width * 0.9f, height * 0.1f),
      cv::Point2f(width * 0.85f, height * 0.95f),
      cv::Point2f(width * 0.15f, height * 0.9f)};
  const cv::Mat matrix =
      cv::getPerspectiveTransform(destination.data(), source.data());
  cv_cuda_pipeline::WarpTransform transform{};
  for (int row = 0; row < 3; ++row)
    for (int column = 0; column < 3; ++column)
      transform.values[row * 3 + column] =
          static_cast<float>(matrix.at<double>(row, column));
  return transform;
}

void Run(const Options& options) {
  const cv::Mat input = cv::imread(options.input, cv::IMREAD_COLOR);
  if (input.empty())
    throw std::runtime_error("Cannot read input: " + options.input);
  const auto row_bytes =
      static_cast<std::size_t>(input.cols) * input.elemSize();
  DeviceBuffer src(row_bytes * input.rows), dst(row_bytes * input.rows);
  Check(cudaMemcpy2D(src.get(), row_bytes, input.data, input.step, row_bytes,
                     input.rows, cudaMemcpyHostToDevice),
        "Upload input");
  const auto transform = options.mode == "affine"
                             ? AffineTransform(input.cols, input.rows)
                             : PerspectiveTransform(input.cols, input.rows);
  Event start, stop;
  Check(cudaEventRecord(start.get()), "Record start event");
  Check(cv_cuda_pipeline::WarpLinear({src.get(), row_bytes, dst.get(),
                                      row_bytes, input.cols, input.rows, 3},
                                     transform),
        "Launch warp kernel");
  Check(cudaEventRecord(stop.get()), "Record stop event");
  Check(cudaEventSynchronize(stop.get()), "Wait for warp kernel");
  float elapsed_ms = 0;
  Check(cudaEventElapsedTime(&elapsed_ms, start.get(), stop.get()),
        "Measure warp kernel");
  cv::Mat output(input.rows, input.cols, CV_8UC3);
  Check(cudaMemcpy2D(output.data, output.step, dst.get(), row_bytes, row_bytes,
                     input.rows, cudaMemcpyDeviceToHost),
        "Download output");
  if (!cv::imwrite(options.output, output))
    throw std::runtime_error("Cannot write output: " + options.output);
  std::cout << "Saved " << options.mode << " warp to " << options.output << " ("
            << output.cols << "x" << output.rows << "), kernel latency "
            << elapsed_ms << " ms\n";
}
}  // namespace

int main(int argc, char** argv) {
  try {
    Run(Parse(argc, argv));
    return 0;
  } catch (const std::exception& e) {
    std::cerr << "warp_demo: " << e.what() << '\n';
    return 1;
  }
}
