#ifndef CV_CUDA_PIPELINE_EXAMPLES_DESKTOP_CUDA_IMAGE_OPS_TRANSFORM_OPS_TRANSFORM_KERNELS_H_
#define CV_CUDA_PIPELINE_EXAMPLES_DESKTOP_CUDA_IMAGE_OPS_TRANSFORM_OPS_TRANSFORM_KERNELS_H_

#include <cuda_runtime_api.h>

#include <cstdint>
#include <string_view>

enum class TransformOperator {
  kCenterCrop,
  kAdvCvtColor,
  kCvtColor,
  kReformat,
  kResize,
  kHqResize,
  kPillowResize,
  kCopyMakeBorder,
  kCustomCrop,
  kRandomResizedCrop,
  kCropFlipNormalizeReformat,
  kResizeCropConvertReformat,
  kRemap,
  kRotate,
};

struct TransformConfig {
  int crop_x = 0;
  int crop_y = 0;
  int crop_width = 0;
  int crop_height = 0;
  float angle_degrees = 45.0f;
};

bool ParseTransformOperator(std::string_view name,
                            TransformOperator *operation);
const char *TransformOperatorName(TransformOperator operation);

cudaError_t LaunchTransform(TransformOperator operation,
                            const std::uint8_t *input, std::uint8_t *output,
                            int input_width, int input_height, int output_width,
                            int output_height, const TransformConfig &config);

#endif // CV_CUDA_PIPELINE_EXAMPLES_DESKTOP_CUDA_IMAGE_OPS_TRANSFORM_OPS_TRANSFORM_KERNELS_H_
