#ifndef CV_CUDA_PIPELINE_EXAMPLES_DESKTOP_CUDA_IMAGE_OPS_FILTER_OPS_FILTER_KERNELS_H_
#define CV_CUDA_PIPELINE_EXAMPLES_DESKTOP_CUDA_IMAGE_OPS_FILTER_OPS_FILTER_KERNELS_H_

#include <cuda_runtime_api.h>

#include <cstdint>
#include <string>

cudaError_t RunFilterOperator(const std::string& operation,
                              const std::uint8_t* input,
                              std::uint8_t* output, int width, int height,
                              int channels);

#endif  // CV_CUDA_PIPELINE_EXAMPLES_DESKTOP_CUDA_IMAGE_OPS_FILTER_OPS_FILTER_KERNELS_H_
