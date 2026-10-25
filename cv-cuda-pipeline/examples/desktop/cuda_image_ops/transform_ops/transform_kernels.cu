#include "cv-cuda-pipeline/examples/desktop/cuda_image_ops/transform_ops/transform_kernels.h"

#include <cuda_runtime.h>

#include <cmath>
#include <cstdint>

namespace {

constexpr int kChannels = 3;
constexpr int kThreads = 256;
constexpr float kPi = 3.14159265358979323846f;

__device__ std::uint8_t ToByte(float value) {
  return static_cast<std::uint8_t>(
      __float2int_rn(fminf(255.0f, fmaxf(0.0f, value))));
}

__device__ float Read(const std::uint8_t *image, int width, int height, int x,
                      int y, int channel) {
  x = max(0, min(width - 1, x));
  y = max(0, min(height - 1, y));
  return static_cast<float>(image[(y * width + x) * kChannels + channel]);
}

__device__ float Bilinear(const std::uint8_t *image, int width, int height,
                          float x, float y, int channel) {
  x = fminf(static_cast<float>(width - 1), fmaxf(0.0f, x));
  y = fminf(static_cast<float>(height - 1), fmaxf(0.0f, y));
  const int x0 = static_cast<int>(floorf(x));
  const int y0 = static_cast<int>(floorf(y));
  const int x1 = min(x0 + 1, width - 1);
  const int y1 = min(y0 + 1, height - 1);
  const float dx = x - x0;
  const float dy = y - y0;
  return (1.0f - dy) *
             ((1.0f - dx) * Read(image, width, height, x0, y0, channel) +
              dx * Read(image, width, height, x1, y0, channel)) +
         dy * ((1.0f - dx) * Read(image, width, height, x0, y1, channel) +
               dx * Read(image, width, height, x1, y1, channel));
}

__device__ float Sinc(float x) {
  if (fabsf(x) < 1.0e-5f) {
    return 1.0f;
  }
  const float px = kPi * x;
  return sinf(px) / px;
}

__device__ float Lanczos(float x) {
  return fabsf(x) < 3.0f ? Sinc(x) * Sinc(x / 3.0f) : 0.0f;
}

__device__ float FilteredSample(const std::uint8_t *image, int width,
                                int height, float x, float y, int channel,
                                float scale_x, float scale_y, int mode) {
  const bool lanczos_x = mode == 2 || (mode == 1 && scale_x > 1.0f);
  const bool lanczos_y = mode == 2 || (mode == 1 && scale_y > 1.0f);
  const float filter_x = lanczos_x ? fmaxf(1.0f, scale_x) : 1.0f;
  const float filter_y = lanczos_y ? fmaxf(1.0f, scale_y) : 1.0f;
  const int radius_x = lanczos_x ? static_cast<int>(ceilf(3.0f * filter_x)) : 1;
  const int radius_y = lanczos_y ? static_cast<int>(ceilf(3.0f * filter_y)) : 1;
  const int center_x = static_cast<int>(floorf(x));
  const int center_y = static_cast<int>(floorf(y));
  float total = 0.0f;
  float weight_sum = 0.0f;
  for (int sy = center_y - radius_y + 1; sy <= center_y + radius_y; ++sy) {
    const float dy = fabsf((y - sy) / filter_y);
    const float wy = lanczos_y ? Lanczos(dy) : fmaxf(0.0f, 1.0f - dy);
    for (int sx = center_x - radius_x + 1; sx <= center_x + radius_x; ++sx) {
      const float dx = fabsf((x - sx) / filter_x);
      const float wx = lanczos_x ? Lanczos(dx) : fmaxf(0.0f, 1.0f - dx);
      const float weight = wy * wx;
      total += Read(image, width, height, sx, sy, channel) * weight;
      weight_sum += weight;
    }
  }
  return fabsf(weight_sum) > 1.0e-6f ? total / weight_sum : 0.0f;
}

__device__ void ResizePixel(const std::uint8_t *input, std::uint8_t *output,
                            int input_width, int input_height, int output_width,
                            int output_height, int pixel, int mode) {
  const int x = pixel % output_width;
  const int y = pixel / output_width;
  const float sx = (x + 0.5f) * input_width / output_width - 0.5f;
  const float sy = (y + 0.5f) * input_height / output_height - 0.5f;
  for (int c = 0; c < kChannels; ++c) {
    const float value =
        mode == 0
            ? Bilinear(input, input_width, input_height, sx, sy, c)
            : FilteredSample(input, input_width, input_height, sx, sy, c,
                             static_cast<float>(input_width) / output_width,
                             static_cast<float>(input_height) / output_height,
                             mode);
    output[pixel * kChannels + c] = ToByte(value);
  }
}

__global__ void CenterCropKernel(const std::uint8_t *input,
                                 std::uint8_t *output, int input_width,
                                 int input_height, int output_width,
                                 int output_height) {
  const int pixel = blockIdx.x * blockDim.x + threadIdx.x;
  if (pixel >= output_width * output_height)
    return;
  const int x = pixel % output_width + (input_width - output_width) / 2;
  const int y = pixel / output_width + (input_height - output_height) / 2;
  for (int c = 0; c < kChannels; ++c) {
    output[pixel * kChannels + c] =
        input[(y * input_width + x) * kChannels + c];
  }
}

__global__ void AdvCvtColorKernel(const std::uint8_t *input,
                                  std::uint8_t *output, int pixels) {
  const int pixel = blockIdx.x * blockDim.x + threadIdx.x;
  if (pixel >= pixels)
    return;
  const int i = pixel * kChannels;
  const float b = input[i];
  const float g = input[i + 1];
  const float r = input[i + 2];
  const float y = 0.2126f * r + 0.7152f * g + 0.0722f * b;
  const float u = (b - y) * 0.5389f + 128.0f;
  const float v = (r - y) * 0.6350f + 128.0f;
  const float yy = ToByte(y);
  const float uu = ToByte(u) - 128.0f;
  const float vv = ToByte(v) - 128.0f;
  output[i + 2] = ToByte(yy + 1.5748f * vv);
  output[i + 1] = ToByte(yy - 0.187324f * uu - 0.468124f * vv);
  output[i] = ToByte(yy + 1.8556f * uu);
}

__global__ void CvtColorKernel(const std::uint8_t *input, std::uint8_t *output,
                               int pixels) {
  const int pixel = blockIdx.x * blockDim.x + threadIdx.x;
  if (pixel >= pixels)
    return;
  const int i = pixel * kChannels;
  output[i] = input[i + 2];
  output[i + 1] = input[i + 1];
  output[i + 2] = input[i];
}

__global__ void ReformatKernel(const std::uint8_t *input, std::uint8_t *output,
                               int pixels) {
  const int pixel = blockIdx.x * blockDim.x + threadIdx.x;
  if (pixel >= pixels)
    return;
  const int y = pixel;
  const int plane_size = pixels;
  for (int c = 0; c < kChannels; ++c) {
    const std::uint8_t nchw_value = input[y * kChannels + c];
    output[c * plane_size + y] = nchw_value;
  }
}

__global__ void ReformatBackKernel(const std::uint8_t *planar,
                                   std::uint8_t *output, int pixels) {
  const int pixel = blockIdx.x * blockDim.x + threadIdx.x;
  if (pixel >= pixels)
    return;
  for (int c = 0; c < kChannels; ++c) {
    output[pixel * kChannels + c] = planar[c * pixels + pixel];
  }
}

__global__ void ResizeKernel(const std::uint8_t *input, std::uint8_t *output,
                             int input_width, int input_height,
                             int output_width, int output_height, int mode) {
  const int pixel = blockIdx.x * blockDim.x + threadIdx.x;
  if (pixel < output_width * output_height) {
    ResizePixel(input, output, input_width, input_height, output_width,
                output_height, pixel, mode);
  }
}

__global__ void BorderKernel(const std::uint8_t *input, std::uint8_t *output,
                             int input_width, int input_height, int top,
                             int left, int output_width, int output_height) {
  const int pixel = blockIdx.x * blockDim.x + threadIdx.x;
  if (pixel >= output_width * output_height)
    return;
  const int x = pixel % output_width - left;
  const int y = pixel / output_width - top;
  const int i = pixel * kChannels;
  if (x >= 0 && x < input_width && y >= 0 && y < input_height) {
    const int source = (y * input_width + x) * kChannels;
    output[i] = input[source];
    output[i + 1] = input[source + 1];
    output[i + 2] = input[source + 2];
  } else {
    output[i] = 0;
    output[i + 1] = 140;
    output[i + 2] = 255;
  }
}

__global__ void CustomCropKernel(const std::uint8_t *input,
                                 std::uint8_t *output, int input_width,
                                 int crop_x, int crop_y, int output_width,
                                 int output_height) {
  const int pixel = blockIdx.x * blockDim.x + threadIdx.x;
  if (pixel >= output_width * output_height)
    return;
  const int source = ((pixel / output_width + crop_y) * input_width +
                      pixel % output_width + crop_x) *
                     kChannels;
  const int dest = pixel * kChannels;
  output[dest] = input[source];
  output[dest + 1] = input[source + 1];
  output[dest + 2] = input[source + 2];
}

__global__ void RandomResizedCropKernel(const std::uint8_t *input,
                                        std::uint8_t *output, int input_width,
                                        int input_height, int crop_x,
                                        int crop_y, int crop_width,
                                        int crop_height, int output_width,
                                        int output_height) {
  const int pixel = blockIdx.x * blockDim.x + threadIdx.x;
  if (pixel >= output_width * output_height)
    return;
  const int x = pixel % output_width;
  const int y = pixel / output_width;
  const float sx = crop_x + (x + 0.5f) * crop_width / output_width - 0.5f;
  const float sy = crop_y + (y + 0.5f) * crop_height / output_height - 0.5f;
  for (int c = 0; c < kChannels; ++c) {
    output[pixel * kChannels + c] =
        ToByte(Bilinear(input, input_width, input_height, sx, sy, c));
  }
}

__global__ void CropFlipNormalizeReformatKernel(const std::uint8_t *input,
                                                std::uint8_t *output,
                                                int input_width, int crop_x,
                                                int crop_y, int output_width,
                                                int output_height) {
  const int pixel = blockIdx.x * blockDim.x + threadIdx.x;
  if (pixel >= output_width * output_height)
    return;
  const int x = output_width - 1 - pixel % output_width + crop_x;
  const int y = pixel / output_width + crop_y;
  const int source = (y * input_width + x) * kChannels;
  const int dest = pixel * kChannels;
  const float means[kChannels] = {0.485f, 0.456f, 0.406f};
  const float stds[kChannels] = {0.229f, 0.224f, 0.225f};
  for (int c = 0; c < kChannels; ++c) {
    const float normalized = (input[source + c] / 255.0f - means[c]) / stds[c];
    output[dest + c] = ToByte((normalized * stds[c] + means[c]) * 255.0f);
  }
}

__global__ void
ResizeCropConvertReformatKernel(const std::uint8_t *input, std::uint8_t *output,
                                int input_width, int input_height,
                                int output_width, int output_height) {
  const int pixel = blockIdx.x * blockDim.x + threadIdx.x;
  if (pixel >= output_width * output_height)
    return;
  const float sx =
      (pixel % output_width + 0.5f) * input_width / output_width - 0.5f;
  const float sy =
      (pixel / output_width + 0.5f) * input_height / output_height - 0.5f;
  const int dest = pixel * kChannels;
  for (int c = 0; c < kChannels; ++c) {
    output[dest + c] =
        ToByte(Bilinear(input, input_width, input_height, sx, sy, 2 - c));
  }
}

__global__ void RemapKernel(const std::uint8_t *input, std::uint8_t *output,
                            int width, int height) {
  const int pixel = blockIdx.x * blockDim.x + threadIdx.x;
  if (pixel >= width * height)
    return;
  const int x = pixel % width;
  const int y = pixel / width;
  const float amplitude = height * 0.04f;
  const float sx = x + amplitude * sinf(6.0f * kPi * y / height);
  const float sy = y + amplitude * sinf(6.0f * kPi * x / width);
  for (int c = 0; c < kChannels; ++c) {
    output[pixel * kChannels + c] =
        ToByte(Bilinear(input, width, height, sx, sy, c));
  }
}

__global__ void RotateKernel(const std::uint8_t *input, std::uint8_t *output,
                             int width, int height, float angle_degrees) {
  const int pixel = blockIdx.x * blockDim.x + threadIdx.x;
  if (pixel >= width * height)
    return;
  const int x = pixel % width;
  const int y = pixel / width;
  const float angle = angle_degrees * kPi / 180.0f;
  const float c = cosf(angle);
  const float s = sinf(angle);
  const float cx = width / 2.0f;
  const float cy = height / 2.0f;
  const float shift_x = cx * (1.0f - c) - cy * s;
  const float shift_y = cy * (1.0f - c) + cx * s;
  const float sx = c * (x - shift_x) - s * (y - shift_y);
  const float sy = s * (x - shift_x) + c * (y - shift_y);
  for (int channel = 0; channel < kChannels; ++channel) {
    output[pixel * kChannels + channel] =
        ToByte(Bilinear(input, width, height, sx, sy, channel));
  }
}

int Blocks(int count) { return (count + kThreads - 1) / kThreads; }

} // namespace

bool ParseTransformOperator(std::string_view name,
                            TransformOperator *operation) {
  constexpr std::string_view names[] = {"center_crop",
                                        "advcvtcolor",
                                        "cvtcolor",
                                        "reformat",
                                        "resize",
                                        "hq_resize",
                                        "pillowresize",
                                        "copymakeborder",
                                        "customcrop",
                                        "random_resized_crop",
                                        "crop_flip_normalize_reformat",
                                        "resize_crop_convert_reformat",
                                        "remap",
                                        "rotate"};
  constexpr TransformOperator operators[] = {
      TransformOperator::kCenterCrop,
      TransformOperator::kAdvCvtColor,
      TransformOperator::kCvtColor,
      TransformOperator::kReformat,
      TransformOperator::kResize,
      TransformOperator::kHqResize,
      TransformOperator::kPillowResize,
      TransformOperator::kCopyMakeBorder,
      TransformOperator::kCustomCrop,
      TransformOperator::kRandomResizedCrop,
      TransformOperator::kCropFlipNormalizeReformat,
      TransformOperator::kResizeCropConvertReformat,
      TransformOperator::kRemap,
      TransformOperator::kRotate,
  };
  for (int i = 0; i < 14; ++i) {
    if (name == names[i]) {
      *operation = operators[i];
      return true;
    }
  }
  return false;
}

const char *TransformOperatorName(TransformOperator operation) {
  switch (operation) {
  case TransformOperator::kCenterCrop:
    return "center_crop";
  case TransformOperator::kAdvCvtColor:
    return "advcvtcolor";
  case TransformOperator::kCvtColor:
    return "cvtcolor";
  case TransformOperator::kReformat:
    return "reformat";
  case TransformOperator::kResize:
    return "resize";
  case TransformOperator::kHqResize:
    return "hq_resize";
  case TransformOperator::kPillowResize:
    return "pillowresize";
  case TransformOperator::kCopyMakeBorder:
    return "copymakeborder";
  case TransformOperator::kCustomCrop:
    return "customcrop";
  case TransformOperator::kRandomResizedCrop:
    return "random_resized_crop";
  case TransformOperator::kCropFlipNormalizeReformat:
    return "crop_flip_normalize_reformat";
  case TransformOperator::kResizeCropConvertReformat:
    return "resize_crop_convert_reformat";
  case TransformOperator::kRemap:
    return "remap";
  case TransformOperator::kRotate:
    return "rotate";
  }
  return "unknown";
}

cudaError_t LaunchTransform(TransformOperator operation,
                            const std::uint8_t *input, std::uint8_t *output,
                            int input_width, int input_height, int output_width,
                            int output_height, const TransformConfig &config) {
  const int output_pixels = output_width * output_height;
  const int input_pixels = input_width * input_height;
  switch (operation) {
  case TransformOperator::kCenterCrop:
    CenterCropKernel<<<Blocks(output_pixels), kThreads>>>(
        input, output, input_width, input_height, output_width, output_height);
    break;
  case TransformOperator::kAdvCvtColor:
    AdvCvtColorKernel<<<Blocks(input_pixels), kThreads>>>(input, output,
                                                          input_pixels);
    break;
  case TransformOperator::kCvtColor:
    CvtColorKernel<<<Blocks(input_pixels), kThreads>>>(input, output,
                                                       input_pixels);
    break;
  case TransformOperator::kReformat: {
    std::uint8_t *planar = nullptr;
    cudaError_t status =
        cudaMalloc(reinterpret_cast<void **>(&planar),
                   static_cast<std::size_t>(input_pixels) * kChannels);
    if (status != cudaSuccess)
      return status;
    ReformatKernel<<<Blocks(input_pixels), kThreads>>>(input, planar,
                                                       input_pixels);
    status = cudaGetLastError();
    if (status == cudaSuccess) {
      ReformatBackKernel<<<Blocks(input_pixels), kThreads>>>(planar, output,
                                                             input_pixels);
      status = cudaGetLastError();
    }
    const cudaError_t free_status = cudaFree(planar);
    return status == cudaSuccess ? free_status : status;
  }
  case TransformOperator::kResize:
    ResizeKernel<<<Blocks(output_pixels), kThreads>>>(
        input, output, input_width, input_height, output_width, output_height,
        0);
    break;
  case TransformOperator::kHqResize:
    ResizeKernel<<<Blocks(output_pixels), kThreads>>>(
        input, output, input_width, input_height, output_width, output_height,
        1);
    break;
  case TransformOperator::kPillowResize:
    ResizeKernel<<<Blocks(output_pixels), kThreads>>>(
        input, output, input_width, input_height, output_width, output_height,
        2);
    break;
  case TransformOperator::kCopyMakeBorder:
    BorderKernel<<<Blocks(output_pixels), kThreads>>>(
        input, output, input_width, input_height, 30, 60, output_width,
        output_height);
    break;
  case TransformOperator::kCustomCrop:
    CustomCropKernel<<<Blocks(output_pixels), kThreads>>>(
        input, output, input_width, config.crop_x, config.crop_y, output_width,
        output_height);
    break;
  case TransformOperator::kRandomResizedCrop:
    RandomResizedCropKernel<<<Blocks(output_pixels), kThreads>>>(
        input, output, input_width, input_height, config.crop_x, config.crop_y,
        config.crop_width, config.crop_height, output_width, output_height);
    break;
  case TransformOperator::kCropFlipNormalizeReformat:
    CropFlipNormalizeReformatKernel<<<Blocks(output_pixels), kThreads>>>(
        input, output, input_width, config.crop_x, config.crop_y, output_width,
        output_height);
    break;
  case TransformOperator::kResizeCropConvertReformat:
    ResizeCropConvertReformatKernel<<<Blocks(output_pixels), kThreads>>>(
        input, output, input_width, input_height, output_width, output_height);
    break;
  case TransformOperator::kRemap:
    RemapKernel<<<Blocks(input_pixels), kThreads>>>(input, output, input_width,
                                                    input_height);
    break;
  case TransformOperator::kRotate:
    RotateKernel<<<Blocks(input_pixels), kThreads>>>(
        input, output, input_width, input_height, config.angle_degrees);
    break;
  }
  return cudaGetLastError();
}
