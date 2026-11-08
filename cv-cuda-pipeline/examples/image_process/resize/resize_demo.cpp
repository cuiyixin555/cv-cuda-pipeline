// MIT License

// Copyright (c) 2026 CUI Xin (崔 欣)

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <cuda_runtime.h>

#include <charconv>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>
#include <system_error>

#include "cv-cuda-pipeline/calculators/image_process/cuda/resize_kernel.h"
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
    CheckCuda(cudaMalloc(reinterpret_cast<void**>(&data_), size), "cudaMalloc");
  }
  ~DeviceBuffer() { cudaFree(data_); }
  DeviceBuffer(const DeviceBuffer&) = delete;
  DeviceBuffer& operator=(const DeviceBuffer&) = delete;
  std::uint8_t* get() const { return data_; }

 private:
  std::uint8_t* data_ = nullptr;
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
  std::string output = "cat_resized.jpg";
  int width = 224;
  int height = 224;
};

int ParsePositiveInt(const std::string& value, const std::string& option) {
  int result = 0;
  const auto parsed =
      std::from_chars(value.data(), value.data() + value.size(), result);
  if (parsed.ec != std::errc() || parsed.ptr != value.data() + value.size() ||
      result <= 0) {
    throw std::invalid_argument(option + " must be a positive integer");
  }
  return result;
}

Options ParseOptions(int argc, char** argv) {
  Options options;
  for (int i = 1; i < argc; ++i) {
    const std::string arg = argv[i];
    if (arg == "--help" || arg == "-h") {
      std::cout << "Usage: " << argv[0]
                << " --input IMAGE [--output IMAGE] [--width N] [--height N]\n";
      std::exit(0);
    }
    if (arg != "--input" && arg != "--output" && arg != "--width" &&
        arg != "--height") {
      throw std::invalid_argument("Unknown option: " + arg);
    }
    if (++i == argc) {
      throw std::invalid_argument("Missing value for " + arg);
    }
    const std::string value = argv[i];
    if (arg == "--input") options.input = value;
    if (arg == "--output") options.output = value;
    if (arg == "--width") options.width = ParsePositiveInt(value, arg);
    if (arg == "--height") options.height = ParsePositiveInt(value, arg);
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
  cv::Mat rgb;
  cv::cvtColor(bgr, rgb, cv::COLOR_BGR2RGB);
  cv::Mat output(options.height, options.width, CV_8UC3);
  cv::Mat batch_output(options.height, options.width, CV_8UC3);
  const std::size_t input_row = static_cast<std::size_t>(rgb.cols) * 3;
  const std::size_t output_row = static_cast<std::size_t>(output.cols) * 3;
  const std::size_t input_bytes = input_row * rgb.rows;
  const std::size_t output_bytes = output_row * output.rows;
  DeviceBuffer device_input(input_bytes);
  DeviceBuffer device_output(output_bytes);
  DeviceBuffer device_batch_output(output_bytes);
  Stream stream;

  CheckCuda(cudaMemcpy2DAsync(device_input.get(), input_row, rgb.data, rgb.step,
                              input_row, rgb.rows, cudaMemcpyHostToDevice,
                              stream.get()),
            "Upload input");
  const cv_cuda_pipeline::ResizeConstImageView src{
      device_input.get(), rgb.cols, rgb.rows, input_row, input_bytes};
  const cv_cuda_pipeline::ResizeImageView dst{
      device_output.get(), output.cols, output.rows, output_row, output_bytes};
  const cv_cuda_pipeline::ResizeImageView batch_dst{device_batch_output.get(),
                                                    output.cols, output.rows,
                                                    output_row, output_bytes};
  CheckCuda(cv_cuda_pipeline::ResizeLinear(src, dst, 1, stream.get()),
            "Resize HWC");
  CheckCuda(cv_cuda_pipeline::ResizeLinear(src, batch_dst, 1, stream.get()),
            "Resize NHWC");
  CheckCuda(cudaMemcpy2DAsync(output.data, output.step, device_output.get(),
                              output_row, output_row, output.rows,
                              cudaMemcpyDeviceToHost, stream.get()),
            "Download HWC");
  CheckCuda(
      cudaMemcpy2DAsync(batch_output.data, batch_output.step,
                        device_batch_output.get(), output_row, output_row,
                        output.rows, cudaMemcpyDeviceToHost, stream.get()),
      "Download NHWC");
  CheckCuda(cudaStreamSynchronize(stream.get()), "Synchronize resize");
  if (cv::norm(output, batch_output, cv::NORM_INF) != 0) {
    throw std::runtime_error("HWC and NHWC resize results differ");
  }
  cv::Mat output_bgr;
  cv::cvtColor(output, output_bgr, cv::COLOR_RGB2BGR);
  if (!cv::imwrite(options.output, output_bgr)) {
    throw std::runtime_error("Cannot write output image: " + options.output);
  }
  const bool aspect_ratio_preserved =
      static_cast<std::int64_t>(bgr.cols) * options.height ==
      static_cast<std::int64_t>(bgr.rows) * options.width;
  std::cout << "Input: " << bgr.cols << "x" << bgr.rows
            << "; output: " << options.width << "x" << options.height
            << "; aspect ratio: "
            << (aspect_ratio_preserved ? "preserved" : "changed") << '\n'
            << "Saved image to " << options.output << "; NHWC shape: (1, "
            << options.height << ", " << options.width << ", 3)\n";
}

}  // namespace

int main(int argc, char** argv) {
  try {
    Run(ParseOptions(argc, argv));
    return 0;
  } catch (const std::exception& error) {
    std::cerr << "resize_demo: " << error.what() << '\n';
    return 1;
  }
}