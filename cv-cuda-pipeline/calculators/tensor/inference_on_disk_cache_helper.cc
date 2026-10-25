#include "cv-cuda-pipeline/calculators/tensor/inference_on_disk_cache_helper.h"

#include <cstdint>
#include <string>
#include <utility>
#include <vector>

#include "absl/log/absl_log.h"
#include "absl/status/status.h"
#include "absl/strings/string_view.h"
#include "cv-cuda-pipeline/framework/deps/file_path.h"
#include "cv-cuda-pipeline/framework/port/file_helpers.h"
#include "cv-cuda-pipeline/framework/port/status_macros.h"
#include "cv-cuda-pipeline/util/tflite/tflite_gpu_runner.h"

namespace mediapipe::api2 {

absl::Status InferenceOnDiskCacheHelper::Init(
    const mediapipe::InferenceCalculatorOptions::Delegate::Gpu&
        gpu_delegate_options) {
  use_serialized_model_ = gpu_delegate_options.has_serialized_model_dir() &&
                          gpu_delegate_options.has_model_token();

  if (use_serialized_model_) {
    serialized_model_path_ =
        mediapipe::file::JoinPath(gpu_delegate_options.serialized_model_dir(),
                                  gpu_delegate_options.model_token());
  }
  cache_writing_behavior_ = gpu_delegate_options.has_cache_writing_behavior()
                                ? gpu_delegate_options.cache_writing_behavior()
                                : mediapipe::InferenceCalculatorOptions::
                                      Delegate::Gpu::WRITE_OR_ERROR;
  return absl::OkStatus();
}

absl::Status InferenceOnDiskCacheHelper::SaveGpuCachesBasedOnBehavior(
    tflite::gpu::TFLiteGPURunner& gpu_runner) const {
  switch (cache_writing_behavior_) {
    case mediapipe::InferenceCalculatorOptions::Delegate::Gpu::NO_WRITE:
      return absl::OkStatus();
    case mediapipe::InferenceCalculatorOptions::Delegate::Gpu::TRY_WRITE: {
      auto status = SaveGpuCaches(gpu_runner);
      if (!status.ok()) {
        ABSL_LOG_FIRST_N(WARNING, 1) << "Failed to save gpu caches: " << status;
      }
      return absl::OkStatus();
    }
    case mediapipe::InferenceCalculatorOptions::Delegate::Gpu::WRITE_OR_ERROR:
      return SaveGpuCaches(gpu_runner);
    default:
      ABSL_LOG_FIRST_N(ERROR, 1)
          << "Unknown cache writing behavior: "
          << static_cast<uint32_t>(cache_writing_behavior_);
      return absl::InvalidArgumentError("Unknown cache writing behavior.");
  }
}

absl::Status InferenceOnDiskCacheHelper::SaveGpuCaches(
    tflite::gpu::TFLiteGPURunner& gpu_runner) const {
  if (use_serialized_model_ && gpu_runner.CanGenerateSerializedModel()) {
    // Save serialized model file.
    ABSL_ASSIGN_OR_RETURN(std::vector<uint8_t> serialized_model_vec,
                          gpu_runner.GetSerializedModel());
    absl::string_view serialized_model(
        reinterpret_cast<char*>(serialized_model_vec.data()),
        serialized_model_vec.size());
    ABSL_RETURN_IF_ERROR(
        mediapipe::file::SetContents(serialized_model_path_, serialized_model));
  }
  return absl::OkStatus();
}

absl::Status InferenceOnDiskCacheHelper::ReadGpuCaches(
    tflite::gpu::TFLiteGPURunner& gpu_runner) const {
  if (use_serialized_model_ &&
      mediapipe::file::Exists(serialized_model_path_).ok()) {
    // Load serialized model file.
    std::string serialized_model_str;
    ABSL_RETURN_IF_ERROR(
        file::GetContents(serialized_model_path_, &serialized_model_str));
    std::vector<uint8_t> serialized_model_vec(serialized_model_str.begin(),
                                              serialized_model_str.end());
    gpu_runner.SetSerializedModel(std::move(serialized_model_vec));
  }
  return absl::OkStatus();
}

}  // namespace mediapipe::api2
