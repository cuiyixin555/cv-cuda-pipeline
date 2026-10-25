#ifndef CV_CUDA_PIPELINE_EXAMPLES_DESKTOP_CUDA_IMAGE_OPS_IMAGE_KERNELS_H_
#define CV_CUDA_PIPELINE_EXAMPLES_DESKTOP_CUDA_IMAGE_OPS_IMAGE_KERNELS_H_

#include <cstdint>

void LaunchBrightnessContrast(const std::uint8_t* src, std::uint8_t* dst,
                              int width, int height);
void LaunchChannelReorder(const std::uint8_t* src, std::uint8_t* dst,
                          int width, int height);
void LaunchColorTwist(const std::uint8_t* src, std::uint8_t* dst,
                      int width, int height);
void LaunchConvertToFloat(const std::uint8_t* src, float* dst, int width,
                          int height);
void LaunchConvertToUint8(const float* src, std::uint8_t* dst, int width,
                          int height);
void LaunchFlip(const std::uint8_t* src, std::uint8_t* dst, int width,
                int height);
void LaunchGammaContrast(const std::uint8_t* src, std::uint8_t* dst,
                         int width, int height);
void LaunchThreshold(const std::uint8_t* src, std::uint8_t* dst, int width,
                     int height);

#endif  // CV_CUDA_PIPELINE_EXAMPLES_DESKTOP_CUDA_IMAGE_OPS_IMAGE_KERNELS_H_
