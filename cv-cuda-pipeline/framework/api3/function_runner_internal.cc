#include "cv-cuda-pipeline/framework/api3/function_runner_internal.h"

#include <utility>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "cv-cuda-pipeline/framework/calculator_framework.h"
#include "cv-cuda-pipeline/framework/output_stream_poller.h"

namespace mediapipe::api3 {

absl::StatusOr<mediapipe::Packet> GetOutputPacket(OutputStreamPoller& poller,
                                                  CalculatorGraph& graph) {
  mediapipe::Packet packet;
  if (!poller.Next(&packet)) {
    if (graph.HasError()) {
      absl::Status status;
      (void)graph.GetCombinedErrors("Failed to poll the output", &status);
      return status;
    } else {
      return absl::InternalError("Failled to poll the output.");
    }
  }

  // NOTE: currently supporting timestamp-less execution only.
  return std::move(packet).At(Timestamp::Unset());
}

}  // namespace mediapipe::api3
