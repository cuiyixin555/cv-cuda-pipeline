#ifndef CV_CUDA_PIPELINE_EXAMPLES_DESKTOP_CUDA_IMAGE_OPS_MISC_OPS_OPERATOR_KERNELS_H_
#define CV_CUDA_PIPELINE_EXAMPLES_DESKTOP_CUDA_IMAGE_OPS_MISC_OPS_OPERATOR_KERNELS_H_

#include <cstdint>

void LaunchBndbox(const std::uint8_t* src, std::uint8_t* dst, int width,
                  int height, int channels);
void LaunchComposite(const std::uint8_t* foreground,
                     const std::uint8_t* background,
                     const std::uint8_t* mask, std::uint8_t* dst, int width,
                     int height, int channels);
void LaunchTvAugment(const std::uint8_t* src, std::uint8_t* dst, int width,
                     int height, int channels);
void LaunchTvColor(const std::uint8_t* src, std::uint8_t* dst, int width,
                   int height, int channels);
void LaunchTvCompose(const std::uint8_t* src, std::uint8_t* dst, int width,
                     int height, int channels, int output_width,
                     int output_height);
void LaunchTvGeometry(const std::uint8_t* src, std::uint8_t* dst, int width,
                      int height, int channels, int output_width,
                      int output_height);
void LaunchTvMisc(const std::uint8_t* src, std::uint8_t* dst, int width,
                  int height, int channels);
void LaunchInpaint(const std::uint8_t* src, const std::uint8_t* mask,
                   std::uint8_t* dst, int width, int height, int channels);
void LaunchLabel(const std::uint8_t* src, std::uint8_t* dst, int width,
                 int height);
void LaunchOsd(const std::uint8_t* src, std::uint8_t* dst, int width,
               int height, int channels);
void LaunchStack(const std::uint8_t* src, std::uint8_t* dst, int width,
                 int height, int channels, int count);
void LaunchWarpAffine(const std::uint8_t* src, std::uint8_t* dst, int width,
                      int height, int channels);
void LaunchWarpPerspective(const std::uint8_t* src, std::uint8_t* dst,
                           int width, int height, int channels);

#endif  // CV_CUDA_PIPELINE_EXAMPLES_DESKTOP_CUDA_IMAGE_OPS_MISC_OPS_OPERATOR_KERNELS_H_
