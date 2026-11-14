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

#include <cstdint>
#include <cstdlib>
#include <exception>
#include <filesystem>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>

#include "cv-cuda-pipeline/calculators/image_process/cuda/inpaint_kernel.h"
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
  std::string output = "cat_inpaint.jpg";
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
  const cv::Mat original = cv::imread(options.input, cv::IMREAD_COLOR);
  if (original.empty())
    throw std::runtime_error("Cannot read input: " + options.input);

  cv::Mat mask(original.rows, original.cols, CV_8UC1, cv::Scalar(0));
  cv::Mat damaged = original.clone();
  std::mt19937 generator(42);
  std::bernoulli_distribution corrupt(0.15);
  std::size_t damaged_pixels = 0;
  for (int y = 0; y < original.rows; ++y) {
    auto* mask_row = mask.ptr<std::uint8_t>(y);
    auto* image_row = damaged.ptr<cv::Vec3b>(y);
    for (int x = 0; x < original.cols; ++x) {
      if (!corrupt(generator)) continue;
      mask_row[x] = 255;
      image_row[x] = cv::Vec3b(0, 0, 0);
      ++damaged_pixels;
    }
  }

  const std::filesystem::path output_path(options.output);
  const std::filesystem::path damaged_path =
      output_path.parent_path() / (output_path.stem().string() + "_damaged" +
                                   output_path.extension().string());
  if (!cv::imwrite(damaged_path.string(), damaged))
    throw std::runtime_error("Cannot write damaged image: " +
                             damaged_path.string());

  cv::Mat inpainted(original.rows, original.cols, CV_8UC3);
  const std::size_t image_stride = damaged.step;
  const std::size_t mask_stride = mask.step;
  DeviceBuffer device_src(image_stride * damaged.rows);
  DeviceBuffer device_mask(mask_stride * mask.rows);
  DeviceBuffer device_dst(image_stride * damaged.rows);
  Check(cudaMemcpy2D(device_src.get(), image_stride, damaged.data, damaged.step,
                     static_cast<std::size_t>(damaged.cols) * 3, damaged.rows,
                     cudaMemcpyHostToDevice),
        "Upload damaged image");
  Check(cudaMemcpy2D(device_mask.get(), mask_stride, mask.data, mask.step,
                     mask.cols, mask.rows, cudaMemcpyHostToDevice),
        "Upload mask");
  Event start, stop;
  Check(cudaEventRecord(start.get()), "Record start event");
  Check(cv_cuda_pipeline::Inpaint(
            {static_cast<const std::uint8_t*>(device_src.get()), image_stride,
             static_cast<const std::uint8_t*>(device_mask.get()), mask_stride,
             static_cast<std::uint8_t*>(device_dst.get()), image_stride,
             original.cols, original.rows}),
        "Inpaint kernel");
  Check(cudaEventRecord(stop.get()), "Record stop event");
  Check(cudaEventSynchronize(stop.get()), "Synchronize inpaint kernel");
  float elapsed = 0.0f;
  Check(cudaEventElapsedTime(&elapsed, start.get(), stop.get()),
        "Measure inpaint kernel");
  Check(cudaMemcpy2D(inpainted.data, inpainted.step, device_dst.get(),
                     image_stride, static_cast<std::size_t>(inpainted.cols) * 3,
                     inpainted.rows, cudaMemcpyDeviceToHost),
        "Download inpainted image");
  if (!cv::imwrite(options.output, inpainted))
    throw std::runtime_error("Cannot write output: " + options.output);
  std::cout << "Saved inpainted output to " << options.output << "; damaged "
            << damaged_pixels << " pixels; CUDA inverse-distance inpaint took "
            << elapsed << " ms\n";
}
}  // namespace

int main(int argc, char** argv) {
  try {
    Run(Parse(argc, argv));
    return 0;
  } catch (const std::exception& e) {
    std::cerr << "inpaint_demo: " << e.what() << '\n';
    return 1;
  }
}
