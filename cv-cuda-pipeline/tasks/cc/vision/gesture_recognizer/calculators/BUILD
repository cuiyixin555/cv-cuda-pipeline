# Copyright 2022 The MediaPipe Authors.
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
load("@rules_cc//cc:cc_test.bzl", "cc_test")
load("//cv-cuda-pipeline/framework/port:build_config.bzl", "mediapipe_proto_library")

package(default_visibility = ["//visibility:public"])

mediapipe_proto_library(
    name = "landmarks_to_matrix_calculator_proto",
    srcs = ["landmarks_to_matrix_calculator.proto"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_options_proto",
        "//cv-cuda-pipeline/framework:calculator_proto",
        "//cv-cuda-pipeline/tasks/cc/core/proto:base_options_proto",
    ],
)

cc_library(
    name = "handedness_to_matrix_calculator",
    srcs = ["handedness_to_matrix_calculator.cc"],
    hdrs = ["handedness_to_matrix_calculator.h"],
    deps = [
        "//cv-cuda-pipeline/framework/api3:calculator",
        "//cv-cuda-pipeline/framework/api3:calculator_context",
        "//cv-cuda-pipeline/framework/api3:contract",
        "//cv-cuda-pipeline/framework/api3:node",
        "//cv-cuda-pipeline/framework/formats:classification_cc_proto",
        "//cv-cuda-pipeline/framework/formats:matrix",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/tasks/cc/vision/gesture_recognizer:handedness_util",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
    ],
    alwayslink = 1,
)

cc_test(
    name = "handedness_to_matrix_calculator_test",
    srcs = ["handedness_to_matrix_calculator_test.cc"],
    deps = [
        ":handedness_to_matrix_calculator",
        "//cv-cuda-pipeline/framework/api3:function_runner",
        "//cv-cuda-pipeline/framework/api3:graph",
        "//cv-cuda-pipeline/framework/api3:packet",
        "//cv-cuda-pipeline/framework/api3:stream",
        "//cv-cuda-pipeline/framework/formats:classification_cc_proto",
        "//cv-cuda-pipeline/framework/formats:matrix",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/status:statusor",
    ],
)

cc_library(
    name = "landmarks_to_matrix_calculator",
    srcs = ["landmarks_to_matrix_calculator.cc"],
    hdrs = ["landmarks_to_matrix_calculator.h"],
    deps = [
        ":landmarks_to_matrix_calculator_cc_proto",
        "//cv-cuda-pipeline/framework/api3:calculator",
        "//cv-cuda-pipeline/framework/api3:calculator_context",
        "//cv-cuda-pipeline/framework/api3:calculator_contract",
        "//cv-cuda-pipeline/framework/api3:contract",
        "//cv-cuda-pipeline/framework/api3:node",
        "//cv-cuda-pipeline/framework/formats:landmark_cc_proto",
        "//cv-cuda-pipeline/framework/formats:matrix",
        "//cv-cuda-pipeline/framework/formats:rect_cc_proto",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
    ],
    alwayslink = 1,
)

cc_test(
    name = "landmarks_to_matrix_calculator_test",
    srcs = ["landmarks_to_matrix_calculator_test.cc"],
    deps = [
        ":landmarks_to_matrix_calculator",
        "//cv-cuda-pipeline/framework/api3:function_runner",
        "//cv-cuda-pipeline/framework/api3:graph",
        "//cv-cuda-pipeline/framework/api3:packet",
        "//cv-cuda-pipeline/framework/api3:stream",
        "//cv-cuda-pipeline/framework/formats:landmark_cc_proto",
        "//cv-cuda-pipeline/framework/formats:matrix",
        "//cv-cuda-pipeline/framework/formats:rect_cc_proto",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "@com_google_absl//absl/strings",
    ],
)

mediapipe_proto_library(
    name = "combined_prediction_calculator_proto",
    srcs = ["combined_prediction_calculator.proto"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_options_proto",
        "//cv-cuda-pipeline/framework:calculator_proto",
    ],
)

cc_library(
    name = "combined_prediction_calculator",
    srcs = ["combined_prediction_calculator.cc"],
    hdrs = ["combined_prediction_calculator.h"],
    deps = [
        ":combined_prediction_calculator_cc_proto",
        "//cv-cuda-pipeline/framework/api3:calculator",
        "//cv-cuda-pipeline/framework/api3:calculator_context",
        "//cv-cuda-pipeline/framework/api3:contract",
        "//cv-cuda-pipeline/framework/api3:node",
        "//cv-cuda-pipeline/framework/formats:classification_cc_proto",
        "@com_google_absl//absl/container:btree",
        "@com_google_absl//absl/status",
    ],
    alwayslink = 1,
)

cc_test(
    name = "combined_prediction_calculator_test",
    srcs = ["combined_prediction_calculator_test.cc"],
    deps = [
        ":combined_prediction_calculator",
        "//cv-cuda-pipeline/framework/api3:function_runner",
        "//cv-cuda-pipeline/framework/api3:graph",
        "//cv-cuda-pipeline/framework/api3:packet",
        "//cv-cuda-pipeline/framework/api3:stream",
        "//cv-cuda-pipeline/framework/formats:classification_cc_proto",
        "//cv-cuda-pipeline/framework/port:gtest",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:status",
    ],
)
