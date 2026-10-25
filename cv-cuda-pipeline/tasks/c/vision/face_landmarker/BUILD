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
    name = "face_landmarker_result",
    hdrs = ["face_landmarker_result.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//cv-cuda-pipeline/tasks/c/components/containers:category",
        "//cv-cuda-pipeline/tasks/c/components/containers:landmark",
        "//cv-cuda-pipeline/tasks/c/components/containers:matrix",
    ],
)

cc_library(
    name = "face_landmarker_result_converter",
    srcs = ["face_landmarker_result_converter.cc"],
    hdrs = ["face_landmarker_result_converter.h"],
    deps = [
        ":face_landmarker_result",
        "//cv-cuda-pipeline/tasks/c/components/containers:category",
        "//cv-cuda-pipeline/tasks/c/components/containers:category_converter",
        "//cv-cuda-pipeline/tasks/c/components/containers:landmark",
        "//cv-cuda-pipeline/tasks/c/components/containers:landmark_converter",
        "//cv-cuda-pipeline/tasks/c/components/containers:matrix",
        "//cv-cuda-pipeline/tasks/c/components/containers:matrix_converter",
        "//cv-cuda-pipeline/tasks/cc/components/containers:category",
        "//cv-cuda-pipeline/tasks/cc/components/containers:landmark",
        "//cv-cuda-pipeline/tasks/cc/vision/face_landmarker:face_landmarker_result",
    ],
)

cc_test(
    name = "face_landmarker_result_converter_test",
    srcs = ["face_landmarker_result_converter_test.cc"],
    linkstatic = 1,
    deps = [
        ":face_landmarker_result",
        ":face_landmarker_result_converter",
        "//cv-cuda-pipeline/framework/formats:matrix",
        "//cv-cuda-pipeline/framework/port:gtest",
        "//cv-cuda-pipeline/tasks/c/components/containers:landmark",
        "//cv-cuda-pipeline/tasks/cc/components/containers:category",
        "//cv-cuda-pipeline/tasks/cc/components/containers:classification_result",
        "//cv-cuda-pipeline/tasks/cc/components/containers:landmark",
        "//cv-cuda-pipeline/tasks/cc/vision/face_landmarker:face_landmarker_result",
        "@com_google_googletest//:gtest_main",
        "@eigen//:eigen3",
    ],
)

FACE_LANDMARKER_DEPS = [
    ":face_landmarker_result",
    ":face_landmarker_result_converter",
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
    "//cv-cuda-pipeline/tasks/cc/vision/face_landmarker",
    "//cv-cuda-pipeline/tasks/cc/vision/face_landmarker:face_landmarker_result",
    "//cv-cuda-pipeline/tasks/cc/vision/utils:image_utils",
]

cc_library(
    name = "face_landmarker_lib",
    srcs = ["face_landmarker.cc"],
    hdrs = ["face_landmarker.h"],
    visibility = ["//visibility:public"],
    deps = FACE_LANDMARKER_DEPS,
)

cc_library(
    name = "face_landmarker_c_lib",
    srcs = ["face_landmarker.cc"],
    hdrs = ["face_landmarker.h"],
    visibility = ["//cv-cuda-pipeline/tasks/c:__subpackages__"],
    deps = FACE_LANDMARKER_DEPS,
    alwayslink = 1,
)

cc_test(
    name = "face_landmarker_test",
    srcs = ["face_landmarker_test.cc"],
    data = [
        "//cv-cuda-pipeline/framework/formats:image_frame_opencv",
        "//cv-cuda-pipeline/framework/port:opencv_core",
        "//cv-cuda-pipeline/framework/port:opencv_imgproc",
        "//cv-cuda-pipeline/tasks/testdata/vision:test_images",
        "//cv-cuda-pipeline/tasks/testdata/vision:test_models",
    ],
    linkstatic = 1,
    deps = [
        ":face_landmarker_lib",
        ":face_landmarker_result",
        "//cv-cuda-pipeline/framework/deps:file_path",
        "//cv-cuda-pipeline/framework/port:gtest",
        "//cv-cuda-pipeline/tasks/c/components/containers:landmark",
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
# //cv-cuda-pipeline/tasks/c/vision/face_landmarker:libface_landmarker.so
cc_binary(
    name = "libface_landmarker.so",
    linkopts = [
        "-Wl,-soname=libface_landmarker.so",
        "-fvisibility=hidden",
    ],
    linkshared = True,
    tags = [
        "manual",
        "nobuilder",
        "notap",
    ],
    deps = [":face_landmarker_lib"],
)

# bazel build --config darwin_arm64 -c opt --strip always --define MEDIAPIPE_DISABLE_GPU=1 \
# //cv-cuda-pipeline/tasks/c/vision/face_landmarker:libface_landmarker.dylib
cc_binary(
    name = "libface_landmarker.dylib",
    linkopts = [
        "-Wl,-install_name,libface_landmarker.dylib",
        "-fvisibility=hidden",
    ],
    linkshared = True,
    tags = [
        "manual",
        "nobuilder",
        "notap",
    ],
    deps = [":face_landmarker_lib"],
)
