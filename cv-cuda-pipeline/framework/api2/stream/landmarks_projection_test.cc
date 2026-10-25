#include "cv-cuda-pipeline/framework/api2/stream/landmarks_projection.h"

#include <array>

#include "cv-cuda-pipeline/framework/api2/builder.h"
#include "cv-cuda-pipeline/framework/calculator_framework.h"
#include "cv-cuda-pipeline/framework/formats/landmark.pb.h"
#include "cv-cuda-pipeline/framework/port/gmock.h"
#include "cv-cuda-pipeline/framework/port/gtest.h"
#include "cv-cuda-pipeline/framework/port/parse_text_proto.h"
#include "cv-cuda-pipeline/framework/port/status_matchers.h"

namespace mediapipe::api2::builder {
namespace {

TEST(LandmarksProjection, ProjectLandmarks) {
  mediapipe::api2::builder::Graph graph;

  Stream<NormalizedLandmarkList> landmarks =
      graph.In("NORM_LANDMARKS").Cast<NormalizedLandmarkList>();
  Stream<std::array<float, 16>> projection_matrix =
      graph.In("PROJECTION_MATRIX").Cast<std::array<float, 16>>();
  Stream<NormalizedLandmarkList> result =
      ProjectLandmarks(landmarks, projection_matrix, graph);
  result.SetName("landmarks_value");

  EXPECT_THAT(
      graph.GetConfig(),
      EqualsProto(mediapipe::ParseTextProtoOrDie<CalculatorGraphConfig>(R"pb(
        node {
          calculator: "LandmarkProjectionCalculator"
          input_stream: "NORM_LANDMARKS:__stream_0"
          input_stream: "PROJECTION_MATRIX:__stream_1"
          output_stream: "NORM_LANDMARKS:landmarks_value"
        }
        input_stream: "NORM_LANDMARKS:__stream_0"
        input_stream: "PROJECTION_MATRIX:__stream_1"
      )pb")));

  CalculatorGraph calcualtor_graph;
  MP_EXPECT_OK(calcualtor_graph.Initialize(graph.GetConfig()));
}

}  // namespace
}  // namespace mediapipe::api2::builder
