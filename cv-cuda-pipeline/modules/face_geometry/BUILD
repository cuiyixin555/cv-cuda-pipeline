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

load("@rules_cc//cc:cc_library.bzl", "cc_library")
load("//cv-cuda-pipeline/framework/port:build_config.bzl", "mediapipe_proto_library")
load("//cv-cuda-pipeline/framework/tool:mediapipe_graph.bzl", "mediapipe_simple_subgraph")

licenses(["notice"])

package(default_visibility = ["//visibility:public"])

mediapipe_simple_subgraph(
    name = "face_geometry",
    graph = "face_geometry.pbtxt",
    register_as = "FaceGeometry",
    deps = [
        ":geometry_pipeline_calculator",
    ],
)

mediapipe_simple_subgraph(
    name = "face_geometry_from_detection",
    graph = "face_geometry_from_detection.pbtxt",
    register_as = "FaceGeometryFromDetection",
    deps = [
        ":geometry_pipeline_calculator",
        "//cv-cuda-pipeline/calculators/core:begin_loop_calculator",
        "//cv-cuda-pipeline/calculators/core:end_loop_calculator",
        "//cv-cuda-pipeline/calculators/util:detection_to_landmarks_calculator",
    ],
)

mediapipe_simple_subgraph(
    name = "face_geometry_from_landmarks",
    graph = "face_geometry_from_landmarks.pbtxt",
    register_as = "FaceGeometryFromLandmarks",
    deps = [
        ":geometry_pipeline_calculator",
    ],
)

mediapipe_proto_library(
    name = "effect_renderer_calculator_proto",
    srcs = ["effect_renderer_calculator.proto"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_options_proto",
    ],
)

cc_library(
    name = "effect_renderer_calculator",
    srcs = ["effect_renderer_calculator.cc"],
    deps = [
        ":effect_renderer_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:resources",
        "//cv-cuda-pipeline/framework/formats:image_frame",
        "//cv-cuda-pipeline/framework/formats:image_frame_opencv",
        "//cv-cuda-pipeline/framework/port:opencv_core",
        "//cv-cuda-pipeline/framework/port:opencv_imgcodecs",
        "//cv-cuda-pipeline/framework/port:opencv_imgproc",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/framework/port:statusor",
        "//cv-cuda-pipeline/gpu:gl_calculator_helper",
        "//cv-cuda-pipeline/gpu:gpu_buffer",
        "//cv-cuda-pipeline/modules/face_geometry/libs:effect_renderer",
        "//cv-cuda-pipeline/modules/face_geometry/libs:validation_utils",
        "//cv-cuda-pipeline/modules/face_geometry/protos:environment_cc_proto",
        "//cv-cuda-pipeline/modules/face_geometry/protos:face_geometry_cc_proto",
        "//cv-cuda-pipeline/modules/face_geometry/protos:mesh_3d_cc_proto",
        "//cv-cuda-pipeline/util:resource_util",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/types:optional",
    ],
    alwayslink = 1,
)

mediapipe_proto_library(
    name = "env_generator_calculator_proto",
    srcs = ["env_generator_calculator.proto"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_options_proto",
        "//cv-cuda-pipeline/modules/face_geometry/protos:environment_proto",
    ],
)

cc_library(
    name = "env_generator_calculator",
    srcs = ["env_generator_calculator.cc"],
    deps = [
        ":env_generator_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/modules/face_geometry/libs:validation_utils",
        "//cv-cuda-pipeline/modules/face_geometry/protos:environment_cc_proto",
    ],
    alwayslink = 1,
)

mediapipe_proto_library(
    name = "geometry_pipeline_calculator_proto",
    srcs = ["geometry_pipeline_calculator.proto"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_options_proto",
    ],
)

cc_library(
    name = "geometry_pipeline_calculator",
    srcs = ["geometry_pipeline_calculator.cc"],
    deps = [
        ":geometry_pipeline_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:resources",
        "//cv-cuda-pipeline/framework/formats:landmark_cc_proto",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/framework/port:statusor",
        "//cv-cuda-pipeline/modules/face_geometry/libs:geometry_pipeline",
        "//cv-cuda-pipeline/modules/face_geometry/libs:validation_utils",
        "//cv-cuda-pipeline/modules/face_geometry/protos:environment_cc_proto",
        "//cv-cuda-pipeline/modules/face_geometry/protos:face_geometry_cc_proto",
        "//cv-cuda-pipeline/modules/face_geometry/protos:geometry_pipeline_metadata_cc_proto",
        "//cv-cuda-pipeline/util:resource_util",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
    ],
    alwayslink = 1,
)
