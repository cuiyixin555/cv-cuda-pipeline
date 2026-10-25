#include "cv-cuda-pipeline/examples/desktop/cuda_image_ops/filter_ops/filter_kernels.h"

#include <cuda_runtime.h>

#include <cmath>
#include <cstdint>
#include <cstring>
#include <string>

namespace {

constexpr int kThreads = 256;
constexpr int kChannels = 3;

__device__ __forceinline__ int Clamp(int value, int low, int high) {
  return max(low, min(high, value));
}

__device__ __forceinline__ int Reflect101(int value, int size) {
  if (size <= 1) return 0;
  while (value < 0 || value >= size) {
    value = value < 0 ? -value : 2 * size - value - 2;
  }
  return value;
}

__device__ __forceinline__ int Reflect(int value, int size) {
  if (size <= 1) return 0;
  while (value < 0 || value >= size) {
    value = value < 0 ? -value - 1 : 2 * size - value - 1;
  }
  return value;
}

__device__ __forceinline__ int Pixel(const std::uint8_t* src, int x, int y,
                                     int channel, int width, int height,
                                     int border) {
  x = border == 0 ? Reflect101(x, width)
                  : (border == 1 ? Reflect(x, width)
                                 : Clamp(x, 0, width - 1));
  y = border == 0 ? Reflect101(y, height)
                  : (border == 1 ? Reflect(y, height)
                                 : Clamp(y, 0, height - 1));
  return src[(y * width + x) * kChannels + channel];
}

__device__ __forceinline__ std::uint8_t Byte(float value) {
  return static_cast<std::uint8_t>(
      Clamp(static_cast<int>(nearbyintf(value)), 0, 255));
}

__global__ void BlurKernel(const std::uint8_t* src, std::uint8_t* dst,
                           int width, int height, int kind) {
  const int pixel = blockIdx.x * blockDim.x + threadIdx.x;
  const int count = width * height;
  if (pixel >= count) return;
  const int x = pixel % width;
  const int y = pixel / width;
  int radius = kind == 0 ? 3 : (kind == 1 ? 4 : 2);
  int border = kind == 0 ? 0 : 1;
  int box_size = 0;
  if (kind == 2) {
    const int x0[3] = {width / 8, width / 4, width * 3 / 4};
    const int y0[3] = {height / 8, height / 4, height * 3 / 4};
    const int bw[3] = {width / 5, width / 2, width / 6};
    const int bh[3] = {height / 5, height / 2, height / 6};
    const int sizes[3] = {21, 45, 11};
    for (int i = 0; i < 3; ++i) {
      if (x >= x0[i] && x < x0[i] + bw[i] && y >= y0[i] &&
          y < y0[i] + bh[i]) {
        box_size = sizes[i];
      }
    }
    if (box_size == 0) {
      for (int c = 0; c < kChannels; ++c) dst[pixel * kChannels + c] = src[pixel * kChannels + c];
      return;
    }
    radius = box_size / 2;
  }
  for (int c = 0; c < kChannels; ++c) {
    float sum = 0.0f;
    float weight_sum = 0.0f;
    for (int dy = -radius; dy <= radius; ++dy) {
      for (int dx = -radius; dx <= radius; ++dx) {
        float weight = 1.0f;
        if (kind == 1) {
          const float sigma = 1.5f;
          weight = expf(-(dx * dx + dy * dy) / (2.0f * sigma * sigma));
        }
        sum += weight * Pixel(src, x + dx, y + dy, c, width, height, border);
        weight_sum += weight;
      }
    }
    dst[pixel * kChannels + c] = Byte(sum / weight_sum);
  }
}

__global__ void BilateralKernel(const std::uint8_t* src, std::uint8_t* dst,
                                int width, int height, bool joint) {
  const int pixel = blockIdx.x * blockDim.x + threadIdx.x;
  const int count = width * height;
  if (pixel >= count) return;
  const int x = pixel % width;
  const int y = pixel / width;
  constexpr int radius = 4;
  constexpr float sigma_color = 75.0f;
  constexpr float sigma_space = 75.0f;
  for (int c = 0; c < kChannels; ++c) {
    float sum = 0.0f;
    float total = 0.0f;
    for (int dy = -radius; dy <= radius; ++dy) {
      for (int dx = -radius; dx <= radius; ++dx) {
        const int sx = x + dx;
        const int sy = y + dy;
        const int sample = Pixel(src, sx, sy, c, width, height, 1);
        float color_distance = 0.0f;
        if (joint) {
          const float source_gray =
              0.114f * src[pixel * 3] + 0.587f * src[pixel * 3 + 1] +
              0.299f * src[pixel * 3 + 2];
          const float guide_gray =
              0.114f * Pixel(src, sx, sy, 0, width, height, 1) +
              0.587f * Pixel(src, sx, sy, 1, width, height, 1) +
              0.299f * Pixel(src, sx, sy, 2, width, height, 1);
          const float difference = guide_gray - source_gray;
          color_distance = 3.0f * difference * difference;
        } else {
          for (int gc = 0; gc < kChannels; ++gc) {
            const float difference =
                Pixel(src, sx, sy, gc, width, height, 1) -
                src[pixel * kChannels + gc];
            color_distance += difference * difference;
          }
        }
        const float spatial = static_cast<float>(dx * dx + dy * dy);
        const float weight =
            expf(-spatial / (2.0f * sigma_space * sigma_space) -
                 color_distance / (2.0f * sigma_color * sigma_color));
        sum += weight * sample;
        total += weight;
      }
    }
    dst[pixel * kChannels + c] = Byte(sum / total);
  }
}

__global__ void AdaptiveThresholdKernel(const std::uint8_t* src,
                                       std::uint8_t* dst, int width,
                                       int height) {
  const int pixel = blockIdx.x * blockDim.x + threadIdx.x;
  const int count = width * height;
  if (pixel >= count) return;
  const int x = pixel % width;
  const int y = pixel / width;
  float weighted = 0.0f;
  float weights = 0.0f;
  for (int dy = -5; dy <= 5; ++dy) {
    for (int dx = -5; dx <= 5; ++dx) {
      const float w =
          expf(-(dx * dx + dy * dy) / (2.0f * 2.3f * 2.3f));
      const int b = Pixel(src, x + dx, y + dy, 0, width, height, 2);
      const int g = Pixel(src, x + dx, y + dy, 1, width, height, 2);
      const int r = Pixel(src, x + dx, y + dy, 2, width, height, 2);
      weighted += w * (0.114f * b + 0.587f * g + 0.299f * r);
      weights += w;
    }
  }
  const float gray = nearbyintf(0.114f * src[pixel * 3] +
                                0.587f * src[pixel * 3 + 1] +
                                0.299f * src[pixel * 3 + 2]);
  const std::uint8_t value =
      gray > nearbyintf(weighted / weights) - 2.0f ? 255 : 0;
  dst[pixel * 3] = value;
  dst[pixel * 3 + 1] = value;
  dst[pixel * 3 + 2] = value;
}

__global__ void SharpenKernel(const std::uint8_t* src, std::uint8_t* dst,
                              int width, int height) {
  const int pixel = blockIdx.x * blockDim.x + threadIdx.x;
  if (pixel >= width * height) return;
  const int x = pixel % width;
  const int y = pixel / width;
  for (int c = 0; c < kChannels; ++c) {
    const int value =
        5 * Pixel(src, x, y, c, width, height, 0) -
        Pixel(src, x - 1, y, c, width, height, 0) -
        Pixel(src, x + 1, y, c, width, height, 0) -
        Pixel(src, x, y - 1, c, width, height, 0) -
        Pixel(src, x, y + 1, c, width, height, 0);
    dst[pixel * kChannels + c] = static_cast<std::uint8_t>(Clamp(value, 0, 255));
  }
}

__device__ unsigned int Hash(unsigned int value) {
  value ^= value >> 16;
  value *= 0x7feb352dU;
  value ^= value >> 15;
  value *= 0x846ca68bU;
  return value ^ (value >> 16);
}

__global__ void NoiseKernel(const std::uint8_t* src, std::uint8_t* dst,
                            int count) {
  const int pixel = blockIdx.x * blockDim.x + threadIdx.x;
  if (pixel >= count) return;
  const float u1 = (Hash(pixel * 2 + 42U) + 1.0f) / 4294967297.0f;
  const float u2 = (Hash(pixel * 2 + 43U) + 1.0f) / 4294967297.0f;
  const float noise = 25.0f * sqrtf(-2.0f * logf(u1)) *
                      cosf(6.28318530718f * u2);
  for (int c = 0; c < kChannels; ++c) {
    dst[pixel * kChannels + c] = Byte(src[pixel * kChannels + c] + noise);
  }
}

__global__ void EraseKernel(const std::uint8_t* src, std::uint8_t* dst,
                            int width, int height) {
  const int pixel = blockIdx.x * blockDim.x + threadIdx.x;
  const int count = width * height;
  if (pixel >= count) return;
  const int x = pixel % width;
  const int y = pixel / width;
  const int ax[6] = {50, 200, 350, 480, 480, 200};
  const int ay[6] = {80, 150, 300, 80, 260, 400};
  const int rw[6] = {120, 160, 100, 120, 120, 160};
  const int rh[6] = {80, 100, 120, 100, 100, 100};
  const std::uint8_t colors[6][3] = {
      {0, 0, 255}, {0, 255, 0}, {255, 0, 0},
      {255, 255, 255}, {0, 0, 0}, {0, 255, 0}};
  bool erased = false;
  for (int i = 0; i < 6; ++i) {
    if (x >= ax[i] && x < ax[i] + rw[i] && y >= ay[i] &&
        y < ay[i] + rh[i]) {
      for (int c = 0; c < kChannels; ++c) {
        if (i != 5 || c == 1) {
          dst[pixel * kChannels + c] = colors[i][c];
        } else if (c == 0 || c == 2) {
          dst[pixel * kChannels + c] = src[pixel * kChannels + c];
        }
      }
      erased = true;
    }
  }
  if (!erased) {
    for (int c = 0; c < kChannels; ++c) dst[pixel * kChannels + c] = src[pixel * kChannels + c];
  }
}

__global__ void MedianKernel(const std::uint8_t* src, std::uint8_t* dst,
                             int width, int height) {
  const int pixel = blockIdx.x * blockDim.x + threadIdx.x;
  if (pixel >= width * height) return;
  const int x = pixel % width;
  const int y = pixel / width;
  for (int c = 0; c < kChannels; ++c) {
    int values[49];
    int n = 0;
    for (int dy = -3; dy <= 3; ++dy)
      for (int dx = -3; dx <= 3; ++dx)
        values[n++] = Pixel(src, x + dx, y + dy, c, width, height, 2);
    for (int i = 1; i < 49; ++i) {
      const int v = values[i];
      int j = i - 1;
      while (j >= 0 && values[j] > v) {
        values[j + 1] = values[j];
        --j;
      }
      values[j + 1] = v;
    }
    dst[pixel * kChannels + c] = static_cast<std::uint8_t>(values[24]);
  }
}

__global__ void MorphKernel(const std::uint8_t* src, std::uint8_t* dst,
                            int width, int height, bool dilate) {
  const int pixel = blockIdx.x * blockDim.x + threadIdx.x;
  if (pixel >= width * height) return;
  const int x = pixel % width;
  const int y = pixel / width;
  for (int c = 0; c < kChannels; ++c) {
    int value = dilate ? 0 : 255;
    for (int dy = -2; dy <= 2; ++dy) {
      for (int dx = -2; dx <= 2; ++dx) {
        const int sample = Pixel(src, x + dx, y + dy, c, width, height, 2);
        value = dilate ? max(value, sample) : min(value, sample);
      }
    }
    dst[pixel * kChannels + c] = static_cast<std::uint8_t>(value);
  }
}

__global__ void LaplacianResponseKernel(const std::uint8_t* src, float* values,
                                        int width, int height) {
  const int pixel = blockIdx.x * blockDim.x + threadIdx.x;
  if (pixel >= width * height * kChannels) return;
  const int channel = pixel % kChannels;
  const int image_pixel = pixel / kChannels;
  const int x = image_pixel % width;
  const int y = image_pixel / width;
  const int center = Pixel(src, x, y, channel, width, height, 2);
  values[pixel] =
      2.0f * Pixel(src, x - 1, y - 1, channel, width, height, 2) +
      2.0f * Pixel(src, x + 1, y - 1, channel, width, height, 2) +
      2.0f * Pixel(src, x - 1, y + 1, channel, width, height, 2) +
      2.0f * Pixel(src, x + 1, y + 1, channel, width, height, 2) -
      8.0f * center;
}

__global__ void NormalizeLaplacianKernel(const float* values,
                                         std::uint8_t* dst, int count) {
  if (blockIdx.x != 0 || threadIdx.x != 0) return;
  float low = values[0];
  float high = values[0];
  for (int i = 1; i < count; ++i) {
    low = fminf(low, values[i]);
    high = fmaxf(high, values[i]);
  }
  const float range = high - low;
  for (int i = 0; i < count; ++i) {
    dst[i] = range > 0.0f ? Byte((values[i] - low) * (255.0f / range))
                         : Byte(values[i]);
  }
}

__global__ void HistogramLutKernel(const std::uint8_t* src,
                                   std::uint8_t* lut, int pixels) {
  if (blockIdx.x != 0 || threadIdx.x != 0) return;
  int hist[256] = {};
  for (int i = 0; i < pixels; ++i) {
    const int gray = static_cast<int>(nearbyintf(
        0.114f * src[i * 3] + 0.587f * src[i * 3 + 1] + 0.299f * src[i * 3 + 2]));
    ++hist[Clamp(gray, 0, 255)];
  }
  int first = 0;
  while (first < 256 && hist[first] == 0) ++first;
  int cdf = 0;
  const int base = first < 256 ? hist[first] : 0;
  for (int i = 0; i < 256; ++i) {
    cdf += hist[i];
    lut[i] = pixels > base && first < 256
                 ? Byte(255.0f * (cdf - base) / (pixels - base))
                 : static_cast<std::uint8_t>(i);
  }
}

__global__ void ApplyGrayLutKernel(const std::uint8_t* src,
                                   const std::uint8_t* lut,
                                   std::uint8_t* dst, int pixels) {
  const int pixel = blockIdx.x * blockDim.x + threadIdx.x;
  if (pixel >= pixels) return;
  const int gray = Clamp(static_cast<int>(nearbyintf(
      0.114f * src[pixel * 3] + 0.587f * src[pixel * 3 + 1] +
      0.299f * src[pixel * 3 + 2])), 0, 255);
  const std::uint8_t v = lut[gray];
  dst[pixel * 3] = v;
  dst[pixel * 3 + 1] = v;
  dst[pixel * 3 + 2] = v;
}

__global__ void ClaheTileLutKernel(const std::uint8_t* src,
                                   std::uint8_t* luts, int width, int height,
                                   int tiles_x, int tiles_y) {
  const int tile = blockIdx.x * blockDim.x + threadIdx.x;
  if (tile >= tiles_x * tiles_y) return;
  const int tx = tile % tiles_x;
  const int ty = tile / tiles_x;
  const int x0 = tx * width / tiles_x;
  const int x1 = (tx + 1) * width / tiles_x;
  const int y0 = ty * height / tiles_y;
  const int y1 = (ty + 1) * height / tiles_y;
  int hist[256] = {};
  const int area = (x1 - x0) * (y1 - y0);
  for (int y = y0; y < y1; ++y) {
    for (int x = x0; x < x1; ++x) {
      const int p = (y * width + x) * 3;
      const int gray = Clamp(static_cast<int>(nearbyintf(
          0.114f * src[p] + 0.587f * src[p + 1] + 0.299f * src[p + 2])), 0, 255);
      ++hist[gray];
    }
  }
  const int limit = max(1, static_cast<int>(2.0f * area / 256.0f));
  int excess = 0;
  for (int i = 0; i < 256; ++i) {
    if (hist[i] > limit) {
      excess += hist[i] - limit;
      hist[i] = limit;
    }
  }
  const int redistribution = excess / 256;
  const int remainder = excess - redistribution * 256;
  int cumulative = 0;
  for (int i = 0; i < 256; ++i) {
    hist[i] += redistribution + (i < remainder ? 1 : 0);
    cumulative += hist[i];
    luts[tile * 256 + i] = area ? Byte(255.0f * cumulative / area) : 0;
  }
}

__global__ void ApplyClaheKernel(const std::uint8_t* src,
                                 const std::uint8_t* luts, std::uint8_t* dst,
                                 int width, int height, int tiles_x,
                                 int tiles_y) {
  const int pixel = blockIdx.x * blockDim.x + threadIdx.x;
  if (pixel >= width * height) return;
  const int x = pixel % width;
  const int y = pixel / width;
  const float tile_x = (x + 0.5f) * tiles_x / width - 0.5f;
  const float tile_y = (y + 0.5f) * tiles_y / height - 0.5f;
  const int raw_tx = static_cast<int>(floorf(tile_x));
  const int raw_ty = static_cast<int>(floorf(tile_y));
  const int tx0 = Clamp(raw_tx, 0, tiles_x - 1);
  const int ty0 = Clamp(raw_ty, 0, tiles_y - 1);
  const int tx1 = Clamp(raw_tx + 1, 0, tiles_x - 1);
  const int ty1 = Clamp(raw_ty + 1, 0, tiles_y - 1);
  const float fx = tile_x - raw_tx;
  const float fy = tile_y - raw_ty;
  const int p = pixel * 3;
  const int gray = Clamp(static_cast<int>(nearbyintf(
      0.114f * src[p] + 0.587f * src[p + 1] + 0.299f * src[p + 2])), 0, 255);
  const float top = (1.0f - fx) * luts[(ty0 * tiles_x + tx0) * 256 + gray] +
                    fx * luts[(ty0 * tiles_x + tx1) * 256 + gray];
  const float bottom =
      (1.0f - fx) * luts[(ty1 * tiles_x + tx0) * 256 + gray] +
      fx * luts[(ty1 * tiles_x + tx1) * 256 + gray];
  const std::uint8_t v = Byte((1.0f - fy) * top + fy * bottom);
  dst[p] = v;
  dst[p + 1] = v;
  dst[p + 2] = v;
}

__global__ void NormalizeKernel(const std::uint8_t* src, std::uint8_t* dst,
                                int count) {
  const int i = blockIdx.x * blockDim.x + threadIdx.x;
  if (i < count) {
    constexpr float mean[3] = {103.53f, 116.28f, 123.675f};
    constexpr float deviation[3] = {57.375f, 57.12f, 58.395f};
    const int c = i % 3;
    dst[i] = Byte((src[i] - mean[c]) / deviation[c] * 64.0f + 128.0f);
  }
}

cudaError_t Finish() {
  cudaError_t status = cudaGetLastError();
  if (status != cudaSuccess) return status;
  return cudaDeviceSynchronize();
}

int Blocks(int count) { return (count + kThreads - 1) / kThreads; }

}  // namespace

cudaError_t RunFilterOperator(const std::string& operation,
                              const std::uint8_t* input,
                              std::uint8_t* output, int width, int height,
                              int channels) {
  if (input == nullptr || output == nullptr || width <= 0 || height <= 0 ||
      channels != kChannels) {
    return cudaErrorInvalidValue;
  }
  const int pixels = width * height;
  const int bytes = pixels * kChannels;
  if (operation == "averageblur" || operation == "gaussian" ||
      operation == "boxblur") {
    const int kind = operation == "averageblur" ? 0
                     : operation == "gaussian" ? 1
                                                 : 2;
    BlurKernel<<<Blocks(pixels), kThreads>>>(input, output, width, height, kind);
  } else if (operation == "bilateral_filter" ||
             operation == "joint_bilateral_filter") {
    BilateralKernel<<<Blocks(pixels), kThreads>>>(
        input, output, width, height,
        operation == "joint_bilateral_filter");
  } else if (operation == "adaptivethreshold") {
    AdaptiveThresholdKernel<<<Blocks(pixels), kThreads>>>(input, output, width,
                                                          height);
  } else if (operation == "conv2d") {
    SharpenKernel<<<Blocks(pixels), kThreads>>>(input, output, width, height);
  } else if (operation == "erase") {
    EraseKernel<<<Blocks(pixels), kThreads>>>(input, output, width, height);
  } else if (operation == "gaussiannoise") {
    NoiseKernel<<<Blocks(pixels), kThreads>>>(input, output, pixels);
  } else if (operation == "median_blur") {
    MedianKernel<<<Blocks(pixels), kThreads>>>(input, output, width, height);
  } else if (operation == "morphology") {
    std::uint8_t* temporary = nullptr;
    cudaError_t status =
        cudaMalloc(reinterpret_cast<void**>(&temporary), bytes);
    if (status != cudaSuccess) return status;
    MorphKernel<<<Blocks(pixels), kThreads>>>(input, temporary, width, height,
                                              true);
    status = Finish();
    if (status == cudaSuccess) {
      MorphKernel<<<Blocks(pixels), kThreads>>>(temporary, output, width,
                                                height, false);
      status = Finish();
    }
    const cudaError_t free_status = cudaFree(temporary);
    return status == cudaSuccess ? free_status : status;
  } else if (operation == "laplacian") {
    float* values = nullptr;
    cudaError_t status =
        cudaMalloc(reinterpret_cast<void**>(&values), bytes * sizeof(float));
    if (status != cudaSuccess) return status;
    LaplacianResponseKernel<<<Blocks(bytes), kThreads>>>(input, values, width,
                                                          height);
    status = Finish();
    if (status == cudaSuccess) {
      NormalizeLaplacianKernel<<<1, 1>>>(values, output, bytes);
      status = Finish();
    }
    const cudaError_t free_status = cudaFree(values);
    return status == cudaSuccess ? free_status : status;
  } else if (operation == "histogrameq") {
    std::uint8_t* lut = nullptr;
    cudaError_t status = cudaMalloc(reinterpret_cast<void**>(&lut), 256);
    if (status != cudaSuccess) return status;
    HistogramLutKernel<<<1, 1>>>(input, lut, pixels);
    status = Finish();
    if (status == cudaSuccess) {
      ApplyGrayLutKernel<<<Blocks(pixels), kThreads>>>(input, lut, output,
                                                        pixels);
      status = Finish();
    }
    const cudaError_t free_status = cudaFree(lut);
    return status == cudaSuccess ? free_status : status;
  } else if (operation == "clahe") {
    const int tiles_x = min(8, width);
    const int tiles_y = min(8, height);
    std::uint8_t* luts = nullptr;
    cudaError_t status =
        cudaMalloc(reinterpret_cast<void**>(&luts), tiles_x * tiles_y * 256);
    if (status != cudaSuccess) return status;
    ClaheTileLutKernel<<<Blocks(tiles_x * tiles_y), kThreads>>>(
        input, luts, width, height, tiles_x, tiles_y);
    status = Finish();
    if (status == cudaSuccess) {
      ApplyClaheKernel<<<Blocks(pixels), kThreads>>>(
          input, luts, output, width, height, tiles_x, tiles_y);
      status = Finish();
    }
    const cudaError_t free_status = cudaFree(luts);
    return status == cudaSuccess ? free_status : status;
  } else if (operation == "normalize") {
    NormalizeKernel<<<Blocks(bytes), kThreads>>>(input, output, bytes);
  } else {
    return cudaErrorInvalidValue;
  }
  return Finish();
}
