#include "cv-cuda-pipeline/framework/api2/stream/landmarks_to_detection.h"

#include "cv-cuda-pipeline/framework/api2/builder.h"
#include "cv-cuda-pipeline/framework/formats/detection.pb.h"
#include "cv-cuda-pipeline/framework/formats/landmark.pb.h"

namespace mediapipe::api2::builder {

Stream<mediapipe::Detection> ConvertLandmarksToDetection(
    Stream<mediapipe::NormalizedLandmarkList> landmarks, Graph& graph) {
  auto& landmarks_to_detection =
      graph.AddNode("LandmarksToDetectionCalculator");
  landmarks.ConnectTo(landmarks_to_detection.In("NORM_LANDMARKS"));
  return landmarks_to_detection.Out("DETECTION").Cast<mediapipe::Detection>();
}

}  // namespace mediapipe::api2::builder
