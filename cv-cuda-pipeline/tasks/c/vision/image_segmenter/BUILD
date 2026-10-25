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

package(default_visibility = ["//cv-cuda-pipeline/tasks:internal"])

licenses(["notice"])

cc_library(
    name = "image_segmenter_result",
    hdrs = ["image_segmenter_result.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//cv-cuda-pipeline/tasks/c/core:common",
        "//cv-cuda-pipeline/tasks/c/vision/core:image",
    ],
)

cc_library(
    name = "image_segmenter_result_converter",
    srcs = ["image_segmenter_result_converter.cc"],
    hdrs = ["image_segmenter_result_converter.h"],
    deps = [
        ":image_segmenter_result",
        "//cv-cuda-pipeline/framework/formats:image",
        "//cv-cuda-pipeline/tasks/c/core:common",
        "//cv-cuda-pipeline/tasks/c/vision/core:image",
        "//cv-cuda-pipeline/tasks/c/vision/core:image_frame_util",
        "//cv-cuda-pipeline/tasks/cc/vision/image_segmenter:image_segmenter_result",
    ],
)

cc_test(
    name = "image_segmenter_result_converter_test",
    srcs = ["image_segmenter_result_converter_test.cc"],
    data = [
        "//cv-cuda-pipeline/framework/formats:image_frame_opencv",
        "//cv-cuda-pipeline/framework/port:opencv_core",
        "//cv-cuda-pipeline/framework/port:opencv_imgproc",
        "//cv-cuda-pipeline/tasks/testdata/vision:test_images",
    ],
    linkstatic = 1,
    deps = [
        ":image_segmenter_result",
        ":image_segmenter_result_converter",
        "//cv-cuda-pipeline/framework/deps:file_path",
        "//cv-cuda-pipeline/framework/formats:image",
        "//cv-cuda-pipeline/framework/port:gtest",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/tasks/c/core:common",
        "//cv-cuda-pipeline/tasks/c/vision/core:image",
        "//cv-cuda-pipeline/tasks/cc/vision/image_segmenter:image_segmenter_result",
        "//cv-cuda-pipeline/tasks/cc/vision/utils:image_utils",
        "@com_google_absl//absl/flags:flag",
        "@com_google_googletest//:gtest_main",
    ],
)

IMAGE_SEGMENTER_DEPS = [
    ":image_segmenter_result",
    ":image_segmenter_result_converter",
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
    "//cv-cuda-pipeline/tasks/cc/vision/image_segmenter",
    "//cv-cuda-pipeline/tasks/cc/vision/image_segmenter:image_segmenter_result",
    "//cv-cuda-pipeline/tasks/cc/vision/utils:image_utils",
]

cc_library(
    name = "image_segmenter_lib",
    srcs = ["image_segmenter.cc"],
    hdrs = ["image_segmenter.h"],
    visibility = ["//visibility:public"],
    deps = IMAGE_SEGMENTER_DEPS,
)

cc_library(
    name = "image_segmenter_c_lib",
    srcs = ["image_segmenter.cc"],
    hdrs = ["image_segmenter.h"],
    visibility = ["//cv-cuda-pipeline/tasks/c:__subpackages__"],
    deps = IMAGE_SEGMENTER_DEPS,
    alwayslink = 1,
)

cc_test(
    name = "image_segmenter_test",
    srcs = ["image_segmenter_test.cc"],
    data = [
        "//cv-cuda-pipeline/framework/formats:image_frame_opencv",
        "//cv-cuda-pipeline/framework/port:opencv_core",
        "//cv-cuda-pipeline/framework/port:opencv_imgproc",
        "//cv-cuda-pipeline/tasks/testdata/vision:test_images",
        "//cv-cuda-pipeline/tasks/testdata/vision:test_models",
    ],
    linkstatic = 1,
    deps = [
        ":image_segmenter_lib",
        ":image_segmenter_result",
        "//cv-cuda-pipeline/framework/deps:file_path",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:status_matchers",
        "//cv-cuda-pipeline/tasks/c/core:common",
        "//cv-cuda-pipeline/tasks/c/core:mp_status",
        "//cv-cuda-pipeline/tasks/c/test:test_utils",
        "//cv-cuda-pipeline/tasks/c/vision/core:image",
        "//cv-cuda-pipeline/tasks/c/vision/core:image_processing_options",
        "//cv-cuda-pipeline/tasks/c/vision/core:image_test_util",
        "//cv-cuda-pipeline/tasks/cc/vision/utils:image_utils",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/synchronization",
        "@com_google_absl//absl/time",
        "@com_google_googletest//:gtest_main",
    ],
)

# bazel build -c opt --linkopt -s --strip always --define MEDIAPIPE_DISABLE_GPU=1 \
# //cv-cuda-pipeline/tasks/c/vision/image_segmenter:libimage_segmenter.so
cc_binary(
    name = "libimage_segmenter.so",
    linkopts = [
        "-Wl,-soname=libimage_segmenter.so",
        "-fvisibility=hidden",
    ],
    linkshared = True,
    tags = [
        "manual",
        "nobuilder",
        "notap",
    ],
    deps = [":image_segmenter_lib"],
)

# bazel build -c opt --strip always --define MEDIAPIPE_DISABLE_GPU=1 \
