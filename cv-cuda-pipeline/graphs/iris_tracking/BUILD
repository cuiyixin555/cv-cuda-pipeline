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

load("@rules_cc//cc:cc_library.bzl", "cc_library")
load(
    "//cv-cuda-pipeline/framework/tool:mediapipe_graph.bzl",
    "mediapipe_binary_graph",
)

licenses(["notice"])

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "iris_depth_cpu_deps",
    deps = [
        "//cv-cuda-pipeline/calculators/core:constant_side_packet_calculator",
        "//cv-cuda-pipeline/calculators/core:flow_limiter_calculator",
        "//cv-cuda-pipeline/calculators/core:split_vector_calculator",
        "//cv-cuda-pipeline/calculators/image:image_file_properties_calculator",
        "//cv-cuda-pipeline/calculators/image:opencv_encoded_image_to_image_frame_calculator",
        "//cv-cuda-pipeline/calculators/image:opencv_image_encoder_calculator",
        "//cv-cuda-pipeline/graphs/iris_tracking/calculators:update_face_landmarks_calculator",
        "//cv-cuda-pipeline/graphs/iris_tracking/subgraphs:iris_and_depth_renderer_cpu",
        "//cv-cuda-pipeline/modules/face_landmark:face_landmark_front_cpu",
        "//cv-cuda-pipeline/modules/iris_landmark:iris_landmark_left_and_right_cpu",
    ],
)

cc_library(
    name = "iris_tracking_cpu_deps",
    deps = [
        "//cv-cuda-pipeline/calculators/core:constant_side_packet_calculator",
        "//cv-cuda-pipeline/calculators/core:flow_limiter_calculator",
        "//cv-cuda-pipeline/calculators/core:split_vector_calculator",
        "//cv-cuda-pipeline/graphs/iris_tracking/calculators:update_face_landmarks_calculator",
        "//cv-cuda-pipeline/graphs/iris_tracking/subgraphs:iris_renderer_cpu",
        "//cv-cuda-pipeline/modules/face_landmark:face_landmark_front_cpu",
        "//cv-cuda-pipeline/modules/iris_landmark:iris_landmark_left_and_right_cpu",
    ],
)

cc_library(
    name = "iris_tracking_cpu_video_input_deps",
    deps = [
        "//cv-cuda-pipeline/calculators/core:constant_side_packet_calculator",
        "//cv-cuda-pipeline/calculators/core:flow_limiter_calculator",
        "//cv-cuda-pipeline/calculators/core:split_vector_calculator",
        "//cv-cuda-pipeline/calculators/video:opencv_video_decoder_calculator",
        "//cv-cuda-pipeline/calculators/video:opencv_video_encoder_calculator",
        "//cv-cuda-pipeline/graphs/iris_tracking/calculators:update_face_landmarks_calculator",
        "//cv-cuda-pipeline/graphs/iris_tracking/subgraphs:iris_renderer_cpu",
        "//cv-cuda-pipeline/modules/face_landmark:face_landmark_front_cpu",
        "//cv-cuda-pipeline/modules/iris_landmark:iris_landmark_left_and_right_cpu",
    ],
)

cc_library(
    name = "iris_tracking_gpu_deps",
    deps = [
        "//cv-cuda-pipeline/calculators/core:constant_side_packet_calculator",
        "//cv-cuda-pipeline/calculators/core:flow_limiter_calculator",
        "//cv-cuda-pipeline/calculators/core:split_vector_calculator",
        "//cv-cuda-pipeline/graphs/iris_tracking/calculators:update_face_landmarks_calculator",
        "//cv-cuda-pipeline/graphs/iris_tracking/subgraphs:iris_and_depth_renderer_gpu",
        "//cv-cuda-pipeline/modules/face_landmark:face_landmark_front_gpu",
        "//cv-cuda-pipeline/modules/iris_landmark:iris_landmark_left_and_right_gpu",
    ],
)

mediapipe_binary_graph(
    name = "iris_tracking_gpu_binary_graph",
    graph = "iris_tracking_gpu.pbtxt",
    output_name = "iris_tracking_gpu.binarypb",
    deps = [":iris_tracking_gpu_deps"],
)
