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

cc_library(
    name = "hand_detections_from_pose_to_rects_calculator",
    srcs = ["hand_detections_from_pose_to_rects_calculator.cc"],
    deps = [
        "//cv-cuda-pipeline/calculators/util:detections_to_rects_calculator",
        "//cv-cuda-pipeline/calculators/util:detections_to_rects_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:calculator_options_cc_proto",
        "//cv-cuda-pipeline/framework/formats:detection_cc_proto",
        "//cv-cuda-pipeline/framework/formats:location_data_cc_proto",
        "//cv-cuda-pipeline/framework/formats:rect_cc_proto",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
    ],
    alwayslink = 1,
)

mediapipe_proto_library(
    name = "roi_tracking_calculator_proto",
    srcs = ["roi_tracking_calculator.proto"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_options_proto",
        "//cv-cuda-pipeline/framework:calculator_proto",
    ],
)

cc_library(
    name = "roi_tracking_calculator",
    srcs = ["roi_tracking_calculator.cc"],
    deps = [
        ":roi_tracking_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/formats:landmark_cc_proto",
        "//cv-cuda-pipeline/framework/formats:rect_cc_proto",
        "//cv-cuda-pipeline/framework/port:logging",
        "//cv-cuda-pipeline/framework/port:rectangle",
        "@com_google_absl//absl/strings:str_format",
    ],
    alwayslink = 1,
)
