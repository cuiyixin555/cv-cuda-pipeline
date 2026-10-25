# Copyright 2020 The MediaPipe Authors.
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

# BUILD:
# bazel build -c opt --define MEDIAPIPE_DISABLE_GPU=1 \
#   //cv-cuda-pipeline/examples/desktop/pose_tracking:pose_tracking_cpu
# RUN:
# bazel-bin/cv-cuda-pipeline/examples/desktop/pose_tracking/pose_tracking_cpu \
#   --calculator_graph_config_file=<.../pose_tracking_cpu.pbtxt> \
#   --input_video_path=<input video path> \
#   --output_video_path=<output video path>
cc_binary(
    name = "pose_tracking_cpu",
    data = [
        "//cv-cuda-pipeline/modules/pose_detection:pose_detection.tflite",
        "//cv-cuda-pipeline/modules/pose_landmark:pose_landmark_full.tflite",
    ],
    deps = [
        "//cv-cuda-pipeline/examples/desktop:demo_run_graph_main",
        "//cv-cuda-pipeline/graphs/pose_tracking:pose_tracking_cpu_deps",
    ],
)

# Linux only
cc_binary(
    name = "pose_tracking_gpu",
    data = [
        "//cv-cuda-pipeline/modules/pose_detection:pose_detection.tflite",
        "//cv-cuda-pipeline/modules/pose_landmark:pose_landmark_full.tflite",
    ],
    deps = [
        "//cv-cuda-pipeline/examples/desktop:demo_run_graph_main_gpu",
        "//cv-cuda-pipeline/graphs/pose_tracking:pose_tracking_gpu_deps",
    ],
)
