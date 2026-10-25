#include "cv-cuda-pipeline/examples/desktop/cuda_image_ops/image_kernels.h"

#include <cuda_runtime.h>

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

namespace {

bool CheckCuda(cudaError_t status, const char* action) {
  if (status == cudaSuccess) {
    return true;
  }
  std::cerr << action << " failed: " << cudaGetErrorString(status) << '\n';
  return false;
}

bool CheckEqual(const std::string& operation,
                const std::vector<std::uint8_t>& actual,
                const std::vector<std::uint8_t>& expected) {
  if (actual == expected) {
    return true;
  }
  std::cerr << operation << " output mismatch\n";
  return false;
}

template <typename Launch>
bool RunByteKernel(const std::string& name,
                   const std::vector<std::uint8_t>& input, int width,
                   int height, Launch launch,
                   const std::vector<std::uint8_t>& expected) {
  std::uint8_t* device_input = nullptr;
  std::uint8_t* device_output = nullptr;
  const std::size_t bytes = input.size() * sizeof(std::uint8_t);
  if (!CheckCuda(
          cudaMalloc(reinterpret_cast<void**>(&device_input), bytes),
          "Allocate test input") ||
      !CheckCuda(
          cudaMalloc(reinterpret_cast<void**>(&device_output), bytes),
          "Allocate test output")) {
    cudaFree(device_output);
    cudaFree(device_input);
    return false;
  }

  std::vector<std::uint8_t> actual(input.size());
  bool succeeded =
      CheckCuda(cudaMemcpy(device_input, input.data(), bytes,
                           cudaMemcpyHostToDevice),
                "Copy test input");
  if (succeeded) {
    launch(device_input, device_output, width, height);
    succeeded = CheckCuda(cudaGetLastError(), "Launch test kernel") &&
                CheckCuda(cudaDeviceSynchronize(), "Run test kernel");
  }
  if (succeeded) {
    succeeded = CheckCuda(cudaMemcpy(actual.data(), device_output, bytes,
                                     cudaMemcpyDeviceToHost),
                          "Copy test output");
  }
  succeeded = CheckCuda(cudaFree(device_output), "Free test output") &&
              succeeded;
  succeeded =
      CheckCuda(cudaFree(device_input), "Free test input") && succeeded;
  return succeeded && CheckEqual(name, actual, expected);
}

bool TestConvertTo(const std::vector<std::uint8_t>& input, int width,
                   int height) {
  const std::size_t count = input.size();
  std::uint8_t* device_input = nullptr;
  std::uint8_t* device_output = nullptr;
  float* device_float = nullptr;
  if (!CheckCuda(cudaMalloc(reinterpret_cast<void**>(&device_input), count),
                 "Allocate convert input") ||
      !CheckCuda(cudaMalloc(reinterpret_cast<void**>(&device_output), count),
                 "Allocate convert output") ||
      !CheckCuda(cudaMalloc(reinterpret_cast<void**>(&device_float),
                            count * sizeof(float)),
                 "Allocate convert float buffer")) {
    cudaFree(device_float);
    cudaFree(device_output);
    cudaFree(device_input);
    return false;
  }
  std::vector<std::uint8_t> actual(count);
  bool succeeded =
      CheckCuda(cudaMemcpy(device_input, input.data(), count,
                           cudaMemcpyHostToDevice),
                "Copy convert input");
  if (succeeded) {
    LaunchConvertToFloat(device_input, device_float, width, height);
    LaunchConvertToUint8(device_float, device_output, width, height);
    succeeded = CheckCuda(cudaGetLastError(), "Launch convert kernels") &&
                CheckCuda(cudaDeviceSynchronize(), "Run convert kernels");
  }
  if (succeeded) {
    succeeded = CheckCuda(cudaMemcpy(actual.data(), device_output, count,
                                     cudaMemcpyDeviceToHost),
                          "Copy convert output");
  }
  succeeded = CheckCuda(cudaFree(device_float), "Free convert float buffer") &&
              succeeded;
  succeeded = CheckCuda(cudaFree(device_output), "Free convert output") &&
              succeeded;
  succeeded = CheckCuda(cudaFree(device_input), "Free convert input") &&
              succeeded;
  return succeeded && CheckEqual("convertto", actual, input);
}

}  // namespace

int main() {
  constexpr int kWidth = 2;
  constexpr int kHeight = 2;
  const std::vector<std::uint8_t> input = {
      10, 100, 200, 30,  64, 90, 128, 129, 200, 255, 250, 255,
  };
  bool passed = true;

  passed = RunByteKernel(
               "channelreorder", input, kWidth, kHeight, LaunchChannelReorder,
               {200, 100, 10, 90, 64, 30, 200, 129, 128, 255, 250, 255}) &&
           passed;
  passed = RunByteKernel(
               "flip", input, kWidth, kHeight, LaunchFlip,
               {30, 64, 90, 10, 100, 200, 255, 250, 255, 128, 129, 200}) &&
           passed;
  passed = RunByteKernel(
               "threshold", input, kWidth, kHeight, LaunchThreshold,
               {0, 0, 255, 0, 0, 0, 0, 255, 255, 255, 255, 255}) &&
           passed;
  passed = RunByteKernel(
               "color_twist", input, kWidth, kHeight, LaunchColorTwist,
               {0, 100, 250, 14, 64, 118, 92, 129, 250, 194, 250, 255}) &&
           passed;
  passed = RunByteKernel(
               "brightness_contrast", input, kWidth, kHeight,
               LaunchBrightnessContrast,
               {0, 154, 255, 7, 78, 133, 213, 215, 255, 255, 255, 255}) &&
           passed;
  passed = RunByteKernel(
               "gamma_contrast", input, kWidth, kHeight, LaunchGammaContrast,
               {0, 33, 149, 2, 12, 26, 56, 57, 149, 255, 244, 255}) &&
           passed;
  passed = TestConvertTo(input, kWidth, kHeight) && passed;
  if (!passed) {
    return 1;
  }
  std::cout << "All 7 CUDA image operator checks passed\n";
  return 0;
}
