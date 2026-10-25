#include "cv-cuda-pipeline/gpu/gpu_origin_utils.h"

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_format.h"

namespace mediapipe {

absl::StatusOr<bool> IsGpuOriginAtBottom(mediapipe::GpuOrigin::Mode origin) {
  switch (origin) {
    case mediapipe::GpuOrigin::TOP_LEFT:
      return false;
    case mediapipe::GpuOrigin::DEFAULT:
    case mediapipe::GpuOrigin::CONVENTIONAL:
      // OpenGL uses a bottom-left origin.
      return true;
    default:
      return absl::InvalidArgumentError(
          absl::StrFormat("Unhandled GPU origin %i", origin));
  }
}

}  // namespace mediapipe
