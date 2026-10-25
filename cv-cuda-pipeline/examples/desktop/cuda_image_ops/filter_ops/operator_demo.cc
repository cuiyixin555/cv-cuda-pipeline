#include "cv-cuda-pipeline/examples/desktop/cuda_image_ops/filter_ops/filter_kernels.h"

#include "cv-cuda-pipeline/framework/port/opencv_core_inc.h"
#include "cv-cuda-pipeline/framework/port/opencv_imgcodecs_inc.h"

#include <cuda_runtime.h>

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <string>
#include <string_view>

namespace {

bool IsSupported(const std::string& name) {
  return name == "adaptivethreshold" || name == "averageblur" ||
         name == "bilateral_filter" || name == "boxblur" || name == "clahe" ||
         name == "conv2d" || name == "erase" || name == "gaussian" ||
         name == "gaussiannoise" || name == "histogrameq" ||
         name == "joint_bilateral_filter" || name == "laplacian" ||
         name == "median_blur" || name == "morphology" ||
         name == "normalize";
}

void Usage(const char* executable) {
  std::cerr << "Usage: " << executable
            << " --operator OP --input IMAGE --output IMAGE\n";
}

bool Parse(int argc, char** argv, std::string* operation, std::string* input,
           std::string* output) {
  for (int i = 1; i < argc; ++i) {
    if (i + 1 >= argc) return false;
    const std::string_view arg(argv[i]);
    const std::string value(argv[++i]);
    if (arg == "--operator") *operation = value;
    else if (arg == "--input") *input = value;
    else if (arg == "--output") *output = value;
    else return false;
  }
  return !operation->empty() && !input->empty() && !output->empty();
}

bool CheckCuda(cudaError_t status, const char* operation) {
  if (status == cudaSuccess) return true;
  std::cerr << operation << " failed: " << cudaGetErrorString(status) << '\n';
  return false;
}

}  // namespace

int main(int argc, char** argv) {
  std::string operation;
  std::string input_path;
  std::string output_path;
  if (!Parse(argc, argv, &operation, &input_path, &output_path) ||
      !IsSupported(operation)) {
    Usage(argv[0]);
    return 2;
  }
  cv::Mat input = cv::imread(input_path, cv::IMREAD_COLOR);
  if (input.empty()) {
    std::cerr << "Could not read input image: " << input_path << '\n';
    return 1;
  }
  if (!input.isContinuous()) input = input.clone();
  cv::Mat output(input.rows, input.cols, CV_8UC3);
  const std::size_t bytes = input.total() * input.elemSize();
  std::uint8_t* device_input = nullptr;
  std::uint8_t* device_output = nullptr;
  if (!CheckCuda(cudaMalloc(reinterpret_cast<void**>(&device_input), bytes),
                 "Allocate CUDA input") ||
      !CheckCuda(cudaMalloc(reinterpret_cast<void**>(&device_output), bytes),
                 "Allocate CUDA output")) {
    if (device_output) cudaFree(device_output);
    if (device_input) cudaFree(device_input);
    return 1;
  }
  bool ok = CheckCuda(cudaMemcpy(device_input, input.data, bytes,
                                 cudaMemcpyHostToDevice),
                      "Copy input to CUDA");
  if (ok) {
    ok = CheckCuda(RunFilterOperator(operation, device_input, device_output,
                                     input.cols, input.rows, input.channels()),
                   "Run CUDA operator");
  }
  if (ok) {
    ok = CheckCuda(cudaMemcpy(output.data, device_output, bytes,
                              cudaMemcpyDeviceToHost),
                   "Copy CUDA output");
  }
  const cudaError_t output_free = cudaFree(device_output);
  const cudaError_t input_free = cudaFree(device_input);
  ok = CheckCuda(output_free, "Free CUDA output") && ok;
  ok = CheckCuda(input_free, "Free CUDA input") && ok;
  if (!ok) return 1;
  if (!cv::imwrite(output_path, output)) {
    std::cerr << "Could not write output image: " << output_path << '\n';
    return 1;
  }
  std::cout << "Wrote " << operation << " result to " << output_path << '\n';
  return 0;
}
