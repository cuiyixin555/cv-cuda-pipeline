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
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

#include "cv-cuda-pipeline/calculators/image_process/cuda/color_twist_kernel.h"
#include "cv-cuda-pipeline/calculators/image_process/cuda/flip_kernel.h"
#include "cv-cuda-pipeline/calculators/image_process/cuda/normalize_kernel.h"
#include "cv-cuda-pipeline/calculators/image_process/cuda/random_resized_crop.h"
#include "cv-cuda-pipeline/calculators/image_process/cuda/resize_kernel.h"
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
  float* get_float() const { return static_cast<float*>(data_); }

 private:
  void* data_ = nullptr;
};
struct Options {
  std::string input;
  std::string output = "cvcuda_tv_compose.png";
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
cv_cuda_pipeline::ColorTwistMatrix Scale(float factor) {
  cv_cuda_pipeline::ColorTwistMatrix matrix{};
  for (int c = 0; c < 3; ++c) matrix.rows[c][c] = factor;
  return matrix;
}
cv_cuda_pipeline::ColorTwistMatrix Contrast(float factor, float mean) {
  auto matrix = Scale(factor);
  for (int c = 0; c < 3; ++c) matrix.rows[c][3] = (1.0f - factor) * mean;
  return matrix;
}
cv_cuda_pipeline::ColorTwistMatrix Saturation(float factor) {
  constexpr float kGray[] = {0.114f, 0.587f, 0.299f};
  cv_cuda_pipeline::ColorTwistMatrix matrix{};
  for (int c = 0; c < 3; ++c)
    for (int k = 0; k < 3; ++k)
      matrix.rows[c][k] = (c == k ? factor : 0.0f) + (1.0f - factor) * kGray[k];
  return matrix;
}
float GrayMeanAfterBrightness(const cv::Mat& image, float brightness) {
  double sum = 0.0;
  for (int y = 0; y < image.rows; ++y) {
    const auto* row = image.ptr<cv::Vec3b>(y);
    for (int x = 0; x < image.cols; ++x) {
      const float b =
          std::nearbyint(std::clamp(row[x][0] * brightness, 0.0f, 255.0f));
      const float g =
          std::nearbyint(std::clamp(row[x][1] * brightness, 0.0f, 255.0f));
      const float r =
          std::nearbyint(std::clamp(row[x][2] * brightness, 0.0f, 255.0f));
      sum += 0.114 * b + 0.587 * g + 0.299 * r;
    }
  }
  return static_cast<float>(sum /
                            (static_cast<double>(image.rows) * image.cols));
}

void Run(const Options& options) {
  const cv::Mat input = cv::imread(options.input, cv::IMREAD_COLOR);
  if (input.empty())
    throw std::runtime_error("Cannot read input: " + options.input);
  cv_cuda_pipeline::CropRectangle crop{};
  if (!cv_cuda_pipeline::SelectRandomResizedCrop(
          input.cols, input.rows, 0.5f, 1.0f, 0.75f, 1.3333333f, 0, &crop))
    throw std::runtime_error("Could not select random resized crop");

  std::mt19937 generator(0);
  std::uniform_real_distribution<float> jitter(0.7f, 1.3f);
  const float brightness = jitter(generator);
  const float contrast = jitter(generator);
  const float saturation = jitter(generator);
  const bool flip = (generator() & 1U) != 0;
  cv::Mat cpu_resized;
  cv::resize(input(cv::Rect(crop.x, crop.y, crop.width, crop.height)),
             cpu_resized, cv::Size(options.width, options.height), 0.0, 0.0,
             cv::INTER_LINEAR);
  if (flip) cv::flip(cpu_resized, cpu_resized, 1);
  const float gray_mean = GrayMeanAfterBrightness(cpu_resized, brightness);

  const std::size_t src_stride = static_cast<std::size_t>(input.cols) * 3;
  const std::size_t row = static_cast<std::size_t>(options.width) * 3;
  const std::size_t bytes = row * options.height;
  const std::size_t float_row = row * sizeof(float);
  DeviceBuffer src(src_stride * input.rows), resized(bytes), flipped(bytes),
      bright(bytes), contrasted(bytes), jittered(bytes),
      normalized(float_row * options.height);
  Check(cudaMemcpy2D(src.get(), src_stride, input.data, input.step, src_stride,
                     input.rows, cudaMemcpyHostToDevice),
        "Upload input");
  const auto* crop_src =
      src.get() + static_cast<std::size_t>(crop.y) * src_stride + crop.x * 3;
  const std::size_t crop_batch_stride =
      static_cast<std::size_t>(crop.height - 1) * src_stride +
      static_cast<std::size_t>(crop.width) * 3;
  const cv_cuda_pipeline::ResizeConstImageView resize_src{
      crop_src, crop.width, crop.height, src_stride, crop_batch_stride};
  const cv_cuda_pipeline::ResizeImageView resize_dst{
      resized.get(), options.width, options.height, row, bytes};
  const auto run_pipeline = [&] {
    Check(cv_cuda_pipeline::ResizeLinear(resize_src, resize_dst, 1),
          "Random resized crop");
    if (flip) {
      Check(cv_cuda_pipeline::Flip({resized.get(), row, flipped.get(), row,
                                    options.width, options.height},
                                   1),
            "Horizontal flip");
    } else {
      Check(cudaMemcpy(flipped.get(), resized.get(), bytes,
                       cudaMemcpyDeviceToDevice),
            "Copy unflipped crop");
    }
    const cv_cuda_pipeline::ColorTwistImageView image{
        flipped.get(), row, bright.get(), row, options.width, options.height};
    Check(cv_cuda_pipeline::ColorTwist(image, Scale(brightness)),
          "Color jitter brightness");
    Check(cv_cuda_pipeline::ColorTwist({bright.get(), row, contrasted.get(),
                                        row, options.width, options.height},
                                       Contrast(contrast, gray_mean)),
          "Color jitter contrast");
    Check(cv_cuda_pipeline::ColorTwist({contrasted.get(), row, jittered.get(),
                                        row, options.width, options.height},
                                       Saturation(saturation)),
          "Color jitter saturation");
    Check(cv_cuda_pipeline::NormalizeImageNet(
              {jittered.get(), row, normalized.get_float(), float_row,
               options.width, options.height}),
          "Normalize float output");
  };
  run_pipeline();
  std::vector<float> first(bytes);
  Check(cudaMemcpy(first.data(), normalized.get(), float_row * options.height,
                   cudaMemcpyDeviceToHost),
        "Download first normalized output");
  run_pipeline();
  std::vector<float> second(bytes);
  Check(cudaMemcpy(second.data(), normalized.get(), float_row * options.height,
                   cudaMemcpyDeviceToHost),
        "Download repeated normalized output");
  if (first != second)
    throw std::runtime_error("Seeded composition was not deterministic");
  cv::Mat preview(options.height, options.width, CV_8UC3);
  Check(cudaMemcpy2D(preview.data, preview.step, flipped.get(), row, row,
                     options.height, cudaMemcpyDeviceToHost),
        "Download crop/flip preview");
  if (!cv::imwrite(options.output, preview))
    throw std::runtime_error("Cannot write output: " + options.output);
  std::cout << "Saved seeded crop/flip preview to " << options.output << " ("
            << preview.cols << "x" << preview.rows << "), crop (" << crop.x
            << ", " << crop.y << ", " << crop.width << "x" << crop.height
            << "), full two-run F32 pipeline verified deterministic\n";
}
}  // namespace

int main(int argc, char** argv) {
  try {
    Run(Parse(argc, argv));
    return 0;
  } catch (const std::exception& e) {
    std::cerr << "tv_compose_demo: " << e.what() << '\n';
    return 1;
  }
}
