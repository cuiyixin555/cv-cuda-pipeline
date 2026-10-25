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

# BUILD:
# bazel build -c opt --define MEDIAPIPE_DISABLE_GPU=1 \
#   //cv-cuda-pipeline/examples/desktop/iris_tracking:iris_depth_from_image_desktop
# RUN:
# bazel-bin/cv-cuda-pipeline/examples/desktop/iris_tracking/iris_depth_from_image_desktop \
#   --input_image_path=<input image path> --output_image_path=<output image path>
cc_binary(
    name = "iris_depth_from_image_desktop",
    srcs = ["iris_depth_from_image_desktop.cc"],
    data = ["//cv-cuda-pipeline/modules/iris_landmark:iris_landmark.tflite"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/formats:image_frame",
        "//cv-cuda-pipeline/framework/formats:image_frame_opencv",
        "//cv-cuda-pipeline/framework/port:file_helpers",
        "//cv-cuda-pipeline/framework/port:opencv_highgui",
        "//cv-cuda-pipeline/framework/port:opencv_imgcodecs",
        "//cv-cuda-pipeline/framework/port:opencv_imgproc",
        "//cv-cuda-pipeline/framework/port:opencv_video",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/graphs/iris_tracking:iris_depth_cpu_deps",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:parse",
        "@com_google_absl//absl/log:absl_log",
    ],
)

# BUILD:
# bazel build -c opt --define MEDIAPIPE_DISABLE_GPU=1 \
#   //cv-cuda-pipeline/examples/desktop/iris_tracking:iris_tracking_cpu_video_input
# RUN:
# bazel-bin/cv-cuda-pipeline/examples/desktop/iris_tracking/iris_tracking_cpu_video_input \
#   --calculator_graph_config_file=<.../iris_tracking_cpu_video_input.pbtxt> \
#   --input_side_packets=input_video_path=<input video path>,output_video_path=<output video path>
cc_binary(
    name = "iris_tracking_cpu_video_input",
    data = ["//cv-cuda-pipeline/modules/iris_landmark:iris_landmark.tflite"],
    deps = [
        "//cv-cuda-pipeline/examples/desktop:simple_run_graph_main",
        "//cv-cuda-pipeline/graphs/iris_tracking:iris_tracking_cpu_video_input_deps",
    ],
)

# BUILD:
# bazel build -c opt --define MEDIAPIPE_DISABLE_GPU=1 \
#   //cv-cuda-pipeline/examples/desktop/iris_tracking:iris_tracking_cpu
# RUN:
# bazel-bin/cv-cuda-pipeline/examples/desktop/iris_tracking/iris_tracking_cpu \
#   --calculator_graph_config_file=<.../iris_tracking_cpu.pbtxt> \
#   --input_video_path=<input video path> \
#   --output_video_path=<output video path>
cc_binary(
    name = "iris_tracking_cpu",
    data = ["//cv-cuda-pipeline/modules/iris_landmark:iris_landmark.tflite"],
    deps = [
        "//cv-cuda-pipeline/examples/desktop:demo_run_graph_main",
        "//cv-cuda-pipeline/graphs/iris_tracking:iris_tracking_cpu_deps",
    ],
)

# Linux only
cc_binary(
    name = "iris_tracking_gpu",
    data = ["//cv-cuda-pipeline/modules/iris_landmark:iris_landmark.tflite"],
    deps = [
        "//cv-cuda-pipeline/examples/desktop:demo_run_graph_main_gpu",
        "//cv-cuda-pipeline/graphs/iris_tracking:iris_tracking_gpu_deps",
    ],
)
