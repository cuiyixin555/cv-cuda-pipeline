#include "cv-cuda-pipeline/examples/desktop/cuda_image_ops/transform_ops/transform_kernels.h"

#include <cuda_runtime.h>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

namespace {

bool CheckCuda(cudaError_t status, const char *action) {
  if (status == cudaSuccess)
    return true;
  std::cerr << action << " failed: " << cudaGetErrorString(status) << '\n';
  return false;
}

struct Result {
  int width = 0;
  int height = 0;
  std::vector<std::uint8_t> pixels;
};

std::vector<std::uint8_t> MakeImage(int width, int height) {
  std::vector<std::uint8_t> pixels(width * height * 3);
  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      const int i = (y * width + x) * 3;
      pixels[i] = static_cast<std::uint8_t>(x + y * 10);
      pixels[i + 1] = static_cast<std::uint8_t>(40 + x);
      pixels[i + 2] = static_cast<std::uint8_t>(80 + y);
    }
  }
  return pixels;
}

bool Run(TransformOperator operation, const std::vector<std::uint8_t> &input,
         int input_width, int input_height, int output_width, int output_height,
         const TransformConfig &config, Result *result) {
  std::uint8_t *device_input = nullptr;
  std::uint8_t *device_output = nullptr;
  const std::size_t input_size = input.size();
  const std::size_t output_size =
      static_cast<std::size_t>(output_width) * output_height * 3;
  bool succeeded = CheckCuda(
      cudaMalloc(reinterpret_cast<void **>(&device_input), input_size),
      "Allocate test input");
  if (succeeded) {
    succeeded = CheckCuda(
        cudaMalloc(reinterpret_cast<void **>(&device_output), output_size),
        "Allocate test output");
  }
  if (succeeded) {
    succeeded = CheckCuda(cudaMemcpy(device_input, input.data(), input_size,
                                     cudaMemcpyHostToDevice),
                          "Copy test input");
  }
  if (succeeded) {
    succeeded =
        CheckCuda(LaunchTransform(operation, device_input, device_output,
                                  input_width, input_height, output_width,
                                  output_height, config),
                  "Launch test operator") &&
        CheckCuda(cudaDeviceSynchronize(), "Run test operator");
  }
  result->width = output_width;
  result->height = output_height;
  result->pixels.resize(output_size);
  if (succeeded) {
    succeeded = CheckCuda(cudaMemcpy(result->pixels.data(), device_output,
                                     output_size, cudaMemcpyDeviceToHost),
                          "Copy test output");
  }
  const cudaError_t output_free =
      device_output == nullptr ? cudaSuccess : cudaFree(device_output);
  const cudaError_t input_free =
      device_input == nullptr ? cudaSuccess : cudaFree(device_input);
  succeeded = CheckCuda(output_free, "Free test output") && succeeded;
  succeeded = CheckCuda(input_free, "Free test input") && succeeded;
  return succeeded;
}

bool Check(bool condition, const char *message) {
  if (!condition)
    std::cerr << message << '\n';
  return condition;
}

int Pixel(const Result &result, int x, int y, int channel) {
  return result.pixels[(y * result.width + x) * 3 + channel];
}

} // namespace

int main() {
  constexpr int kWidth = 6;
  constexpr int kHeight = 5;
  const std::vector<std::uint8_t> input = MakeImage(kWidth, kHeight);
  TransformConfig config;
  Result result;
  bool passed = true;

  config = {};
  passed = Run(TransformOperator::kCenterCrop, input, kWidth, kHeight, 4, 3,
               config, &result) &&
           passed;
  passed = Check(result.width == 4 && result.height == 3 &&
                     Pixel(result, 0, 0, 0) == input[(1 * kWidth + 1) * 3],
                 "center_crop should return a centered 4x3 patch") &&
           passed;

  config = {};
  passed = Run(TransformOperator::kAdvCvtColor, input, kWidth, kHeight, kWidth,
               kHeight, config, &result) &&
           passed;
  for (std::size_t i = 0; i < input.size(); ++i) {
    passed = Check(std::abs(static_cast<int>(result.pixels[i]) -
                            static_cast<int>(input[i])) <= 3,
                   "advcvtcolor round-trip exceeded 3 intensity values") &&
             passed;
  }

  config = {};
  passed = Run(TransformOperator::kCvtColor, input, kWidth, kHeight, kWidth,
               kHeight, config, &result) &&
           passed;
  passed = Check(Pixel(result, 2, 1, 0) == input[(1 * kWidth + 2) * 3 + 2] &&
                     Pixel(result, 2, 1, 2) == input[(1 * kWidth + 2) * 3],
                 "cvtcolor should swap the outer channels") &&
           passed;

  config = {};
  passed = Run(TransformOperator::kReformat, input, kWidth, kHeight, kWidth,
               kHeight, config, &result) &&
           passed;
  passed = Check(result.pixels == input,
                 "reformat HWC/CHW round-trip should preserve bytes") &&
           passed;

  for (TransformOperator resize_op :
       {TransformOperator::kResize, TransformOperator::kHqResize,
        TransformOperator::kPillowResize}) {
    config = {};
    passed =
        Run(resize_op, input, kWidth, kHeight, 7, 6, config, &result) && passed;
    passed =
        Check(result.width == 7 && result.height == 6 && !result.pixels.empty(),
              "resize variants should produce the requested dimensions") &&
        passed;
  }

  config = {};
  passed = Run(TransformOperator::kCopyMakeBorder, input, kWidth, kHeight,
               kWidth + 120, kHeight + 60, config, &result) &&
           passed;
  passed =
      Check(result.width == 126 && result.height == 65 &&
                Pixel(result, 0, 0, 0) == 0 && Pixel(result, 0, 0, 1) == 140 &&
                Pixel(result, 0, 0, 2) == 255 &&
                Pixel(result, 60, 30, 0) == input[0],
            "copymakeborder should add the configured solid border") &&
      passed;

  config = {};
  config.crop_x = 1;
  config.crop_y = 1;
  config.crop_width = 3;
  config.crop_height = 3;
  passed = Run(TransformOperator::kCustomCrop, input, kWidth, kHeight, 3, 3,
               config, &result) &&
           passed;
  passed = Check(Pixel(result, 0, 0, 0) == input[(kWidth + 1) * 3],
                 "customcrop should begin at its requested origin") &&
           passed;

  config.crop_x = 1;
  config.crop_y = 1;
  config.crop_width = 4;
  config.crop_height = 3;
  passed = Run(TransformOperator::kRandomResizedCrop, input, kWidth, kHeight, 7,
               6, config, &result) &&
           passed;
  passed =
      Check(result.width == 7 && result.height == 6 && !result.pixels.empty(),
            "random_resized_crop should resize its selected crop") &&
      passed;

  config = {};
  config.crop_x = 0;
  config.crop_y = 0;
  passed = Run(TransformOperator::kCropFlipNormalizeReformat, input, kWidth,
               kHeight, 4, 4, config, &result) &&
           passed;
  passed = Check(Pixel(result, 0, 0, 0) == input[3 * 3] &&
                     Pixel(result, 3, 0, 0) == input[0],
                 "crop_flip_normalize_reformat should horizontally flip") &&
           passed;

  config = {};
  passed = Run(TransformOperator::kResizeCropConvertReformat, input, kWidth,
               kHeight, 7, 6, config, &result) &&
           passed;
  passed = Check(Pixel(result, 0, 0, 0) == input[2] &&
                     Pixel(result, 0, 0, 2) == input[0],
                 "resize_crop_convert_reformat should reverse channels") &&
           passed;

  config = {};
  passed = Run(TransformOperator::kRemap, input, kWidth, kHeight, kWidth,
               kHeight, config, &result) &&
           passed;
  passed = Check(result.width == kWidth && result.height == kHeight &&
                     !result.pixels.empty(),
                 "remap should preserve image dimensions") &&
           passed;

  config = {};
  config.angle_degrees = 45.0f;
  passed = Run(TransformOperator::kRotate, input, kWidth, kHeight, kWidth,
               kHeight, config, &result) &&
           passed;
  passed = Check(result.width == kWidth && result.height == kHeight &&
                     !result.pixels.empty(),
                 "rotate should preserve image dimensions") &&
           passed;

  if (!passed)
    return 1;
  std::cout << "All 14 transform operator CUDA checks passed\n";
  return 0;
}
