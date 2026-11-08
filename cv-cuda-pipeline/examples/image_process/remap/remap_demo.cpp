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

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "cv-cuda-pipeline/calculators/image_process/cuda/remap_kernel.h"
#include "cv-cuda-pipeline/framework/port/opencv_core_inc.h"
#include "cv-cuda-pipeline/framework/port/opencv_imgcodecs_inc.h"
#include "cv-cuda-pipeline/framework/port/opencv_imgproc_inc.h"

namespace {

void CheckCuda(cudaError_t status, const char* operation) {
  if (status != cudaSuccess) {
    throw std::runtime_error(std::string(operation) + ": " +
                             cudaGetErrorString(status));
  }
}

class DeviceBuffer {
 public:
  explicit DeviceBuffer(std::size_t size) {
    CheckCuda(cudaMalloc(&data_, size), "cudaMalloc");
  }
  ~DeviceBuffer() { cudaFree(data_); }
  DeviceBuffer(const DeviceBuffer&) = delete;
  DeviceBuffer& operator=(const DeviceBuffer&) = delete;
  void* get() const { return data_; }

 private:
  void* data_ = nullptr;
};

class Stream {
 public:
  Stream() { CheckCuda(cudaStreamCreate(&stream_), "cudaStreamCreate"); }
  ~Stream() { cudaStreamDestroy(stream_); }
  Stream(const Stream&) = delete;
  Stream& operator=(const Stream&) = delete;
  cudaStream_t get() const { return stream_; }

 private:
  cudaStream_t stream_ = nullptr;
};

struct Options {
  std::string input;
  std::string output = "cat_remap.png";
};

Options ParseOptions(int argc, char** argv) {
  Options options;
  for (int i = 1; i < argc; ++i) {
    const std::string arg = argv[i];
    if (arg == "--help" || arg == "-h") {
      std::cout << "Usage: " << argv[0] << " --input IMAGE [--output IMAGE]\n";
      std::exit(0);
    }
    if (arg != "--input" && arg != "--output") {
      throw std::invalid_argument("Unknown option: " + arg);
    }
    if (++i == argc) {
      throw std::invalid_argument("Missing value for " + arg);
    }
    if (arg == "--input") options.input = argv[i];
    if (arg == "--output") options.output = argv[i];
  }
  if (options.input.empty() || options.output.empty()) {
    throw std::invalid_argument("--input and --output must not be empty");
  }
  return options;
}

void Run(const Options& options) {
  const cv::Mat bgr = cv::imread(options.input, cv::IMREAD_COLOR);
  if (bgr.empty()) {
    throw std::runtime_error("Cannot read input image: " + options.input);
  }
  if (bgr.cols > (1 << 23) || bgr.rows > (1 << 23)) {
    throw std::runtime_error("Image dimensions exceed the remap limit");
  }
  cv::Mat rgb;
  cv::cvtColor(bgr, rgb, cv::COLOR_BGR2RGB);
  cv::Mat output(rgb.rows, rgb.cols, CV_8UC3);
  const std::size_t row_bytes = static_cast<std::size_t>(rgb.cols) * 3;
  const std::size_t image_bytes = row_bytes * rgb.rows;
  const std::size_t map_row_bytes =
      static_cast<std::size_t>(rgb.cols) * sizeof(float2);
  std::vector<float2> map(static_cast<std::size_t>(rgb.cols) * rgb.rows);
  constexpr float kTwoPi = 6.2831853071795864769f;
  const float amplitude = rgb.rows * 0.04f;
  const float freq_x = kTwoPi * 3.f / rgb.cols;
  const float freq_y = kTwoPi * 3.f / rgb.rows;
  for (int y = 0; y < rgb.rows; ++y) {
    for (int x = 0; x < rgb.cols; ++x) {
      map[static_cast<std::size_t>(y) * rgb.cols + x] =
          make_float2(x + amplitude * std::sin(freq_y * y),
                      y + amplitude * std::sin(freq_x * x));
    }
  }

  DeviceBuffer src(image_bytes);
  DeviceBuffer dst(image_bytes);
  DeviceBuffer device_map(map.size() * sizeof(float2));
  Stream stream;
  CheckCuda(
      cudaMemcpy2DAsync(src.get(), row_bytes, rgb.data, rgb.step, row_bytes,
                        rgb.rows, cudaMemcpyHostToDevice, stream.get()),
      "Upload input");
  CheckCuda(
      cudaMemcpyAsync(device_map.get(), map.data(), map.size() * sizeof(float2),
                      cudaMemcpyHostToDevice, stream.get()),
      "Upload map");
  const cv_cuda_pipeline::RemapImageView view{
      static_cast<const std::uint8_t*>(src.get()),
      row_bytes,
      static_cast<std::uint8_t*>(dst.get()),
      row_bytes,
      static_cast<const float2*>(device_map.get()),
      map_row_bytes,
      rgb.cols,
      rgb.rows};
  CheckCuda(cv_cuda_pipeline::RemapLinear(view, stream.get()), "Remap");
  CheckCuda(cudaMemcpy2DAsync(output.data, output.step, dst.get(), row_bytes,
                              row_bytes, output.rows, cudaMemcpyDeviceToHost,
                              stream.get()),
            "Download output");
  CheckCuda(cudaStreamSynchronize(stream.get()), "Synchronize remap");
  cv::Mat output_bgr;
  cv::cvtColor(output, output_bgr, cv::COLOR_RGB2BGR);
  if (!cv::imwrite(options.output, output_bgr)) {
    throw std::runtime_error("Cannot write output image: " + options.output);
  }
  std::cout << "Saved image to " << options.output << " (" << rgb.cols << "x"
            << rgb.rows << ")\n";
}

}  // namespace

int main(int argc, char** argv) {
  try {
    Run(ParseOptions(argc, argv));
    return 0;
  } catch (const std::exception& error) {
    std::cerr << "remap_demo: " << error.what() << '\n';
    return 1;
  }
}
