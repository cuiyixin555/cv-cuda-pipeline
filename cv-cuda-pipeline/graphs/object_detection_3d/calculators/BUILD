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

licenses(["notice"])

package(default_visibility = ["//visibility:public"])

# This is a protobuf alias.
mediapipe_proto_library(
    name = "gl_animation_overlay_calculator_proto",
    srcs = ["gl_animation_overlay_calculator.proto"],
    def_options_lib = False,
    visibility = ["//visibility:public"],
    exports = [
        "//cv-cuda-pipeline/gpu:gl_animation_overlay_calculator_proto",
    ],
    deps = [
        "//cv-cuda-pipeline/gpu:gl_animation_overlay_calculator_proto",
    ],
)

mediapipe_proto_library(
    name = "annotations_to_model_matrices_calculator_proto",
    srcs = ["annotations_to_model_matrices_calculator.proto"],
    visibility = ["//visibility:public"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_proto",
    ],
)

mediapipe_proto_library(
    name = "model_matrix_proto",
    srcs = ["model_matrix.proto"],
    visibility = ["//visibility:public"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_proto",
    ],
)

mediapipe_proto_library(
    name = "annotations_to_render_data_calculator_proto",
    srcs = ["annotations_to_render_data_calculator.proto"],
    visibility = ["//visibility:public"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_proto",
        "//cv-cuda-pipeline/util:color_proto",
    ],
)

cc_library(
    name = "gl_animation_overlay_calculator",
    srcs = ["gl_animation_overlay_calculator.cc"],
    visibility = ["//visibility:public"],
    deps = [
        ":gl_animation_overlay_calculator_cc_proto",
        ":model_matrix_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/gpu:gl_calculator_helper",
        "//cv-cuda-pipeline/gpu:shader_util",
        "//cv-cuda-pipeline/modules/objectron/calculators:camera_parameters_cc_proto",
        "@com_google_absl//absl/log:absl_check",
        "@com_google_absl//absl/log:absl_log",
    ],
    alwayslink = 1,
)

cc_library(
    name = "annotations_to_model_matrices_calculator",
    srcs = ["annotations_to_model_matrices_calculator.cc"],
    visibility = ["//visibility:public"],
    deps = [
        ":annotations_to_model_matrices_calculator_cc_proto",
        ":model_matrix_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:calculator_options_cc_proto",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/modules/objectron/calculators:annotation_cc_proto",
        "//cv-cuda-pipeline/modules/objectron/calculators:box",
        "//cv-cuda-pipeline/util:color_cc_proto",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/strings",
        "@eigen//:eigen3",
    ],
    alwayslink = 1,
)

cc_library(
    name = "annotations_to_render_data_calculator",
    srcs = ["annotations_to_render_data_calculator.cc"],
    visibility = ["//visibility:public"],
    deps = [
        ":annotations_to_render_data_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:calculator_options_cc_proto",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/modules/objectron/calculators:annotation_cc_proto",
        "//cv-cuda-pipeline/util:color_cc_proto",
        "//cv-cuda-pipeline/util:render_data_cc_proto",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/strings",
    ],
    alwayslink = 1,
)
