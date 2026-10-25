#include "cv-cuda-pipeline/examples/desktop/cuda_image_ops/transform_ops/transform_kernels.h"

#include "cv-cuda-pipeline/framework/port/opencv_core_inc.h"
#include "cv-cuda-pipeline/framework/port/opencv_imgcodecs_inc.h"

#include <cuda_runtime.h>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <random>
#include <string>
#include <string_view>

namespace {

struct Options {
  TransformOperator operation{};
  bool has_operation = false;
  std::string input;
  std::string output = "operator_output.png";
  int width = 224;
  int height = 224;
  int crop_x = -1;
  int crop_y = -1;
  int crop_width = 0;
  int crop_height = 0;
  unsigned int seed = 42;
  float angle = 45.0f;
};

void PrintUsage(const char *executable) {
  std::cerr
      << "Usage: " << executable
      << " --operator NAME --input IMAGE [--output IMAGE] [--width N] "
         "[--height N] [--x N --y N --crop-width N --crop-height N] "
         "[--seed N] [--angle DEGREES]\n"
      << "Operators: center_crop, advcvtcolor, cvtcolor, reformat, resize, "
         "hq_resize, pillowresize, copymakeborder, customcrop, "
         "random_resized_crop, crop_flip_normalize_reformat, "
         "resize_crop_convert_reformat, remap, rotate\n"
      << "Defaults: output=operator_output.png, width=224, height=224, "
         "seed=42, angle=45.\n";
}

bool ParseInt(std::string_view text, int *value) {
  try {
    std::size_t consumed = 0;
    const int parsed = std::stoi(std::string(text), &consumed);
    if (consumed != text.size())
      return false;
    *value = parsed;
    return true;
  } catch (...) {
    return false;
  }
}

bool ParseUnsigned(std::string_view text, unsigned int *value) {
  try {
    std::size_t consumed = 0;
    const unsigned long parsed = std::stoul(std::string(text), &consumed);
    if (consumed != text.size() || parsed > 0xffffffffUL)
      return false;
    *value = static_cast<unsigned int>(parsed);
    return true;
  } catch (...) {
    return false;
  }
}

bool ParseFloat(std::string_view text, float *value) {
  try {
    std::size_t consumed = 0;
    const float parsed = std::stof(std::string(text), &consumed);
    if (consumed != text.size() || !std::isfinite(parsed))
      return false;
    *value = parsed;
    return true;
  } catch (...) {
    return false;
  }
}

bool ParseOptions(int argc, char **argv, Options *options) {
  for (int i = 1; i < argc; ++i) {
    const std::string_view argument(argv[i]);
    if (argument == "--help" || argument == "-h")
      return false;
    if (i + 1 >= argc) {
      std::cerr << "Missing value after " << argument << '\n';
      return false;
    }
    const std::string_view value(argv[++i]);
    if (argument == "--operator") {
      options->has_operation =
          ParseTransformOperator(value, &options->operation);
      if (!options->has_operation) {
        std::cerr << "Unsupported operator: " << value << '\n';
        return false;
      }
    } else if (argument == "--input") {
      options->input = value;
    } else if (argument == "--output") {
      options->output = value;
    } else if (argument == "--width") {
      if (!ParseInt(value, &options->width))
        return false;
    } else if (argument == "--height") {
      if (!ParseInt(value, &options->height))
        return false;
    } else if (argument == "--x") {
      if (!ParseInt(value, &options->crop_x))
        return false;
    } else if (argument == "--y") {
      if (!ParseInt(value, &options->crop_y))
        return false;
    } else if (argument == "--crop-width") {
      if (!ParseInt(value, &options->crop_width))
        return false;
    } else if (argument == "--crop-height") {
      if (!ParseInt(value, &options->crop_height))
        return false;
    } else if (argument == "--seed") {
      if (!ParseUnsigned(value, &options->seed))
        return false;
    } else if (argument == "--angle") {
      if (!ParseFloat(value, &options->angle))
        return false;
    } else {
      std::cerr << "Unknown argument: " << argument << '\n';
      return false;
    }
  }
  if (!options->has_operation || options->input.empty() ||
      options->width <= 0 || options->height <= 0) {
    return false;
  }
  return true;
}

bool CheckCuda(cudaError_t status, const char *action) {
  if (status == cudaSuccess)
    return true;
  std::cerr << action << " failed: " << cudaGetErrorString(status) << '\n';
  return false;
}

int Clamp(int value, int low, int high) {
  return std::max(low, std::min(value, high));
}

bool SetCrop(const Options &options, int image_width, int image_height,
             TransformConfig *config, bool random_crop) {
  if (random_crop) {
    std::mt19937 generator(options.seed);
    std::uniform_real_distribution<float> scale_distribution(0.08f, 1.0f);
    std::uniform_real_distribution<float> ratio_distribution(0.75f,
                                                             4.0f / 3.0f);
    bool selected = false;
    for (int attempt = 0; attempt < 10 && !selected; ++attempt) {
      const float area =
          image_width * image_height * scale_distribution(generator);
      const float ratio = ratio_distribution(generator);
      const int crop_width =
          static_cast<int>(std::round(std::sqrt(area * ratio)));
      const int crop_height =
          static_cast<int>(std::round(std::sqrt(area / ratio)));
      if (crop_width > 0 && crop_width <= image_width && crop_height > 0 &&
          crop_height <= image_height) {
        std::uniform_int_distribution<int> x_distribution(0, image_width -
                                                                 crop_width);
        std::uniform_int_distribution<int> y_distribution(0, image_height -
                                                                 crop_height);
        config->crop_x = x_distribution(generator);
        config->crop_y = y_distribution(generator);
        config->crop_width = crop_width;
        config->crop_height = crop_height;
        selected = true;
      }
    }
    if (!selected) {
      const float ratio = std::clamp(
          static_cast<float>(image_width) / image_height, 0.75f, 4.0f / 3.0f);
      if (static_cast<float>(image_width) / image_height > ratio) {
        config->crop_height = image_height;
        config->crop_width = std::min(
            image_width, static_cast<int>(std::round(image_height * ratio)));
      } else {
        config->crop_width = image_width;
        config->crop_height = std::min(
            image_height, static_cast<int>(std::round(image_width / ratio)));
      }
      config->crop_x = (image_width - config->crop_width) / 2;
      config->crop_y = (image_height - config->crop_height) / 2;
    }
    return true;
  }

  const bool custom = options.operation == TransformOperator::kCustomCrop;
  int x = options.crop_x >= 0 ? options.crop_x
                              : (custom ? image_width / 5 : image_width / 10);
  int y = options.crop_y >= 0 ? options.crop_y
                              : (custom ? image_height / 5 : image_height / 10);
  x = Clamp(x, 0, image_width - 1);
  y = Clamp(y, 0, image_height - 1);
  int width = options.crop_width > 0
                  ? options.crop_width
                  : (custom ? (image_width * 3) / 5 : (image_width * 8) / 10);
  int height = options.crop_height > 0 ? options.crop_height
                                       : (custom ? (image_height * 3) / 5
                                                 : (image_height * 8) / 10);
  width = Clamp(width, 1, image_width - x);
  height = Clamp(height, 1, image_height - y);
  config->crop_x = x;
  config->crop_y = y;
  config->crop_width = width;
  config->crop_height = height;
  return true;
}

} // namespace

int main(int argc, char **argv) {
  Options options;
  if (!ParseOptions(argc, argv, &options)) {
    PrintUsage(argv[0]);
    return 2;
  }

  cv::Mat image = cv::imread(options.input, cv::IMREAD_COLOR);
  if (image.empty()) {
    std::cerr << "Could not read input image: " << options.input << '\n';
    return 1;
  }
  if (!image.isContinuous())
    image = image.clone();

  int output_width = image.cols;
  int output_height = image.rows;
  TransformConfig config;
  config.angle_degrees = options.angle;
  switch (options.operation) {
  case TransformOperator::kCenterCrop:
    output_width = std::min(options.width, image.cols);
    output_height = std::min(options.height, image.rows);
    break;
  case TransformOperator::kCopyMakeBorder:
    output_width = image.cols + 120;
    output_height = image.rows + 60;
    break;
  case TransformOperator::kCustomCrop:
  case TransformOperator::kCropFlipNormalizeReformat:
    SetCrop(options, image.cols, image.rows, &config, false);
    output_width = config.crop_width;
    output_height = config.crop_height;
    break;
  case TransformOperator::kRandomResizedCrop:
    SetCrop(options, image.cols, image.rows, &config, true);
    output_width = options.width;
    output_height = options.height;
    break;
  case TransformOperator::kResize:
  case TransformOperator::kHqResize:
  case TransformOperator::kPillowResize:
  case TransformOperator::kResizeCropConvertReformat:
    output_width = options.width;
    output_height = options.height;
    break;
  default:
    break;
  }

  cv::Mat output(output_height, output_width, CV_8UC3);
  const std::size_t input_bytes = image.total() * image.elemSize();
  const std::size_t output_bytes = output.total() * output.elemSize();
  std::uint8_t *device_input = nullptr;
  std::uint8_t *device_output = nullptr;
  bool succeeded = CheckCuda(
      cudaMalloc(reinterpret_cast<void **>(&device_input), input_bytes),
      "Allocate CUDA input");
  if (succeeded) {
    succeeded = CheckCuda(
        cudaMalloc(reinterpret_cast<void **>(&device_output), output_bytes),
        "Allocate CUDA output");
  }
  if (succeeded) {
    succeeded = CheckCuda(cudaMemcpy(device_input, image.data, input_bytes,
                                     cudaMemcpyHostToDevice),
                          "Copy input image to CUDA");
  }
  if (succeeded) {
    succeeded = CheckCuda(LaunchTransform(options.operation, device_input,
                                          device_output, image.cols, image.rows,
                                          output_width, output_height, config),
                          "Launch CUDA operator") &&
                CheckCuda(cudaDeviceSynchronize(), "Run CUDA operator");
  }
  if (succeeded) {
    succeeded = CheckCuda(cudaMemcpy(output.data, device_output, output_bytes,
                                     cudaMemcpyDeviceToHost),
                          "Copy output image from CUDA");
  }
  const cudaError_t output_free =
      device_output == nullptr ? cudaSuccess : cudaFree(device_output);
  const cudaError_t input_free =
      device_input == nullptr ? cudaSuccess : cudaFree(device_input);
  succeeded = CheckCuda(output_free, "Free CUDA output") && succeeded;
  succeeded = CheckCuda(input_free, "Free CUDA input") && succeeded;
  if (!succeeded)
    return 1;

  if (!cv::imwrite(options.output, output)) {
    std::cerr << "Could not write output image: " << options.output << '\n';
    return 1;
  }
  std::cout << "Wrote " << TransformOperatorName(options.operation) << " "
            << output.cols << "x" << output.rows << " image to "
            << options.output << '\n';
  return 0;
}
