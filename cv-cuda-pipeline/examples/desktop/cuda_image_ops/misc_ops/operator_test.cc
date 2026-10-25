#include "cv-cuda-pipeline/examples/desktop/cuda_image_ops/misc_ops/operator_kernels.h"

#include <cuda_runtime.h>

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

namespace {

bool CheckCuda(cudaError_t status, const char* action) {
  if (status == cudaSuccess) return true;
  std::cerr << action << ": " << cudaGetErrorString(status) << '\n';
  return false;
}

bool Finish(const char* operation) {
  return CheckCuda(cudaGetLastError(), operation) &&
         CheckCuda(cudaDeviceSynchronize(), operation);
}

struct Buffers {
  std::uint8_t* input = nullptr;
  std::uint8_t* extra = nullptr;
  std::uint8_t* output = nullptr;
  int* labels = nullptr;

  ~Buffers() {
    cudaFree(labels);
    cudaFree(output);
    cudaFree(extra);
    cudaFree(input);
  }
};

bool Allocate(Buffers& buffers, std::size_t input_bytes,
              std::size_t output_bytes, std::size_t extra_bytes = 0) {
  return CheckCuda(cudaMalloc(reinterpret_cast<void**>(&buffers.input),
                              input_bytes),
                   "Allocate test input") &&
         CheckCuda(cudaMalloc(reinterpret_cast<void**>(&buffers.output),
                              output_bytes),
                   "Allocate test output") &&
         (extra_bytes == 0 ||
          CheckCuda(cudaMalloc(reinterpret_cast<void**>(&buffers.extra),
                               extra_bytes),
                    "Allocate test extra input"));
}

bool CopyToDevice(std::uint8_t* destination,
                  const std::vector<std::uint8_t>& source) {
  return CheckCuda(cudaMemcpy(destination, source.data(), source.size(),
                              cudaMemcpyHostToDevice),
                   "Copy test input to CUDA");
}

bool CopyFromDevice(std::vector<std::uint8_t>& destination,
                    const std::uint8_t* source) {
  return CheckCuda(cudaMemcpy(destination.data(), source, destination.size(),
                              cudaMemcpyDeviceToHost),
                   "Copy test output from CUDA");
}

bool TestUnary(const char* name, void (*launch)(const std::uint8_t*,
                                                 std::uint8_t*, int, int, int),
               bool expect_change) {
  constexpr int kWidth = 8;
  constexpr int kHeight = 8;
  constexpr int kChannels = 3;
  std::vector<std::uint8_t> input(kWidth * kHeight * kChannels, 100);
  std::vector<std::uint8_t> output(input.size());
  Buffers buffers;
  if (!Allocate(buffers, input.size(), output.size()) ||
      !CopyToDevice(buffers.input, input)) {
    return false;
  }
  launch(buffers.input, buffers.output, kWidth, kHeight, kChannels);
  if (!Finish(name) || !CopyFromDevice(output, buffers.output)) return false;
  if (expect_change && output == input) {
    std::cerr << name << " did not modify the synthetic image\n";
    return false;
  }
  if (!expect_change && output != input) {
    std::cerr << name << " changed a constant image unexpectedly\n";
    return false;
  }
  return true;
}

bool TestComposite() {
  constexpr int kWidth = 2;
  constexpr int kHeight = 2;
  constexpr int kChannels = 3;
  std::vector<std::uint8_t> foreground(12, 200);
  std::vector<std::uint8_t> background(12, 20);
  std::vector<std::uint8_t> mask = {255, 0, 0, 255};
  std::vector<std::uint8_t> output(12);
  Buffers buffers;
  if (!Allocate(buffers, foreground.size(), output.size(), mask.size()) ||
      !CopyToDevice(buffers.input, foreground) ||
      !CopyToDevice(buffers.extra, mask)) {
    return false;
  }
  std::uint8_t* device_background = nullptr;
  if (!CheckCuda(cudaMalloc(reinterpret_cast<void**>(&device_background),
                            background.size()),
                 "Allocate composite background") ||
      !CopyToDevice(device_background, background)) {
    cudaFree(device_background);
    return false;
  }
  LaunchComposite(buffers.input, device_background, buffers.extra,
                  buffers.output, kWidth, kHeight, kChannels);
  const bool succeeded =
      Finish("composite") && CopyFromDevice(output, buffers.output);
  cudaFree(device_background);
  return succeeded &&
         output == std::vector<std::uint8_t>(
                       {200, 200, 200, 20, 20, 20, 20, 20, 20, 200, 200, 200});
}

bool TestInpaint() {
  constexpr int kWidth = 3;
  constexpr int kHeight = 3;
  constexpr int kChannels = 3;
  std::vector<std::uint8_t> input(kWidth * kHeight * kChannels, 80);
  std::vector<std::uint8_t> mask(kWidth * kHeight, 0);
  mask[4] = 255;
  input[12] = input[13] = input[14] = 0;
  std::vector<std::uint8_t> output(input.size());
  Buffers buffers;
  if (!Allocate(buffers, input.size(), output.size(), mask.size()) ||
      !CopyToDevice(buffers.input, input) ||
      !CopyToDevice(buffers.extra, mask)) {
    return false;
  }
  LaunchInpaint(buffers.input, buffers.extra, buffers.output, kWidth, kHeight,
                kChannels);
  return Finish("inpaint") && CopyFromDevice(output, buffers.output) &&
         output[12] == 80 && output[13] == 80 && output[14] == 80 &&
         output[0] == 80;
}

bool TestTvCompose() {
  constexpr int kWidth = 8;
  constexpr int kHeight = 8;
  constexpr int kChannels = 3;
  constexpr int kOutputWidth = 4;
  constexpr int kOutputHeight = 4;
  std::vector<std::uint8_t> input(kWidth * kHeight * kChannels, 90);
  std::vector<std::uint8_t> output(kOutputWidth * kOutputHeight * kChannels);
  Buffers buffers;
  if (!Allocate(buffers, input.size(), output.size()) ||
      !CopyToDevice(buffers.input, input)) {
    return false;
  }
  LaunchTvCompose(buffers.input, buffers.output, kWidth, kHeight, kChannels,
                  kOutputWidth, kOutputHeight);
  return Finish("cvcuda_tv_compose") &&
         CopyFromDevice(output, buffers.output) &&
         output == std::vector<std::uint8_t>(output.size(), 90);
}

bool TestTvGeometry() {
  constexpr int kWidth = 8;
  constexpr int kHeight = 8;
  constexpr int kChannels = 3;
  std::vector<std::uint8_t> input(kWidth * kHeight * kChannels, 100);
  std::vector<std::uint8_t> output(input.size());
  Buffers buffers;
  if (!Allocate(buffers, input.size(), output.size()) ||
      !CopyToDevice(buffers.input, input)) {
    return false;
  }
  LaunchTvGeometry(buffers.input, buffers.output, kWidth, kHeight, kChannels,
                   kWidth, kHeight);
  if (!Finish("cvcuda_tv_geometry") ||
      !CopyFromDevice(output, buffers.output)) {
    return false;
  }
  return output != input;
}

bool TestStack() {
  constexpr int kWidth = 2;
  constexpr int kHeight = 2;
  constexpr int kChannels = 3;
  constexpr int kCount = 3;
  std::vector<std::uint8_t> input(kWidth * kHeight * kChannels * kCount);
  for (std::size_t i = 0; i < input.size(); ++i) {
    input[i] = static_cast<std::uint8_t>(i);
  }
  std::vector<std::uint8_t> output(input.size());
  Buffers buffers;
  if (!Allocate(buffers, input.size(), output.size()) ||
      !CopyToDevice(buffers.input, input)) {
    return false;
  }
  LaunchStack(buffers.input, buffers.output, kWidth, kHeight, kChannels,
              kCount);
  return Finish("stack") && CopyFromDevice(output, buffers.output) &&
         output == input;
}

bool TestLabel() {
  constexpr int kWidth = 5;
  constexpr int kHeight = 3;
  std::vector<std::uint8_t> input(kWidth * kHeight * 3, 0);
  for (int x = 0; x < 3; ++x) {
    input[(kWidth + x) * 3] = 255;
    input[(kWidth + x) * 3 + 1] = 255;
    input[(kWidth + x) * 3 + 2] = 255;
  }
  std::vector<std::uint8_t> output(input.size());
  Buffers buffers;
  if (!Allocate(buffers, input.size(), output.size()) ||
      !CopyToDevice(buffers.input, input)) {
    return false;
  }
  LaunchLabel(buffers.input, buffers.output, kWidth, kHeight);
  if (!Finish("label") || !CopyFromDevice(output, buffers.output)) return false;
  const std::size_t first = (kWidth + 0) * 3;
  const std::size_t last = (kWidth + 2) * 3;
  return output[0] == 0 && output[1] == 0 && output[2] == 0 &&
         output[first] == output[last] &&
         output[first + 1] == output[last + 1] &&
         output[first + 2] == output[last + 2] && output[first] != 0;
}

bool TestAll() {
  const bool passed =
      TestUnary("bndbox", LaunchBndbox, true) &&
      TestComposite() &&
      TestUnary("cvcuda_tv_augment", LaunchTvAugment, true) &&
      TestUnary("cvcuda_tv_color", LaunchTvColor, true) &&
      TestTvCompose() &&
      TestTvGeometry() &&
      TestUnary("cvcuda_tv_misc", LaunchTvMisc, true) &&
      TestInpaint() &&
      TestLabel() &&
      TestUnary("osd", LaunchOsd, true) &&
      TestStack() &&
      TestUnary("warp_affine", LaunchWarpAffine, true) &&
      TestUnary("warp_perspective", LaunchWarpPerspective, true);
  if (passed) {
    std::cout << "All 13 miscellaneous CUDA operator tests passed\n";
  }
  return passed;
}

}  // namespace

int main() {
  int device_count = 0;
  if (!CheckCuda(cudaGetDeviceCount(&device_count), "Query CUDA devices") ||
      device_count == 0) {
    std::cerr << "A CUDA device is required for operator_test\n";
    return 1;
  }
  return TestAll() ? 0 : 1;
}
