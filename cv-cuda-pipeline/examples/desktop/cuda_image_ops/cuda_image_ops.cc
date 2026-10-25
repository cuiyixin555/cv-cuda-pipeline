#include "cv-cuda-pipeline/examples/desktop/cuda_image_ops/image_kernels.h"

#include "cv-cuda-pipeline/framework/port/opencv_core_inc.h"
#include "cv-cuda-pipeline/framework/port/opencv_imgcodecs_inc.h"

#include <cuda_runtime.h>

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <string>
#include <string_view>

namespace {

struct Options {
  std::string operation;
  std::string input;
  std::string output;
};

void PrintUsage(const char* executable) {
  std::cerr << "Usage: " << executable
            << " --operator OP --input IMAGE --output IMAGE\n"
            << "Operators: brightness_contrast, channelreorder, color_twist, "
               "convertto, flip, gamma_contrast, threshold\n";
}

bool ParseOptions(int argc, char** argv, Options* options) {
  for (int i = 1; i < argc; ++i) {
    const std::string_view argument(argv[i]);
    if (i + 1 >= argc) {
      std::cerr << "Missing value after " << argument << '\n';
      return false;
    }
    const std::string value(argv[++i]);
    if (argument == "--operator") {
      options->operation = value;
    } else if (argument == "--input") {
      options->input = value;
    } else if (argument == "--output") {
      options->output = value;
    } else {
      std::cerr << "Unknown argument: " << argument << '\n';
      return false;
    }
  }
  if (options->operation.empty() || options->input.empty() ||
      options->output.empty()) {
    return false;
  }
  return true;
}

bool CheckCuda(cudaError_t status, const char* action) {
  if (status == cudaSuccess) {
    return true;
  }
  std::cerr << action << " failed: " << cudaGetErrorString(status) << '\n';
  return false;
}

bool IsSupportedOperator(const std::string& operation) {
  return operation == "brightness_contrast" || operation == "channelreorder" ||
         operation == "color_twist" || operation == "convertto" ||
         operation == "flip" || operation == "gamma_contrast" ||
         operation == "threshold";
}

bool RunKernel(const std::string& operation, const std::uint8_t* input,
               std::uint8_t* output, float* float_buffer, int width,
               int height) {
  if (operation == "brightness_contrast") {
    LaunchBrightnessContrast(input, output, width, height);
  } else if (operation == "channelreorder") {
    LaunchChannelReorder(input, output, width, height);
  } else if (operation == "color_twist") {
    LaunchColorTwist(input, output, width, height);
  } else if (operation == "convertto") {
    LaunchConvertToFloat(input, float_buffer, width, height);
    LaunchConvertToUint8(float_buffer, output, width, height);
  } else if (operation == "flip") {
    LaunchFlip(input, output, width, height);
  } else if (operation == "gamma_contrast") {
    LaunchGammaContrast(input, output, width, height);
  } else if (operation == "threshold") {
    LaunchThreshold(input, output, width, height);
  } else {
    std::cerr << "Unsupported operator: " << operation << '\n';
    return false;
  }
  return CheckCuda(cudaGetLastError(), "CUDA kernel launch") &&
         CheckCuda(cudaDeviceSynchronize(), "CUDA kernel execution");
}

}  // namespace

int main(int argc, char** argv) {
  Options options;
  if (!ParseOptions(argc, argv, &options)) {
    PrintUsage(argv[0]);
    return 2;
  }
  if (!IsSupportedOperator(options.operation)) {
    std::cerr << "Unsupported operator: " << options.operation << '\n';
    PrintUsage(argv[0]);
    return 2;
  }

  cv::Mat image = cv::imread(options.input, cv::IMREAD_COLOR);
  if (image.empty()) {
    std::cerr << "Could not read input image: " << options.input << '\n';
    return 1;
  }
  if (!image.isContinuous()) {
    image = image.clone();
  }
  cv::Mat output(image.rows, image.cols, CV_8UC3);

  const std::size_t byte_count = image.total() * image.elemSize();
  const std::size_t float_count = byte_count / sizeof(std::uint8_t);
  std::uint8_t* device_input = nullptr;
  std::uint8_t* device_output = nullptr;
  float* device_float = nullptr;
  if (!CheckCuda(cudaMalloc(reinterpret_cast<void**>(&device_input), byte_count),
                 "Allocate CUDA input") ||
      !CheckCuda(
          cudaMalloc(reinterpret_cast<void**>(&device_output), byte_count),
          "Allocate CUDA output") ||
      (options.operation == "convertto" &&
       !CheckCuda(cudaMalloc(reinterpret_cast<void**>(&device_float),
                             float_count * sizeof(float)),
                  "Allocate CUDA float buffer"))) {
    cudaFree(device_float);
    cudaFree(device_output);
    cudaFree(device_input);
    return 1;
  }

  bool succeeded =
      CheckCuda(cudaMemcpy(device_input, image.data, byte_count,
                           cudaMemcpyHostToDevice),
                "Copy input image to CUDA");
  if (succeeded) {
    succeeded = RunKernel(options.operation, device_input, device_output,
                          device_float, image.cols, image.rows);
  }
  if (succeeded) {
    succeeded =
        CheckCuda(cudaMemcpy(output.data, device_output, byte_count,
                             cudaMemcpyDeviceToHost),
                  "Copy output image from CUDA");
  }

  const cudaError_t free_float = cudaFree(device_float);
  const cudaError_t free_output = cudaFree(device_output);
  const cudaError_t free_input = cudaFree(device_input);
  succeeded = CheckCuda(free_float, "Free CUDA float buffer") && succeeded;
  succeeded = CheckCuda(free_output, "Free CUDA output buffer") && succeeded;
  succeeded = CheckCuda(free_input, "Free CUDA input buffer") && succeeded;
  if (!succeeded) {
    return 1;
  }

  if (!cv::imwrite(options.output, output)) {
    std::cerr << "Could not write output image: " << options.output << '\n';
    return 1;
  }
  std::cout << "Wrote " << options.operation << " result to " << options.output
            << '\n';
  return 0;
}
