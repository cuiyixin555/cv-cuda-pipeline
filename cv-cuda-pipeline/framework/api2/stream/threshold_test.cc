#include "cv-cuda-pipeline/framework/api2/stream/threshold.h"

#include "cv-cuda-pipeline/framework/api2/builder.h"
#include "cv-cuda-pipeline/framework/calculator_framework.h"
#include "cv-cuda-pipeline/framework/port/gmock.h"
#include "cv-cuda-pipeline/framework/port/gtest.h"
#include "cv-cuda-pipeline/framework/port/parse_text_proto.h"
#include "cv-cuda-pipeline/framework/port/status_matchers.h"

namespace mediapipe::api2::builder {
namespace {

TEST(ThresholdTest, IsOverThresholdTest) {
  mediapipe::api2::builder::Graph graph;

  Stream<float> score = graph.In("SCORE").Cast<float>();
  Stream<bool> flag = IsOverThreshold(score, /*threshold=*/0.5f, graph);
  flag.SetName("flag");

  EXPECT_THAT(
      graph.GetConfig(),
      EqualsProto(mediapipe::ParseTextProtoOrDie<CalculatorGraphConfig>(R"pb(
        node {
          calculator: "ThresholdingCalculator"
          input_stream: "FLOAT:__stream_0"
          output_stream: "FLAG:flag"
          options {
            [mediapipe.ThresholdingCalculatorOptions.ext] { threshold: 0.5 }
          }
        }
        input_stream: "SCORE:__stream_0"
      )pb")));

  CalculatorGraph calcualtor_graph;
  MP_EXPECT_OK(calcualtor_graph.Initialize(graph.GetConfig()));
}

}  // namespace
}  // namespace mediapipe::api2::builder
