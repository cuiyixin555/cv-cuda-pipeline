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
load("//cv-cuda-pipeline/framework/port:build_config.bzl", "mediapipe_proto_library")

licenses(["notice"])

package(default_visibility = ["//visibility:private"])

mediapipe_proto_library(
    name = "callback_packet_calculator_proto",
    srcs = ["callback_packet_calculator.proto"],
    visibility = ["//cv-cuda-pipeline/framework:__subpackages__"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_options_proto",
        "//cv-cuda-pipeline/framework:calculator_proto",
    ],
)

cc_library(
    name = "callback_packet_calculator",
    srcs = ["callback_packet_calculator.cc"],
    hdrs = ["callback_packet_calculator.h"],
    visibility = ["//cv-cuda-pipeline/framework:__subpackages__"],
    deps = [
        ":callback_packet_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_base",
        "//cv-cuda-pipeline/framework:calculator_registry",
        "//cv-cuda-pipeline/framework:output_side_packet",
        "@com_google_absl//absl/status",
    ],
    alwayslink = 1,
)
