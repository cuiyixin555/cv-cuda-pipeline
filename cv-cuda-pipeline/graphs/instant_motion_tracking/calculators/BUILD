# Copyright 2020 Google LLC
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
# Placeholder: load proto_library

load("//cv-cuda-pipeline/framework/port:build_config.bzl", "mediapipe_cc_proto_library")

licenses(["notice"])

package(default_visibility = ["//visibility:public"])

proto_library(
    name = "sticker_buffer_proto",
    srcs = [
        "sticker_buffer.proto",
    ],
)

mediapipe_cc_proto_library(
    name = "sticker_buffer_cc_proto",
    srcs = [
        "sticker_buffer.proto",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":sticker_buffer_proto",
    ],
)

cc_library(
    name = "sticker_manager_calculator",
    srcs = ["sticker_manager_calculator.cc"],
    hdrs = ["transformations.h"],
    deps = [
        ":sticker_buffer_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:timestamp",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
    ],
    alwayslink = 1,
)

cc_library(
    name = "matrices_manager_calculator",
    srcs = ["matrices_manager_calculator.cc"],
    hdrs = ["transformations.h"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:timestamp",
        "//cv-cuda-pipeline/framework/formats:image_frame",
        "//cv-cuda-pipeline/framework/port:opencv_imgproc",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/graphs/object_detection_3d/calculators:model_matrix_cc_proto",
        "//cv-cuda-pipeline/modules/objectron/calculators:box",
        "@com_google_absl//absl/log:absl_log",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/strings",
        "@eigen//:eigen3",
    ],
    alwayslink = 1,
)

cc_library(
    name = "tracked_anchor_manager_calculator",
    srcs = ["tracked_anchor_manager_calculator.cc"],
    hdrs = ["transformations.h"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/util/tracking:box_tracker_cc_proto",
    ],
    alwayslink = 1,
)
