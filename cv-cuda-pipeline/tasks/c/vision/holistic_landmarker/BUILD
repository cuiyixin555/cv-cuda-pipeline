# Copyright 2026 The MediaPipe Authors.
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

licenses(["notice"])

cc_library(
    name = "holistic_landmarker_result",
    hdrs = ["holistic_landmarker_result.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//cv-cuda-pipeline/tasks/c/components/containers:category",
        "//cv-cuda-pipeline/tasks/c/components/containers:landmark",
        "//cv-cuda-pipeline/tasks/c/vision/core:image",
    ],
)

cc_library(
    name = "holistic_landmarker_result_converter",
    srcs = ["holistic_landmarker_result_converter.cc"],
    hdrs = ["holistic_landmarker_result_converter.h"],
    deps = [
        ":holistic_landmarker_result",
        "//cv-cuda-pipeline/framework/formats:image_format_cc_proto",
        "//cv-cuda-pipeline/tasks/c/components/containers:category",
        "//cv-cuda-pipeline/tasks/c/components/containers:category_converter",
        "//cv-cuda-pipeline/tasks/c/components/containers:landmark_converter",
        "//cv-cuda-pipeline/tasks/c/vision/core:image_frame_util",
        "//cv-cuda-pipeline/tasks/cc/components/containers:category",
        "//cv-cuda-pipeline/tasks/cc/components/containers:landmark",
        "//cv-cuda-pipeline/tasks/cc/vision/holistic_landmarker:holistic_landmarker_result",
    ],
)

HOLISTIC_LANDMARKER_DEPS = [
    ":holistic_landmarker_result",
    ":holistic_landmarker_result_converter",
    "@com_google_absl//absl/log:absl_log",
    "@com_google_absl//absl/status",
    "@com_google_absl//absl/status:statusor",
    "@com_google_absl//absl/log:absl_check",
    "//cv-cuda-pipeline/framework/formats:image",
    "//cv-cuda-pipeline/tasks/c/core:base_options",
    "//cv-cuda-pipeline/tasks/c/core:base_options_converter",
    "//cv-cuda-pipeline/tasks/c/core:mp_status",
    "//cv-cuda-pipeline/tasks/c/core:mp_status_converter",
    "//cv-cuda-pipeline/tasks/c/core:common",
    "//cv-cuda-pipeline/tasks/c/vision/core:image",
    "//cv-cuda-pipeline/tasks/c/vision/core:image_frame_util",
    "//cv-cuda-pipeline/tasks/c/vision/core:image_processing_options",
    "//cv-cuda-pipeline/tasks/c/vision/core:image_processing_options_converter",
    "//cv-cuda-pipeline/tasks/cc/vision/core:image_processing_options",
    "//cv-cuda-pipeline/tasks/cc/vision/core:running_mode",
    "//cv-cuda-pipeline/tasks/cc/vision/holistic_landmarker",
    "//cv-cuda-pipeline/tasks/cc/vision/holistic_landmarker:holistic_landmarker_result",
    "//cv-cuda-pipeline/tasks/cc/vision/utils:image_utils",
]

cc_library(
    name = "holistic_landmarker_lib",
    srcs = ["holistic_landmarker.cc"],
    hdrs = ["holistic_landmarker.h"],
    visibility = ["//visibility:public"],
    deps = HOLISTIC_LANDMARKER_DEPS,
)

cc_library(
    name = "holistic_landmarker_c_lib",
    srcs = ["holistic_landmarker.cc"],
    hdrs = ["holistic_landmarker.h"],
    visibility = ["//cv-cuda-pipeline/tasks/c:__subpackages__"],
    deps = HOLISTIC_LANDMARKER_DEPS,
    alwayslink = 1,
)
