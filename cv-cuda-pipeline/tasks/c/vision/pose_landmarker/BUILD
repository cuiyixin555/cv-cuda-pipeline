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

load("@rules_cc//cc:cc_binary.bzl", "cc_binary")
load("@rules_cc//cc:cc_library.bzl", "cc_library")
load("@rules_cc//cc:cc_test.bzl", "cc_test")

licenses(["notice"])

cc_library(
    name = "pose_landmarker_result",
    hdrs = ["pose_landmarker_result.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//cv-cuda-pipeline/tasks/c/components/containers:landmark",
        "//cv-cuda-pipeline/tasks/c/vision/core:image",
    ],
)

cc_library(
    name = "pose_landmarker_result_converter",
    srcs = ["pose_landmarker_result_converter.cc"],
    hdrs = ["pose_landmarker_result_converter.h"],
    deps = [
        ":pose_landmarker_result",
        "//cv-cuda-pipeline/framework/formats:image",
        "//cv-cuda-pipeline/tasks/c/components/containers:landmark",
        "//cv-cuda-pipeline/tasks/c/components/containers:landmark_converter",
        "//cv-cuda-pipeline/tasks/c/core:common",
        "//cv-cuda-pipeline/tasks/c/vision/core:image",
        "//cv-cuda-pipeline/tasks/c/vision/core:image_frame_util",
        "//cv-cuda-pipeline/tasks/cc/components/containers:landmark",
        "//cv-cuda-pipeline/tasks/cc/vision/pose_landmarker:pose_landmarker_result",
    ],
)

cc_test(
    name = "pose_landmarker_result_converter_test",
    srcs = ["pose_landmarker_result_converter_test.cc"],
    data = [
        "//cv-cuda-pipeline/framework/formats:image_frame_opencv",
        "//cv-cuda-pipeline/framework/port:opencv_core",
        "//cv-cuda-pipeline/framework/port:opencv_imgproc",
        "//cv-cuda-pipeline/tasks/testdata/vision:test_images",
    ],
    linkstatic = 1,
    deps = [
        ":pose_landmarker_result",
        ":pose_landmarker_result_converter",
        "//cv-cuda-pipeline/framework/deps:file_path",
        "//cv-cuda-pipeline/framework/formats:image",
        "//cv-cuda-pipeline/framework/port:gtest",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/tasks/c/components/containers:landmark",
        "//cv-cuda-pipeline/tasks/cc/components/containers:landmark",
        "//cv-cuda-pipeline/tasks/cc/vision/pose_landmarker:pose_landmarker_result",
        "//cv-cuda-pipeline/tasks/cc/vision/utils:image_utils",
        "@com_google_absl//absl/flags:flag",
        "@com_google_googletest//:gtest_main",
    ],
)

POSE_LANDMARKER_DEPS = [
    ":pose_landmarker_result",
    ":pose_landmarker_result_converter",
    "@com_google_absl//absl/log:absl_log",
    "@com_google_absl//absl/log:absl_check",
    "@com_google_absl//absl/status",
    "@com_google_absl//absl/status:statusor",
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
    "//cv-cuda-pipeline/tasks/cc/vision/pose_landmarker",
    "//cv-cuda-pipeline/tasks/cc/vision/pose_landmarker:pose_landmarker_result",
]

cc_library(
    name = "pose_landmarker_lib",
    srcs = ["pose_landmarker.cc"],
    hdrs = ["pose_landmarker.h"],
    visibility = ["//visibility:public"],
    deps = POSE_LANDMARKER_DEPS,
)

cc_library(
    name = "pose_landmarker_c_lib",
    srcs = ["pose_landmarker.cc"],
    hdrs = ["pose_landmarker.h"],
    visibility = ["//cv-cuda-pipeline/tasks/c:__subpackages__"],
    deps = POSE_LANDMARKER_DEPS,
    alwayslink = 1,
)

cc_test(
    name = "pose_landmarker_test",
    srcs = ["pose_landmarker_test.cc"],
    data = [
        "//cv-cuda-pipeline/framework/formats:image_frame_opencv",
        "//cv-cuda-pipeline/framework/port:opencv_core",
        "//cv-cuda-pipeline/framework/port:opencv_imgproc",
        "//cv-cuda-pipeline/tasks/testdata/vision:test_images",
        "//cv-cuda-pipeline/tasks/testdata/vision:test_models",
    ],
    linkstatic = 1,
    deps = [
        ":pose_landmarker_lib",
        ":pose_landmarker_result",
        "//cv-cuda-pipeline/framework/deps:file_path",
        "//cv-cuda-pipeline/framework/port:gtest",
        "//cv-cuda-pipeline/tasks/c/components/containers:landmark",
        "//cv-cuda-pipeline/tasks/c/core:common",
        "//cv-cuda-pipeline/tasks/c/core:mp_status",
        "//cv-cuda-pipeline/tasks/c/vision/core:image",
        "//cv-cuda-pipeline/tasks/c/vision/core:image_frame_util",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/synchronization",
        "@com_google_absl//absl/time",
        "@com_google_googletest//:gtest_main",
    ],
)

# bazel build -c opt --linkopt -s --strip always --define MEDIAPIPE_DISABLE_GPU=1 \
# //cv-cuda-pipeline/tasks/c/vision/pose_landmarker:libpose_landmarker.so
cc_binary(
    name = "libpose_landmarker.so",
    linkopts = [
        "-Wl,-soname=libpose_landmarker.so",
        "-fvisibility=hidden",
    ],
    linkshared = True,
    tags = [
        "manual",
        "nobuilder",
        "notap",
    ],
    deps = [":pose_landmarker_lib"],
)

# bazel build -c opt --strip always --define MEDIAPIPE_DISABLE_GPU=1 \
