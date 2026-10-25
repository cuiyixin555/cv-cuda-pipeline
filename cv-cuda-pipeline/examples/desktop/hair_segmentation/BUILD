# Copyright 2019 The MediaPipe Authors.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

load("@rules_cc//cc:cc_binary.bzl", "cc_binary")

licenses(["notice"])

# Linux only
cc_binary(
    name = "hair_segmentation_gpu",
    data = ["//cv-cuda-pipeline/models:hair_segmentation.tflite"],
    deps = [
        "//cv-cuda-pipeline/examples/desktop:demo_run_graph_main_gpu",
        "//cv-cuda-pipeline/graphs/hair_segmentation:mobile_calculators",
    ],
)

# BUILD:
# bazel build -c opt --define MEDIAPIPE_DISABLE_GPU=1 \
#   //cv-cuda-pipeline/examples/desktop/hair_segmentation:hair_segmentation_cpu
# RUN:
# bazel-bin/cv-cuda-pipeline/examples/desktop/hair_segmentation/hair_segmentation_cpu \
#   --calculator_graph_config_file=<.../hair_segmentation_desktop_live.pbtxt> \
#   --input_video_path=<input video path> \
#   --output_video_path=<output video path>
cc_binary(
    name = "hair_segmentation_cpu",
    data = ["//cv-cuda-pipeline/models:hair_segmentation.tflite"],
    deps = [
        "//cv-cuda-pipeline/examples/desktop:demo_run_graph_main",
    ] + select({
        "//cv-cuda-pipeline/gpu:disable_gpu": [
            "//cv-cuda-pipeline/graphs/hair_segmentation:desktop_calculators",
        ],
        "//conditions:default": [
            "//cv-cuda-pipeline/graphs/hair_segmentation:mobile_calculators",
        ],
    }),
)
