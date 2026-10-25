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
    name = "hand_landmarker_result",
    hdrs = ["hand_landmarker_result.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//cv-cuda-pipeline/tasks/c/components/containers:category",
        "//cv-cuda-pipeline/tasks/c/components/containers:landmark",
    ],
)

cc_library(
    name = "hand_landmarker_result_converter",
    srcs = ["hand_landmarker_result_converter.cc"],
    hdrs = ["hand_landmarker_result_converter.h"],
    deps = [
        ":hand_landmarker_result",
        "//cv-cuda-pipeline/tasks/c/components/containers:category",
        "//cv-cuda-pipeline/tasks/c/components/containers:category_converter",
        "//cv-cuda-pipeline/tasks/c/components/containers:landmark",
        "//cv-cuda-pipeline/tasks/c/components/containers:landmark_converter",
        "//cv-cuda-pipeline/tasks/cc/components/containers:category",
        "//cv-cuda-pipeline/tasks/cc/components/containers:landmark",
        "//cv-cuda-pipeline/tasks/cc/vision/hand_landmarker:hand_landmarker_result",
    ],
)

cc_test(
    name = "hand_landmarker_result_converter_test",
    srcs = ["hand_landmarker_result_converter_test.cc"],
    linkstatic = 1,
    deps = [
        ":hand_landmarker_result",
        ":hand_landmarker_result_converter",
        "//cv-cuda-pipeline/framework/port:gtest",
        "//cv-cuda-pipeline/tasks/c/components/containers:landmark",
        "//cv-cuda-pipeline/tasks/cc/components/containers:category",
        "//cv-cuda-pipeline/tasks/cc/components/containers:classification_result",
        "//cv-cuda-pipeline/tasks/cc/components/containers:landmark",
        "//cv-cuda-pipeline/tasks/cc/vision/hand_landmarker:hand_landmarker_result",
        "@com_google_googletest//:gtest_main",
    ],
)

HAND_LANDMARKER_DEPS = [
    ":hand_landmarker_result",
    ":hand_landmarker_result_converter",
    "@com_google_absl//absl/log:absl_log",
    "@com_google_absl//absl/log:absl_check",
    "@com_google_absl//absl/status",
    "@com_google_absl//absl/status:statusor",
    "//cv-cuda-pipeline/framework/formats:image",
    "//cv-cuda-pipeline/tasks/c/core:base_options",
    "//cv-cuda-pipeline/tasks/c/core:base_options_converter",
    "//cv-cuda-pipeline/tasks/c/core:mp_status",
    "//cv-cuda-pipeline/tasks/c/core:mp_status_converter",
    "//cv-cuda-pipeline/tasks/c/vision/core:image",
    "//cv-cuda-pipeline/tasks/c/core:common",
    "//cv-cuda-pipeline/tasks/c/vision/core:image_frame_util",
    "//cv-cuda-pipeline/tasks/c/vision/core:image_processing_options",
    "//cv-cuda-pipeline/tasks/c/vision/core:image_processing_options_converter",
    "//cv-cuda-pipeline/tasks/cc/vision/core:image_processing_options",
    "//cv-cuda-pipeline/tasks/cc/vision/core:running_mode",
    "//cv-cuda-pipeline/tasks/cc/vision/hand_landmarker",
    "//cv-cuda-pipeline/tasks/cc/vision/hand_landmarker:hand_landmarker_result",
    "//cv-cuda-pipeline/tasks/cc/vision/utils:image_utils",
]

cc_library(
    name = "hand_landmarker_lib",
    srcs = ["hand_landmarker.cc"],
    hdrs = ["hand_landmarker.h"],
    visibility = ["//visibility:public"],
    deps = HAND_LANDMARKER_DEPS,
)

cc_library(
    name = "hand_landmarker_c_lib",
    srcs = ["hand_landmarker.cc"],
    hdrs = ["hand_landmarker.h"],
    visibility = ["//cv-cuda-pipeline/tasks/c:__subpackages__"],
    deps = HAND_LANDMARKER_DEPS,
    alwayslink = 1,
)

cc_test(
    name = "hand_landmarker_test",
    srcs = ["hand_landmarker_test.cc"],
    data = [
        "//cv-cuda-pipeline/framework/formats:image_frame_opencv",
        "//cv-cuda-pipeline/framework/port:opencv_core",
        "//cv-cuda-pipeline/framework/port:opencv_imgproc",
        "//cv-cuda-pipeline/tasks/testdata/vision:test_images",
        "//cv-cuda-pipeline/tasks/testdata/vision:test_models",
        "//cv-cuda-pipeline/tasks/testdata/vision:test_protos",
    ],
    linkstatic = 1,
    deps = [
        ":hand_landmarker_lib",
        ":hand_landmarker_result",
        "//cv-cuda-pipeline/framework/deps:file_path",
        "//cv-cuda-pipeline/framework/formats:classification_cc_proto",
        "//cv-cuda-pipeline/framework/formats:landmark_cc_proto",
        "//cv-cuda-pipeline/framework/port:file_helpers",
        "//cv-cuda-pipeline/framework/port:gtest",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
        "//cv-cuda-pipeline/tasks/c/components/containers:landmark",
        "//cv-cuda-pipeline/tasks/c/core:common",
        "//cv-cuda-pipeline/tasks/c/core:mp_status",
        "//cv-cuda-pipeline/tasks/c/vision/core:image",
        "//cv-cuda-pipeline/tasks/c/vision/core:image_processing_options",
        "//cv-cuda-pipeline/tasks/c/vision/core:image_test_util",
        "//cv-cuda-pipeline/tasks/cc/components/containers/proto:landmarks_detection_result_cc_proto",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/synchronization",
        "@com_google_absl//absl/time",
        "@com_google_googletest//:gtest_main",
    ],
)

# bazel build -c opt --linkopt -s --strip always --define MEDIAPIPE_DISABLE_GPU=1 \
# //cv-cuda-pipeline/tasks/c/vision/hand_landmarker:libhand_landmarker.so
cc_binary(
    name = "libhand_landmarker.so",
    linkopts = [
        "-Wl,-soname=libhand_landmarker.so",
        "-fvisibility=hidden",
    ],
    linkshared = True,
    tags = [
        "manual",
        "nobuilder",
        "notap",
    ],
    deps = [":hand_landmarker_lib"],
)

# bazel build --config darwin_arm64 -c opt --strip always --define MEDIAPIPE_DISABLE_GPU=1 \
# //cv-cuda-pipeline/tasks/c/vision/hand_landmarker:libhand_landmarker.dylib
cc_binary(
    name = "libhand_landmarker.dylib",
    linkopts = [
        "-Wl,-install_name,libhand_landmarker.dylib",
        "-fvisibility=hidden",
    ],
    linkshared = True,
    tags = [
        "manual",
        "nobuilder",
        "notap",
    ],
    deps = [":hand_landmarker_lib"],
)
