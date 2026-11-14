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

#include "cv-cuda-pipeline/examples/image_process/quality_resize_common/quality_resize_demo.h"

#include <cuda_runtime.h>

#include <charconv>
#include <cstddef>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>
#include <system_error>

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
struct Options {
  std::string input;
  std::string output;
  int width = 512;
  int height = 512;
};
int ParsePositive(const std::string& value, const std::string& option) {
  int result = 0;
  const auto parsed =
      std::from_chars(value.data(), value.data() + value.size(), result);
  if (parsed.ec != std::errc() || parsed.ptr != value.data() + value.size() ||
      result <= 0)
    throw std::invalid_argument(option + " must be a positive integer");
  return result;
}
Options Parse(int argc, char** argv, const char* default_output) {
  Options options;
  options.output = default_output;
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
    const std::string value = argv[i];
    if (arg == "--input")
      options.input = value;
    else if (arg == "--output")
      options.output = value;
    else if (arg == "--width")
      options.width = ParsePositive(value, arg);
    else
      options.height = ParsePositive(value, arg);
  }
  if (options.input.empty() || options.output.empty())
    throw std::invalid_argument("--input and --output must not be empty");
  return options;
}
}  // namespace

int RunQualityResizeDemo(int argc, char** argv,
                         cv_cuda_pipeline::QualityResizeMode mode,
                         const char* default_output) {
  try {
    const Options options = Parse(argc, argv, default_output);
    const cv::Mat input = cv::imread(options.input, cv::IMREAD_COLOR);
    if (input.empty())
      throw std::runtime_error("Cannot read input: " + options.input);

    const std::size_t src_stride = input.step;
    const std::size_t dst_stride = static_cast<std::size_t>(options.width) * 3;
    const std::size_t intermediate_stride = dst_stride * sizeof(float);
    const std::size_t intermediate_bytes = intermediate_stride * input.rows;
    const std::size_t dst_bytes = dst_stride * options.height;
    DeviceBuffer src(src_stride * input.rows), dst(dst_bytes),
        intermediate(intermediate_bytes);
    Check(cudaMemcpy2D(src.get(), src_stride, input.data, input.step,
                       static_cast<std::size_t>(input.cols) * 3, input.rows,
                       cudaMemcpyHostToDevice),
          "Upload input");
    cv::Mat output(options.height, options.width, CV_8UC3);
    const cv_cuda_pipeline::QualityResizeImageView image{
        static_cast<const std::uint8_t*>(src.get()),
        src_stride,
        static_cast<float*>(intermediate.get()),
        intermediate_stride,
        static_cast<std::uint8_t*>(dst.get()),
        dst_stride,
        input.cols,
        input.rows,
        options.width,
        options.height};
    Check(cv_cuda_pipeline::QualityResize(image, mode), "Quality resize");
    Check(cudaMemcpy2D(output.data, output.step, dst.get(), dst_stride,
                       dst_stride, options.height, cudaMemcpyDeviceToHost),
          "Download output");
    if (!cv::imwrite(options.output, output))
      throw std::runtime_error("Cannot write output: " + options.output);
    std::cout << "Saved " << options.width << "x" << options.height
              << " quality-resized image to " << options.output << '\n';
    return 0;
  } catch (const std::exception& e) {
    std::cerr << "quality_resize_demo: " << e.what() << '\n';
    return 1;
  }
}
