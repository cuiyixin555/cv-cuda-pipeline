# Copyright 2024 The MediaPipe Authors.
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

FACE_DETECTOR_DEPS = [
    "@com_google_absl//absl/log:absl_log",
    "@com_google_absl//absl/status",
    "@com_google_absl//absl/log:absl_check",
    "@com_google_absl//absl/status:statusor",
    "//cv-cuda-pipeline/framework/formats:image",
    "//cv-cuda-pipeline/tasks/c/components/containers:detection_result",
    "//cv-cuda-pipeline/tasks/c/components/containers:detection_result_converter",
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
    "//cv-cuda-pipeline/tasks/cc/vision/face_detector",
]

cc_library(
    name = "face_detector_lib",
    srcs = ["face_detector.cc"],
    hdrs = ["face_detector.h"],
    visibility = ["//visibility:public"],
    deps = FACE_DETECTOR_DEPS,
)

cc_library(
    name = "face_detector_c_lib",
    srcs = ["face_detector.cc"],
    hdrs = ["face_detector.h"],
    visibility = ["//cv-cuda-pipeline/tasks/c:__subpackages__"],
    deps = FACE_DETECTOR_DEPS,
    alwayslink = 1,
)

cc_test(
    name = "face_detector_test",
    srcs = ["face_detector_test.cc"],
    data = [
        "//cv-cuda-pipeline/framework/formats:image_frame_opencv",
        "//cv-cuda-pipeline/framework/port:opencv_core",
        "//cv-cuda-pipeline/framework/port:opencv_imgproc",
        "//cv-cuda-pipeline/tasks/testdata/vision:test_images",
        "//cv-cuda-pipeline/tasks/testdata/vision:test_models",
    ],
    linkstatic = 1,
    deps = [
        ":face_detector_lib",
        "//cv-cuda-pipeline/framework/deps:file_path",
        "//cv-cuda-pipeline/framework/port:gtest",
        "//cv-cuda-pipeline/tasks/c/components/containers:detection_result",
        "//cv-cuda-pipeline/tasks/c/components/containers:keypoint",
        "//cv-cuda-pipeline/tasks/c/components/containers:rect",
        "//cv-cuda-pipeline/tasks/c/core:base_options",
        "//cv-cuda-pipeline/tasks/c/core:common",
        "//cv-cuda-pipeline/tasks/c/core:mp_status",
        "//cv-cuda-pipeline/tasks/c/vision/core:image",
        "//cv-cuda-pipeline/tasks/c/vision/core:image_processing_options",
        "//cv-cuda-pipeline/tasks/c/vision/core:image_test_util",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/synchronization",
        "@com_google_absl//absl/time",
        "@com_google_googletest//:gtest_main",
    ],
)

# bazel build -c opt --linkopt -s --strip always --define MEDIAPIPE_DISABLE_GPU=1 \
# //cv-cuda-pipeline/tasks/c/vision/face_detector:libface_detector.so
cc_binary(
    name = "libface_detector.so",
    linkopts = [
        "-Wl,-soname=libface_detector.so",
        "-fvisibility=hidden",
    ],
    linkshared = True,
    tags = [
        "manual",
        "nobuilder",
        "notap",
    ],
    deps = [":face_detector_lib"],
)

# bazel build -c opt --strip always --define MEDIAPIPE_DISABLE_GPU=1 \
