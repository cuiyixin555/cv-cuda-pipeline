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
#include <cmath>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>

#include "cv-cuda-pipeline/calculators/image_process/cuda/color_twist_kernel.h"
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
  std::string output = "cvcuda_tv_color.png";
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

cv_cuda_pipeline::ColorTwistMatrix Scale(float factor) {
  cv_cuda_pipeline::ColorTwistMatrix matrix{};
  for (int c = 0; c < 3; ++c) matrix.rows[c][c] = factor;
  return matrix;
}
cv_cuda_pipeline::ColorTwistMatrix Contrast(float mean) {
  auto matrix = Scale(1.2f);
  for (int c = 0; c < 3; ++c) matrix.rows[c][3] = -0.2f * mean;
  return matrix;
}
cv_cuda_pipeline::ColorTwistMatrix Saturation() {
  constexpr float kGrayB = 0.114f, kGrayG = 0.587f, kGrayR = 0.299f;
  constexpr float kSaturation = 0.8f;
  constexpr float kGrayWeight = 1.0f - kSaturation;
  cv_cuda_pipeline::ColorTwistMatrix matrix{};
  matrix.rows[0][0] = kSaturation + kGrayWeight * kGrayB;
  matrix.rows[0][1] = kGrayWeight * kGrayG;
  matrix.rows[0][2] = kGrayWeight * kGrayR;
  matrix.rows[1][0] = kGrayWeight * kGrayB;
  matrix.rows[1][1] = kSaturation + kGrayWeight * kGrayG;
  matrix.rows[1][2] = kGrayWeight * kGrayR;
  matrix.rows[2][0] = kGrayWeight * kGrayB;
  matrix.rows[2][1] = kGrayWeight * kGrayG;
  matrix.rows[2][2] = kSaturation + kGrayWeight * kGrayR;
  return matrix;
}
float BrightnessGrayscaleMean(const cv::Mat& input) {
  double sum = 0.0;
  for (int y = 0; y < input.rows; ++y) {
    const auto* row = input.ptr<cv::Vec3b>(y);
    for (int x = 0; x < input.cols; ++x) {
      const float b = std::nearbyint(std::min(255.0f, row[x][0] * 1.15f));
      const float g = std::nearbyint(std::min(255.0f, row[x][1] * 1.15f));
      const float r = std::nearbyint(std::min(255.0f, row[x][2] * 1.15f));
      sum += 0.114 * b + 0.587 * g + 0.299 * r;
    }
  }
  return static_cast<float>(sum /
                            (static_cast<double>(input.rows) * input.cols));
}

void Run(const Options& options) {
  const cv::Mat input = cv::imread(options.input, cv::IMREAD_COLOR);
  if (input.empty())
    throw std::runtime_error("Cannot read input: " + options.input);
  const std::size_t row_bytes =
      static_cast<std::size_t>(input.cols) * input.elemSize();
  const std::size_t bytes = row_bytes * input.rows;
  DeviceBuffer src(bytes), brightness(bytes), contrast(bytes), output(bytes);
  Check(cudaMemcpy2D(src.get(), row_bytes, input.data, input.step, row_bytes,
                     input.rows, cudaMemcpyHostToDevice),
        "Upload input");
  const auto image = [&](const std::uint8_t* in, std::uint8_t* out) {
    return cv_cuda_pipeline::ColorTwistImageView{
        in, row_bytes, out, row_bytes, input.cols, input.rows};
  };
  const float grayscale_mean = BrightnessGrayscaleMean(input);
  Event start, stop;
  Check(cudaEventRecord(start.get()), "Record start event");
  Check(cv_cuda_pipeline::ColorTwist(image(src.get(), brightness.get()),
                                     Scale(1.15f)),
        "Adjust brightness");
  Check(cv_cuda_pipeline::ColorTwist(image(brightness.get(), contrast.get()),
                                     Contrast(grayscale_mean)),
        "Adjust contrast");
  Check(cv_cuda_pipeline::ColorTwist(image(contrast.get(), output.get()),
                                     Saturation()),
        "Adjust saturation");
  Check(cudaEventRecord(stop.get()), "Record stop event");
  Check(cudaEventSynchronize(stop.get()), "Wait for color adjustments");
  float elapsed_ms = 0;
  Check(cudaEventElapsedTime(&elapsed_ms, start.get(), stop.get()),
        "Measure color adjustments");
  cv::Mat result(input.rows, input.cols, CV_8UC3);
  Check(cudaMemcpy2D(result.data, result.step, output.get(), row_bytes,
                     row_bytes, input.rows, cudaMemcpyDeviceToHost),
        "Download output");
  if (!cv::imwrite(options.output, result))
    throw std::runtime_error("Cannot write output: " + options.output);
  std::cout << "Saved brightness/contrast/saturation adjustment to "
            << options.output << " (" << result.cols << "x" << result.rows
            << "), three-kernel latency " << elapsed_ms << " ms\n";
}
}  // namespace

int main(int argc, char** argv) {
  try {
    Run(Parse(argc, argv));
    return 0;
  } catch (const std::exception& e) {
    std::cerr << "tv_color_demo: " << e.what() << '\n';
    return 1;
  }
}
