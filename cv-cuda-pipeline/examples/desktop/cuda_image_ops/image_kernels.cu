#include "cv-cuda-pipeline/examples/desktop/cuda_image_ops/image_kernels.h"

#include <cuda_runtime.h>

#include <cmath>
#include <cstdint>

namespace {

constexpr int kChannels = 3;
constexpr int kThreadsPerBlock = 256;

__device__ std::uint8_t ClampRoundToUint8(float value) {
  const int rounded = __float2int_rn(value);
  return static_cast<std::uint8_t>(max(0, min(255, rounded)));
}

__global__ void BrightnessContrastKernel(const std::uint8_t* src,
                                         std::uint8_t* dst, int count) {
  const int index = blockIdx.x * blockDim.x + threadIdx.x;
  if (index >= count) {
    return;
  }
  const float value = static_cast<float>(src[index]);
  dst[index] = ClampRoundToUint8(1.5f * (1.4f * (value - 127.0f) + 127.0f) +
                                 20.0f);
}

__global__ void ChannelReorderKernel(const std::uint8_t* src,
                                     std::uint8_t* dst, int pixels) {
  const int pixel = blockIdx.x * blockDim.x + threadIdx.x;
  if (pixel >= pixels) {
    return;
  }
  const int offset = pixel * kChannels;
  dst[offset] = src[offset + 2];
  dst[offset + 1] = src[offset + 1];
  dst[offset + 2] = src[offset];
}

__global__ void ColorTwistKernel(const std::uint8_t* src, std::uint8_t* dst,
                                 int pixels) {
  const int pixel = blockIdx.x * blockDim.x + threadIdx.x;
  if (pixel >= pixels) {
    return;
  }
  const int offset = pixel * kChannels;
  dst[offset] = ClampRoundToUint8(0.8f * src[offset] - 10.0f);
  dst[offset + 1] = src[offset + 1];
  dst[offset + 2] = ClampRoundToUint8(1.2f * src[offset + 2] + 10.0f);
}

__global__ void ConvertToFloatKernel(const std::uint8_t* src, float* dst,
                                     int count) {
  const int index = blockIdx.x * blockDim.x + threadIdx.x;
  if (index < count) {
    dst[index] = static_cast<float>(src[index]) / 255.0f;
  }
}

__global__ void ConvertToUint8Kernel(const float* src, std::uint8_t* dst,
                                     int count) {
  const int index = blockIdx.x * blockDim.x + threadIdx.x;
  if (index < count) {
    dst[index] = ClampRoundToUint8(src[index] * 255.0f);
  }
}

__global__ void FlipKernel(const std::uint8_t* src, std::uint8_t* dst,
                           int width, int height) {
  const int pixel = blockIdx.x * blockDim.x + threadIdx.x;
  const int pixels = width * height;
  if (pixel >= pixels) {
    return;
  }
  const int y = pixel / width;
  const int x = pixel - y * width;
  const int src_offset = pixel * kChannels;
  const int dst_offset = (y * width + (width - 1 - x)) * kChannels;
  dst[dst_offset] = src[src_offset];
  dst[dst_offset + 1] = src[src_offset + 1];
  dst[dst_offset + 2] = src[src_offset + 2];
}

__global__ void GammaContrastKernel(const std::uint8_t* src,
                                    std::uint8_t* dst, int count) {
  const int index = blockIdx.x * blockDim.x + threadIdx.x;
  if (index < count) {
    const float normalized = static_cast<float>(src[index]) / 255.0f;
    dst[index] = ClampRoundToUint8(powf(normalized, 2.2f) * 255.0f);
  }
}

__global__ void ThresholdKernel(const std::uint8_t* src, std::uint8_t* dst,
                                int count) {
  const int index = blockIdx.x * blockDim.x + threadIdx.x;
  if (index < count) {
    dst[index] = src[index] > 128 ? 255 : 0;
  }
}

int BlocksFor(int count) {
  return (count + kThreadsPerBlock - 1) / kThreadsPerBlock;
}

}  // namespace

void LaunchBrightnessContrast(const std::uint8_t* src, std::uint8_t* dst,
                              int width, int height) {
  BrightnessContrastKernel<<<BlocksFor(width * height * kChannels),
                             kThreadsPerBlock>>>(src, dst,
                                                 width * height * kChannels);
}

void LaunchChannelReorder(const std::uint8_t* src, std::uint8_t* dst,
                          int width, int height) {
  ChannelReorderKernel<<<BlocksFor(width * height), kThreadsPerBlock>>>(
      src, dst, width * height);
}

void LaunchColorTwist(const std::uint8_t* src, std::uint8_t* dst, int width,
                      int height) {
  ColorTwistKernel<<<BlocksFor(width * height), kThreadsPerBlock>>>(
      src, dst, width * height);
}

void LaunchConvertToFloat(const std::uint8_t* src, float* dst, int width,
                          int height) {
  ConvertToFloatKernel<<<BlocksFor(width * height * kChannels),
                         kThreadsPerBlock>>>(src, dst,
                                             width * height * kChannels);
}

void LaunchConvertToUint8(const float* src, std::uint8_t* dst, int width,
                          int height) {
  ConvertToUint8Kernel<<<BlocksFor(width * height * kChannels),
                         kThreadsPerBlock>>>(src, dst,
                                             width * height * kChannels);
}

void LaunchFlip(const std::uint8_t* src, std::uint8_t* dst, int width,
                int height) {
  FlipKernel<<<BlocksFor(width * height), kThreadsPerBlock>>>(src, dst, width,
                                                              height);
}

void LaunchGammaContrast(const std::uint8_t* src, std::uint8_t* dst, int width,
                         int height) {
  GammaContrastKernel<<<BlocksFor(width * height * kChannels),
                        kThreadsPerBlock>>>(src, dst,
                                            width * height * kChannels);
}

void LaunchThreshold(const std::uint8_t* src, std::uint8_t* dst, int width,
                     int height) {
  ThresholdKernel<<<BlocksFor(width * height * kChannels), kThreadsPerBlock>>>(
      src, dst, width * height * kChannels);
}
