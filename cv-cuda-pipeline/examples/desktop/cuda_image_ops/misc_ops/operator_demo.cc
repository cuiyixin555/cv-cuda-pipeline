#include "cv-cuda-pipeline/examples/desktop/cuda_image_ops/misc_ops/operator_kernels.h"

#include <cuda_runtime.h>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <algorithm>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace {

void CheckCuda(cudaError_t status, const char* operation) {
  if (status != cudaSuccess) {
    throw std::runtime_error(std::string(operation) + ": " +
                             cudaGetErrorString(status));
  }
}

class DeviceBuffer {
 public:
  explicit DeviceBuffer(std::size_t bytes) {
    CheckCuda(cudaMalloc(&data_, bytes), "Allocate CUDA image buffer");
  }
  ~DeviceBuffer() { cudaFree(data_); }
  DeviceBuffer(const DeviceBuffer&) = delete;
  DeviceBuffer& operator=(const DeviceBuffer&) = delete;
  std::uint8_t* get() { return static_cast<std::uint8_t*>(data_); }

 private:
  void* data_ = nullptr;
};

struct Arguments {
  std::string operation;
  std::string input;
  std::string output;
  std::string background;
  std::string mask;
  std::vector<std::string> stack_inputs;
  int output_width = 0;
  int output_height = 0;
};

void PrintUsage(const char* executable) {
  std::cerr
      << "Usage: " << executable
      << " OPERATOR INPUT OUTPUT [--background IMAGE] [--mask IMAGE]"
         " [--input IMAGE ...] [--size WIDTHxHEIGHT]\n"
         "Use operator_demo --help to list supported operators.\n";
}

Arguments ParseArguments(int argc, char** argv) {
  if (argc < 2 || std::string(argv[1]) == "--help") {
    std::cout
        << "Supported operators: bndbox, composite, cvcuda_tv_augment, "
           "cvcuda_tv_color, cvcuda_tv_compose, cvcuda_tv_geometry, "
           "cvcuda_tv_misc, inpaint, label, osd, stack, warp_affine, "
           "warp_perspective\n"
           "Required options: composite --background IMAGE --mask IMAGE; "
           "inpaint --mask IMAGE. For stack, repeat --input IMAGE to add "
           "images (the positional INPUT is included). --size WxH sets "
           "output dimensions for cvcuda_tv_compose and cvcuda_tv_geometry.\n";
    std::exit(argc < 2 ? 2 : 0);
  }
  if (argc < 4) {
    PrintUsage(argv[0]);
    throw std::invalid_argument("Missing required positional arguments");
  }
  Arguments args;
  args.operation = argv[1];
  args.input = argv[2];
  args.output = argv[3];
  for (int i = 4; i < argc; ++i) {
    const std::string option = argv[i];
    if (i + 1 >= argc) {
      throw std::invalid_argument("Missing value for " + option);
    }
    const std::string value = argv[++i];
    if (option == "--background") {
      args.background = value;
    } else if (option == "--mask") {
      args.mask = value;
    } else if (option == "--input") {
      args.stack_inputs.push_back(value);
    } else if (option == "--size") {
      const std::size_t separator = value.find('x');
      if (separator == std::string::npos) {
        throw std::invalid_argument("--size must be WIDTHxHEIGHT");
      }
      args.output_width = std::stoi(value.substr(0, separator));
      args.output_height = std::stoi(value.substr(separator + 1));
      if (args.output_width <= 0 || args.output_height <= 0) {
        throw std::invalid_argument("--size dimensions must be positive");
      }
    } else {
      throw std::invalid_argument("Unknown option: " + option);
    }
  }
  return args;
}

cv::Mat LoadImage(const std::string& path, int mode) {
  cv::Mat image = cv::imread(path, mode);
  if (image.empty()) {
    throw std::runtime_error("Unable to load image: " + path);
  }
  return image;
}

void RequireMatching(const cv::Mat& reference, const cv::Mat& other,
                     const char* name) {
  if (reference.size() != other.size()) {
    throw std::invalid_argument(std::string(name) +
                                " must have the same dimensions as input");
  }
}

template <typename Launch>
void RunSingleImage(const cv::Mat& source, cv::Mat& output, Launch launch) {
  const std::size_t bytes = source.total() * source.elemSize();
  DeviceBuffer device_source(bytes);
  DeviceBuffer device_output(bytes);
  CheckCuda(cudaMemcpy(device_source.get(), source.data, bytes,
                       cudaMemcpyHostToDevice),
            "Copy input image to CUDA");
  launch(device_source.get(), device_output.get());
  CheckCuda(cudaGetLastError(), "Check CUDA operator launch");
  CheckCuda(cudaDeviceSynchronize(), "Run CUDA operator");
  CheckCuda(cudaMemcpy(output.data, device_output.get(), bytes,
                       cudaMemcpyDeviceToHost),
            "Copy output image from CUDA");
}

void Run(const Arguments& args) {
  cv::Mat input = LoadImage(args.input, cv::IMREAD_COLOR);
  const int width = input.cols;
  const int height = input.rows;
  constexpr int kChannels = 3;
  const int default_output_width =
      args.operation == "cvcuda_tv_compose" ? 224 : width;
  const int default_output_height =
      args.operation == "cvcuda_tv_compose" ? 224 : height;
  const int output_width = args.output_width == 0 ? default_output_width
                                                  : args.output_width;
  const int output_height = args.output_height == 0 ? default_output_height
                                                    : args.output_height;
  if ((args.output_width != 0 || args.output_height != 0) &&
      args.operation != "cvcuda_tv_compose" &&
      args.operation != "cvcuda_tv_geometry") {
    throw std::invalid_argument(
        "--size is only supported by cvcuda_tv_compose and "
        "cvcuda_tv_geometry");
  }

  cv::Mat output;
  if (args.operation == "composite") {
    if (args.background.empty() || args.mask.empty()) {
      throw std::invalid_argument(
          "composite requires both --background and --mask");
    }
    cv::Mat background = LoadImage(args.background, cv::IMREAD_COLOR);
    cv::Mat mask = LoadImage(args.mask, cv::IMREAD_GRAYSCALE);
    RequireMatching(input, background, "Background");
    RequireMatching(input, mask, "Mask");
    output.create(height, width, CV_8UC3);
    const std::size_t image_bytes = input.total() * input.elemSize();
    const std::size_t mask_bytes = mask.total();
    DeviceBuffer device_foreground(image_bytes);
    DeviceBuffer device_background(image_bytes);
    DeviceBuffer device_mask(mask_bytes);
    DeviceBuffer device_output(image_bytes);
    CheckCuda(cudaMemcpy(device_foreground.get(), input.data, image_bytes,
                         cudaMemcpyHostToDevice),
              "Copy foreground to CUDA");
    CheckCuda(cudaMemcpy(device_background.get(), background.data, image_bytes,
                         cudaMemcpyHostToDevice),
              "Copy background to CUDA");
    CheckCuda(cudaMemcpy(device_mask.get(), mask.data, mask_bytes,
                         cudaMemcpyHostToDevice),
              "Copy composite mask to CUDA");
    LaunchComposite(device_foreground.get(), device_background.get(),
                    device_mask.get(), device_output.get(), width, height,
                    kChannels);
    CheckCuda(cudaGetLastError(), "Check composite launch");
    CheckCuda(cudaDeviceSynchronize(), "Run composite operator");
    CheckCuda(cudaMemcpy(output.data, device_output.get(), image_bytes,
                         cudaMemcpyDeviceToHost),
              "Copy composite output from CUDA");
  } else if (args.operation == "inpaint") {
    if (args.mask.empty()) {
      throw std::invalid_argument("inpaint requires --mask IMAGE");
    }
    cv::Mat mask = LoadImage(args.mask, cv::IMREAD_GRAYSCALE);
    RequireMatching(input, mask, "Inpaint mask");
    output.create(height, width, CV_8UC3);
    const std::size_t image_bytes = input.total() * input.elemSize();
    DeviceBuffer device_source(image_bytes);
    DeviceBuffer device_mask(mask.total());
    DeviceBuffer device_output(image_bytes);
    CheckCuda(cudaMemcpy(device_source.get(), input.data, image_bytes,
                         cudaMemcpyHostToDevice),
              "Copy inpaint input to CUDA");
    CheckCuda(cudaMemcpy(device_mask.get(), mask.data, mask.total(),
                         cudaMemcpyHostToDevice),
              "Copy inpaint mask to CUDA");
    LaunchInpaint(device_source.get(), device_mask.get(), device_output.get(),
                  width, height, kChannels);
    CheckCuda(cudaGetLastError(), "Check inpaint launch");
    CheckCuda(cudaDeviceSynchronize(), "Run inpaint operator");
    CheckCuda(cudaMemcpy(output.data, device_output.get(), image_bytes,
                         cudaMemcpyDeviceToHost),
              "Copy inpaint output from CUDA");
  } else if (args.operation == "stack") {
    std::vector<cv::Mat> images;
    images.push_back(input);
    for (const std::string& path : args.stack_inputs) {
      cv::Mat image = LoadImage(path, cv::IMREAD_COLOR);
      RequireMatching(input, image, "Stack image");
      images.push_back(std::move(image));
    }
    const int count = static_cast<int>(images.size());
    output.create(height * count, width, CV_8UC3);
    const std::size_t image_bytes = input.total() * input.elemSize();
    const std::size_t batch_bytes = image_bytes * count;
    std::vector<std::uint8_t> batch(batch_bytes);
    for (int i = 0; i < count; ++i) {
      std::copy(images[i].data, images[i].data + image_bytes,
                batch.data() + i * image_bytes);
    }
    DeviceBuffer device_source(batch_bytes);
    DeviceBuffer device_output(batch_bytes);
    CheckCuda(cudaMemcpy(device_source.get(), batch.data(), batch_bytes,
                         cudaMemcpyHostToDevice),
              "Copy stack batch to CUDA");
    LaunchStack(device_source.get(), device_output.get(), width, height,
                kChannels, count);
    CheckCuda(cudaGetLastError(), "Check stack launch");
    CheckCuda(cudaDeviceSynchronize(), "Run stack operator");
    CheckCuda(cudaMemcpy(output.data, device_output.get(), batch_bytes,
                         cudaMemcpyDeviceToHost),
              "Copy stack output from CUDA");
  } else if (args.operation == "label") {
    output.create(height, width, CV_8UC3);
    const std::size_t input_bytes = input.total() * input.elemSize();
    const std::size_t output_bytes = output.total() * output.elemSize();
    DeviceBuffer device_source(input_bytes);
    DeviceBuffer device_output(output_bytes);
    CheckCuda(cudaMemcpy(device_source.get(), input.data, input_bytes,
                         cudaMemcpyHostToDevice),
              "Copy label input to CUDA");
    LaunchLabel(device_source.get(), device_output.get(), width, height);
    CheckCuda(cudaGetLastError(), "Check label launch");
    CheckCuda(cudaDeviceSynchronize(), "Run label operator");
    CheckCuda(cudaMemcpy(output.data, device_output.get(), output_bytes,
                         cudaMemcpyDeviceToHost),
              "Copy label output from CUDA");
  } else {
    const bool compose = args.operation == "cvcuda_tv_compose";
    output.create(output_height, output_width, CV_8UC3);
    const std::size_t input_bytes = input.total() * input.elemSize();
    const std::size_t output_bytes = output.total() * output.elemSize();
    DeviceBuffer device_source(input_bytes);
    DeviceBuffer device_output(output_bytes);
    CheckCuda(cudaMemcpy(device_source.get(), input.data, input_bytes,
                         cudaMemcpyHostToDevice),
              "Copy input image to CUDA");
    auto launch = [&](std::uint8_t* src, std::uint8_t* dst) {
      if (args.operation == "bndbox") {
        LaunchBndbox(src, dst, width, height, kChannels);
      } else if (args.operation == "cvcuda_tv_augment") {
        LaunchTvAugment(src, dst, width, height, kChannels);
      } else if (args.operation == "cvcuda_tv_color") {
        LaunchTvColor(src, dst, width, height, kChannels);
      } else if (compose) {
        LaunchTvCompose(src, dst, width, height, kChannels, output_width,
                        output_height);
      } else if (args.operation == "cvcuda_tv_geometry") {
        LaunchTvGeometry(src, dst, width, height, kChannels, output_width,
                         output_height);
      } else if (args.operation == "cvcuda_tv_misc") {
        LaunchTvMisc(src, dst, width, height, kChannels);
      } else if (args.operation == "osd") {
        LaunchOsd(src, dst, width, height, kChannels);
      } else if (args.operation == "warp_affine") {
        LaunchWarpAffine(src, dst, width, height, kChannels);
      } else if (args.operation == "warp_perspective") {
        LaunchWarpPerspective(src, dst, width, height, kChannels);
      } else {
        throw std::invalid_argument("Unsupported operator: " + args.operation);
      }
    };
    launch(device_source.get(), device_output.get());
    CheckCuda(cudaGetLastError(), "Check CUDA operator launch");
    CheckCuda(cudaDeviceSynchronize(), "Run CUDA operator");
    CheckCuda(cudaMemcpy(output.data, device_output.get(), output_bytes,
                         cudaMemcpyDeviceToHost),
              "Copy output image from CUDA");
  }
  if (!cv::imwrite(args.output, output)) {
    throw std::runtime_error("Unable to write output image: " + args.output);
  }
  std::cout << "Wrote " << args.output << " (" << output.cols << "x"
            << output.rows << ")\n";
}

}  // namespace

int main(int argc, char** argv) {
  try {
    const Arguments args = ParseArguments(argc, argv);
    Run(args);
    return 0;
  } catch (const std::exception& error) {
    std::cerr << "operator_demo: " << error.what() << '\n';
    return 1;
  }
}
