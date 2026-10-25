# Copyright 2023 The MediaPipe Authors.
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

licenses(["notice"])

package(
    default_visibility = ["//visibility:public"],
)

mediapipe_proto_library(
    name = "env_generator_calculator_proto",
    srcs = ["env_generator_calculator.proto"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_options_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/face_geometry/proto:environment_proto",
    ],
)

cc_library(
    name = "env_generator_calculator",
    srcs = ["env_generator_calculator.cc"],
    deps = [
        ":env_generator_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/tasks/cc/vision/face_geometry/libs:validation_utils",
        "//cv-cuda-pipeline/tasks/cc/vision/face_geometry/proto:environment_cc_proto",
    ],
    alwayslink = 1,
)

mediapipe_proto_library(
    name = "geometry_pipeline_calculator_proto",
    srcs = ["geometry_pipeline_calculator.proto"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_options_proto",
        "//cv-cuda-pipeline/tasks/cc/core/proto:external_file_proto",
    ],
)

cc_library(
    name = "geometry_pipeline_calculator",
    srcs = ["geometry_pipeline_calculator.cc"],
    deps = [
        ":geometry_pipeline_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/formats:landmark_cc_proto",
        "//cv-cuda-pipeline/framework/port:logging",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/framework/port:statusor",
        "//cv-cuda-pipeline/tasks/cc:common",
        "//cv-cuda-pipeline/tasks/cc/core:external_file_handler",
        "//cv-cuda-pipeline/tasks/cc/core/proto:external_file_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/face_geometry/libs:geometry_pipeline",
        "//cv-cuda-pipeline/tasks/cc/vision/face_geometry/libs:validation_utils",
        "//cv-cuda-pipeline/tasks/cc/vision/face_geometry/proto:environment_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/face_geometry/proto:face_geometry_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/face_geometry/proto:geometry_pipeline_metadata_cc_proto",
        "//cv-cuda-pipeline/util:resource_util",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/strings:str_format",
    ],
    alwayslink = 1,
)
