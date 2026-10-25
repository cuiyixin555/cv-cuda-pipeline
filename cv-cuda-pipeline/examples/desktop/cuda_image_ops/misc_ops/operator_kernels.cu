#include "cv-cuda-pipeline/examples/desktop/cuda_image_ops/misc_ops/operator_kernels.h"

#include <cuda_runtime.h>

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <utility>

namespace {

constexpr int kThreads = 256;

void CheckCuda(cudaError_t status, const char* operation) {
  if (status != cudaSuccess) {
    throw std::runtime_error(std::string(operation) + ": " +
                             cudaGetErrorString(status));
  }
}

void CheckLaunch(const char* operation) {
  CheckCuda(cudaGetLastError(), operation);
}

__device__ __forceinline__ std::uint8_t ClampByte(float value) {
  return static_cast<std::uint8_t>(fminf(255.0f, fmaxf(0.0f, roundf(value))));
}

__device__ __forceinline__ int PixelCount(int width, int height) {
  return width * height;
}

__device__ float ReadBilinear(const std::uint8_t* src, int width, int height,
                              int channels, float x, float y, int channel) {
  if (x < 0.0f || y < 0.0f || x > width - 1.0f || y > height - 1.0f) {
    return 0.0f;
  }
  const int x0 = static_cast<int>(floorf(x));
  const int y0 = static_cast<int>(floorf(y));
  const int x1 = min(x0 + 1, width - 1);
  const int y1 = min(y0 + 1, height - 1);
  const float fx = x - x0;
  const float fy = y - y0;
  const int i00 = (y0 * width + x0) * channels + channel;
  const int i10 = (y0 * width + x1) * channels + channel;
  const int i01 = (y1 * width + x0) * channels + channel;
  const int i11 = (y1 * width + x1) * channels + channel;
  return (1 - fy) * ((1 - fx) * src[i00] + fx * src[i10]) +
         fy * ((1 - fx) * src[i01] + fx * src[i11]);
}

__global__ void BndboxKernel(const std::uint8_t* src, std::uint8_t* dst,
                             int width, int height, int channels) {
  const int pixel = blockIdx.x * blockDim.x + threadIdx.x;
  if (pixel >= PixelCount(width, height)) return;
  const int x = pixel % width;
  const int y = pixel / width;
  const int box_w[3] = {max(1, width / 3), max(1, width / 2),
                        max(1, width / 4)};
  const int box_h[3] = {max(1, height / 3), max(1, height / 3),
                        max(1, height / 4)};
  const int box_x[3] = {width / 3, width / 5, width * 2 / 3};
  const int box_y[3] = {height / 12, height / 2, height * 2 / 3};
  const std::uint8_t colors[3][3] = {{0, 80, 255}, {60, 220, 0},
                                     {255, 120, 30}};
  const int thickness = max(1, min(width, height) / 100);
  for (int c = 0; c < channels; ++c) {
    dst[pixel * channels + c] = src[pixel * channels + c];
  }
  for (int b = 0; b < 3; ++b) {
    const int left = box_x[b];
    const int top = box_y[b];
    const int right = left + box_w[b];
    const int bottom = top + box_h[b];
    const bool inside_x = x >= left && x < right;
    const bool inside_y = y >= top && y < bottom;
    if (inside_x && inside_y &&
        (x - left < thickness || right - 1 - x < thickness ||
         y - top < thickness || bottom - 1 - y < thickness)) {
      for (int c = 0; c < channels; ++c) {
        dst[pixel * channels + c] =
            colors[b][c < 3 ? c : 0];
      }
    }
  }
}

__global__ void CompositeKernel(const std::uint8_t* foreground,
                                const std::uint8_t* background,
                                const std::uint8_t* mask, std::uint8_t* dst,
                                int width, int height, int channels) {
  const int index = blockIdx.x * blockDim.x + threadIdx.x;
  const int count = PixelCount(width, height) * channels;
  if (index < count) {
    const int pixel = index / channels;
    dst[index] = mask[pixel] != 0 ? foreground[index] : background[index];
  }
}

__global__ void TvAugmentKernel(const std::uint8_t* src, std::uint8_t* dst,
                                int width, int height, int channels) {
  const int pixel = blockIdx.x * blockDim.x + threadIdx.x;
  if (pixel >= PixelCount(width, height)) return;
  const int x = pixel % width;
  const int y = pixel / width;
  const int erase_w = max(1, width / 4);
  const int erase_h = max(1, height / 4);
  const bool erased = x >= (width - erase_w) / 2 &&
                      x < (width - erase_w) / 2 + erase_w &&
                      y >= (height - erase_h) / 2 &&
                      y < (height - erase_h) / 2 + erase_h;
  const int sx = (x / 2) * 2;
  const int sy = (y / 2) * 2;
  const int sample = (min(sy, height - 1) * width + min(sx, width - 1)) *
                     channels;
  for (int c = 0; c < channels; ++c) {
    float value = erased ? (c == 0 ? 224.0f : c == 1 ? 96.0f : 32.0f)
                         : static_cast<float>(src[sample + c]);
    const float step = 4.0f;
    value = roundf(value / step) * step;
    dst[pixel * channels + c] = ClampByte(value);
  }
}

__global__ void TvColorKernel(const std::uint8_t* src, std::uint8_t* dst,
                              int width, int height, int channels) {
  const int pixel = blockIdx.x * blockDim.x + threadIdx.x;
  if (pixel >= PixelCount(width, height)) return;
  float values[4] = {0, 0, 0, 255};
  for (int c = 0; c < channels; ++c) {
    values[c] = (static_cast<float>(src[pixel * channels + c]) * 1.15f -
                 128.0f) *
                    1.2f +
                128.0f;
  }
  if (channels >= 3) {
    const float gray =
        0.114f * values[0] + 0.587f * values[1] + 0.299f * values[2];
    for (int c = 0; c < 3; ++c) {
      values[c] = gray + (values[c] - gray) * 0.8f;
    }
  }
  for (int c = 0; c < channels; ++c) {
    dst[pixel * channels + c] = ClampByte(values[c]);
  }
}

__global__ void TvComposeKernel(const std::uint8_t* src, std::uint8_t* dst,
                                int width, int height, int channels,
                                int output_width, int output_height) {
  const int pixel = blockIdx.x * blockDim.x + threadIdx.x;
  if (pixel >= PixelCount(output_width, output_height)) return;
  const int x = pixel % output_width;
  const int y = pixel / output_width;
  const float crop_w = fmaxf(1.0f, width * 0.75f);
  const float crop_h = fmaxf(1.0f, height * 0.75f);
  const float crop_x = (width - crop_w) * 0.5f;
  const float crop_y = (height - crop_h) * 0.5f;
  const float sx = crop_x + (output_width - 1 - x) * crop_w / output_width;
  const float sy = crop_y + y * crop_h / output_height;
  for (int c = 0; c < channels; ++c) {
    dst[pixel * channels + c] =
        ClampByte(ReadBilinear(src, width, height, channels, sx, sy, c));
  }
}

__global__ void TvGeometryKernel(const std::uint8_t* src, std::uint8_t* dst,
                                 int width, int height, int channels,
                                 int output_width, int output_height) {
  const int pixel = blockIdx.x * blockDim.x + threadIdx.x;
  if (pixel >= PixelCount(output_width, output_height)) return;
  const float x = static_cast<float>(pixel % output_width) -
                  (output_width - 1) * 0.5f;
  const float y = static_cast<float>(pixel / output_width) -
                  (output_height - 1) * 0.5f;
  constexpr float kAngle = 15.0f * 0.017453292519943295f;
  const float cosine = cosf(kAngle);
  const float sine = sinf(kAngle);
  const float sx =
      (cosine * x + sine * y + (output_width - 1) * 0.5f) * width /
      output_width;
  const float sy =
      (-sine * x + cosine * y + (output_height - 1) * 0.5f) * height /
      output_height;
  for (int c = 0; c < channels; ++c) {
    dst[pixel * channels + c] =
        ClampByte(ReadBilinear(src, width, height, channels, sx, sy, c));
  }
}

__global__ void TvMiscKernel(const std::uint8_t* src, std::uint8_t* dst,
                             int width, int height, int channels) {
  const int index = blockIdx.x * blockDim.x + threadIdx.x;
  const int count = PixelCount(width, height) * channels;
  if (index >= count) return;
  const int c = index % channels;
  const int pixel = index / channels;
  const int x = pixel % width;
  const int y = pixel / width;
  constexpr int weights[5] = {1, 4, 6, 4, 1};
  float sum = 0.0f;
  for (int dy = -2; dy <= 2; ++dy) {
    for (int dx = -2; dx <= 2; ++dx) {
      const int sx = min(width - 1, max(0, x + dx));
      const int sy = min(height - 1, max(0, y + dy));
      sum += weights[dx + 2] * weights[dy + 2] *
             src[(sy * width + sx) * channels + c];
    }
  }
  unsigned int state = static_cast<unsigned int>(index) * 747796405u + 2891336453u;
  float noise = 0.0f;
  for (int i = 0; i < 12; ++i) {
    state = state * 1664525u + 1013904223u;
    noise += static_cast<float>((state >> 16) & 0xffffu) / 65535.0f - 0.5f;
  }
  dst[index] = ClampByte(sum / 256.0f + noise * 7.65f);
}

__global__ void InpaintKernel(const std::uint8_t* src,
                              const std::uint8_t* mask, std::uint8_t* dst,
                              int width, int height, int channels) {
  const int pixel = blockIdx.x * blockDim.x + threadIdx.x;
  if (pixel >= PixelCount(width, height)) return;
  if (mask[pixel] == 0) {
    for (int c = 0; c < channels; ++c) {
      dst[pixel * channels + c] = src[pixel * channels + c];
    }
    return;
  }
  for (int c = 0; c < channels; ++c) {
    int sum = 0;
    int count = 0;
    for (int dy = -1; dy <= 1; ++dy) {
      for (int dx = -1; dx <= 1; ++dx) {
        if (dx == 0 && dy == 0) continue;
        const int x = pixel % width + dx;
        const int y = pixel / width + dy;
        if (x >= 0 && x < width && y >= 0 && y < height) {
          sum += src[(y * width + x) * channels + c];
          ++count;
        }
      }
    }
    dst[pixel * channels + c] =
        count == 0 ? src[pixel * channels + c]
                   : static_cast<std::uint8_t>((sum + count / 2) / count);
  }
}

__global__ void InitLabelsKernel(const std::uint8_t* src, int* labels,
                                int width, int height) {
  const int pixel = blockIdx.x * blockDim.x + threadIdx.x;
  if (pixel < PixelCount(width, height)) {
    const int gray = (29 * src[pixel * 3] + 150 * src[pixel * 3 + 1] +
                      77 * src[pixel * 3 + 2]) >>
                     8;
    labels[pixel] = gray > 128 ? pixel + 1 : 0;
  }
}

__global__ void PropagateLabelsKernel(const int* input, int* output,
                                      int width, int height) {
  const int pixel = blockIdx.x * blockDim.x + threadIdx.x;
  if (pixel >= PixelCount(width, height)) return;
  int label = input[pixel];
  if (label == 0) {
    output[pixel] = 0;
    return;
  }
  const int x = pixel % width;
  const int y = pixel / width;
  if (x > 0 && input[pixel - 1] != 0) label = min(label, input[pixel - 1]);
  if (x + 1 < width && input[pixel + 1] != 0) {
    label = min(label, input[pixel + 1]);
  }
  if (y > 0 && input[pixel - width] != 0) {
    label = min(label, input[pixel - width]);
  }
  if (y + 1 < height && input[pixel + width] != 0) {
    label = min(label, input[pixel + width]);
  }
  output[pixel] = label;
}

__global__ void ColorLabelsKernel(const int* labels, std::uint8_t* dst,
                                 int width, int height) {
  const int pixel = blockIdx.x * blockDim.x + threadIdx.x;
  if (pixel >= PixelCount(width, height)) return;
  const unsigned int label = static_cast<unsigned int>(labels[pixel]);
  if (label == 0) {
    dst[pixel * 3] = dst[pixel * 3 + 1] = dst[pixel * 3 + 2] = 0;
    return;
  }
  unsigned int value = label * 747796405u + 2891336453u;
  value = ((value >> ((value >> 28u) + 4u)) ^ value) * 277803737u;
  value = (value >> 22u) ^ value;
  dst[pixel * 3] = 64 + (value & 191u);
  dst[pixel * 3 + 1] = 64 + ((value >> 8u) & 191u);
  dst[pixel * 3 + 2] = 64 + ((value >> 16u) & 191u);
}

__device__ bool GlyphPixel(char ch, int x, int y) {
  if (x < 0 || x >= 5 || y < 0 || y >= 7) return false;
  const unsigned char* rows = nullptr;
  const unsigned char o[7] = {14, 17, 17, 17, 17, 17, 14};
  const unsigned char s[7] = {15, 16, 16, 14, 1, 1, 30};
  const unsigned char d[7] = {14, 17, 17, 17, 17, 17, 14};
  const unsigned char c[7] = {14, 17, 16, 16, 16, 17, 14};
  const unsigned char v[7] = {17, 17, 17, 17, 17, 10, 4};
  const unsigned char u[7] = {17, 17, 17, 17, 17, 17, 14};
  const unsigned char* glyph = ch == 'O' ? o : ch == 'S' ? s : ch == 'D' ? d :
                               ch == 'C' ? c : ch == 'V' ? v : u;
  rows = glyph;
  return (rows[y] & (1 << (4 - x))) != 0;
}

__global__ void OsdKernel(const std::uint8_t* src, std::uint8_t* dst,
                          int width, int height, int channels) {
  const int pixel = blockIdx.x * blockDim.x + threadIdx.x;
  if (pixel >= PixelCount(width, height)) return;
  const int x = pixel % width;
  const int y = pixel / width;
  float color[3] = {-1, -1, -1};
  const int bx = width / 8;
  const int by = height / 8;
  const int bw = max(1, width / 4);
  const int bh = max(1, height / 4);
  if (x >= bx && x < bx + bw && y >= by && y < by + bh &&
      (x - bx < 3 || bx + bw - 1 - x < 3 || y - by < 3 ||
       by + bh - 1 - y < 3)) {
    color[0] = 0;
    color[1] = 255;
    color[2] = 255;
  }
  if (abs(x * height - y * width) <= max(width, height)) {
    color[0] = 0;
    color[1] = 255;
    color[2] = 0;
  }
  const int radius = max(1, min(width, height) / 8);
  const int dx = x - width / 2;
  const int dy = y - height / 2;
  const int distance2 = dx * dx + dy * dy;
  if (abs(distance2 - radius * radius) < max(2, radius * 2)) {
    color[0] = 0;
    color[1] = 128;
    color[2] = 255;
  }
  const int tx = 8;
  const int ty = height > 40 ? height - 18 : 2;
  if (x >= tx && x < tx + 20 && y >= ty && y < ty + 7) {
    const int glyph_x = (x - tx) % 7;
    const int glyph_index = (x - tx) / 7;
    const char text[3] = {'C', 'V', 'D'};
    if (glyph_index < 3 && GlyphPixel(text[glyph_index], glyph_x, y - ty)) {
      color[0] = color[1] = color[2] = 255;
    }
  }
  for (int c = 0; c < channels; ++c) {
    const float base = src[pixel * channels + c];
    dst[pixel * channels + c] =
        ClampByte(color[c < 3 ? c : 0] < 0 ? base : color[c < 3 ? c : 0]);
  }
}

__global__ void StackKernel(const std::uint8_t* src, std::uint8_t* dst,
                            int width, int height, int channels, int count) {
  const int index = blockIdx.x * blockDim.x + threadIdx.x;
  const int output_count = PixelCount(width, height) * channels * count;
  if (index < output_count) {
    dst[index] = src[index];
  }
}

__global__ void WarpAffineKernel(const std::uint8_t* src, std::uint8_t* dst,
                                 int width, int height, int channels) {
  const int pixel = blockIdx.x * blockDim.x + threadIdx.x;
  if (pixel >= PixelCount(width, height)) return;
  const float x = static_cast<float>(pixel % width) - (width - 1) * 0.5f;
  const float y = static_cast<float>(pixel / width) - (height - 1) * 0.5f;
  constexpr float angle = 15.0f * 0.017453292519943295f;
  const float sx = cosf(angle) * x + sinf(angle) * y + (width - 1) * 0.5f -
                   20.0f;
  const float sy = -sinf(angle) * x + cosf(angle) * y + (height - 1) * 0.5f;
  for (int c = 0; c < channels; ++c) {
    dst[pixel * channels + c] =
        ClampByte(ReadBilinear(src, width, height, channels, sx, sy, c));
  }
}

__global__ void WarpPerspectiveKernel(const std::uint8_t* src,
                                      std::uint8_t* dst, int width, int height,
                                      int channels) {
  const int pixel = blockIdx.x * blockDim.x + threadIdx.x;
  if (pixel >= PixelCount(width, height)) return;
  const float x = static_cast<float>(pixel % width);
  const float y = static_cast<float>(pixel / width);
  const float denominator = 1.0f + 0.00035f * x - 0.0002f * y;
  const float sx = (1.18f * x - 0.09f * width) / denominator;
  const float sy = (1.12f * y - 0.04f * height) / denominator;
  for (int c = 0; c < channels; ++c) {
    dst[pixel * channels + c] =
        ClampByte(ReadBilinear(src, width, height, channels, sx, sy, c));
  }
}

int Blocks(int count) { return (count + kThreads - 1) / kThreads; }

}  // namespace

void LaunchBndbox(const std::uint8_t* src, std::uint8_t* dst, int width,
                  int height, int channels) {
  BndboxKernel<<<Blocks(width * height), kThreads>>>(src, dst, width, height,
                                                      channels);
  CheckLaunch("Launch bndbox kernel");
}

void LaunchComposite(const std::uint8_t* foreground,
                     const std::uint8_t* background,
                     const std::uint8_t* mask, std::uint8_t* dst, int width,
                     int height, int channels) {
  CompositeKernel<<<Blocks(width * height * channels), kThreads>>>(
      foreground, background, mask, dst, width, height, channels);
  CheckLaunch("Launch composite kernel");
}

void LaunchTvAugment(const std::uint8_t* src, std::uint8_t* dst, int width,
                     int height, int channels) {
  TvAugmentKernel<<<Blocks(width * height), kThreads>>>(src, dst, width,
                                                        height, channels);
  CheckLaunch("Launch tv_augment kernel");
}

void LaunchTvColor(const std::uint8_t* src, std::uint8_t* dst, int width,
                   int height, int channels) {
  TvColorKernel<<<Blocks(width * height), kThreads>>>(src, dst, width, height,
                                                      channels);
  CheckLaunch("Launch tv_color kernel");
}

void LaunchTvCompose(const std::uint8_t* src, std::uint8_t* dst, int width,
                     int height, int channels, int output_width,
                     int output_height) {
  TvComposeKernel<<<Blocks(output_width * output_height), kThreads>>>(
      src, dst, width, height, channels, output_width, output_height);
  CheckLaunch("Launch tv_compose kernel");
}

void LaunchTvGeometry(const std::uint8_t* src, std::uint8_t* dst, int width,
                      int height, int channels, int output_width,
                      int output_height) {
  TvGeometryKernel<<<Blocks(output_width * output_height), kThreads>>>(
      src, dst, width, height, channels, output_width, output_height);
  CheckLaunch("Launch tv_geometry kernel");
}

void LaunchTvMisc(const std::uint8_t* src, std::uint8_t* dst, int width,
                  int height, int channels) {
  TvMiscKernel<<<Blocks(width * height * channels), kThreads>>>(
      src, dst, width, height, channels);
  CheckLaunch("Launch tv_misc kernel");
}

void LaunchInpaint(const std::uint8_t* src, const std::uint8_t* mask,
                   std::uint8_t* dst, int width, int height, int channels) {
  std::uint8_t* first = nullptr;
  std::uint8_t* second = nullptr;
  const std::size_t bytes = static_cast<std::size_t>(width) * height * channels;
  CheckCuda(cudaMalloc(reinterpret_cast<void**>(&first), bytes),
            "Allocate inpaint workspace");
  try {
    CheckCuda(cudaMalloc(reinterpret_cast<void**>(&second), bytes),
              "Allocate inpaint workspace");
    CheckCuda(cudaMemcpy(first, src, bytes, cudaMemcpyDeviceToDevice),
              "Initialize inpaint workspace");
    const int blocks = Blocks(width * height);
    for (int iteration = 0; iteration < 8; ++iteration) {
      InpaintKernel<<<blocks, kThreads>>>(first, mask, second, width, height,
                                          channels);
      CheckLaunch("Launch inpaint kernel");
      std::swap(first, second);
    }
    CheckCuda(cudaMemcpy(dst, first, bytes, cudaMemcpyDeviceToDevice),
              "Copy inpaint output");
  } catch (...) {
    cudaFree(second);
    cudaFree(first);
    throw;
  }
  CheckCuda(cudaFree(second), "Free inpaint workspace");
  CheckCuda(cudaFree(first), "Free inpaint workspace");
}

void LaunchLabel(const std::uint8_t* src, std::uint8_t* dst, int width,
                 int height) {
  int* first = nullptr;
  int* second = nullptr;
  const std::size_t bytes = static_cast<std::size_t>(width) * height *
                            sizeof(int);
  CheckCuda(cudaMalloc(reinterpret_cast<void**>(&first), bytes),
            "Allocate label workspace");
  try {
    CheckCuda(cudaMalloc(reinterpret_cast<void**>(&second), bytes),
              "Allocate label workspace");
    const int blocks = Blocks(width * height);
    InitLabelsKernel<<<blocks, kThreads>>>(src, first, width, height);
    CheckLaunch("Launch label initialization kernel");
    for (int iteration = 0; iteration < 128; ++iteration) {
      PropagateLabelsKernel<<<blocks, kThreads>>>(first, second, width, height);
      CheckLaunch("Launch connected-component propagation kernel");
      std::swap(first, second);
    }
    ColorLabelsKernel<<<blocks, kThreads>>>(first, dst, width, height);
    CheckLaunch("Launch label coloring kernel");
  } catch (...) {
    cudaFree(second);
    cudaFree(first);
    throw;
  }
  CheckCuda(cudaFree(second), "Free label workspace");
  CheckCuda(cudaFree(first), "Free label workspace");
}

void LaunchOsd(const std::uint8_t* src, std::uint8_t* dst, int width,
               int height, int channels) {
  OsdKernel<<<Blocks(width * height), kThreads>>>(src, dst, width, height,
                                                  channels);
  CheckLaunch("Launch osd kernel");
}

void LaunchStack(const std::uint8_t* src, std::uint8_t* dst, int width,
                 int height, int channels, int count) {
  StackKernel<<<Blocks(width * height * channels * count), kThreads>>>(
      src, dst, width, height, channels, count);
  CheckLaunch("Launch stack kernel");
}

void LaunchWarpAffine(const std::uint8_t* src, std::uint8_t* dst, int width,
                      int height, int channels) {
  WarpAffineKernel<<<Blocks(width * height), kThreads>>>(src, dst, width,
                                                         height, channels);
  CheckLaunch("Launch warp_affine kernel");
}

void LaunchWarpPerspective(const std::uint8_t* src, std::uint8_t* dst,
                           int width, int height, int channels) {
  WarpPerspectiveKernel<<<Blocks(width * height), kThreads>>>(
      src, dst, width, height, channels);
  CheckLaunch("Launch warp_perspective kernel");
}
