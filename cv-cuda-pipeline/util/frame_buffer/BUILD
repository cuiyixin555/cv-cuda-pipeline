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

load("@rules_cc//cc:cc_library.bzl", "cc_library")
load("@rules_cc//cc:cc_test.bzl", "cc_test")

package(default_visibility = ["//cv-cuda-pipeline/util/frame_buffer:__subpackages__"])

cc_library(
    name = "frame_buffer_util",
    srcs = ["frame_buffer_util.cc"],
    hdrs = ["frame_buffer_util.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":buffer",
        "//cv-cuda-pipeline/framework/formats:frame_buffer",
        "//cv-cuda-pipeline/framework/formats:tensor",
        "//cv-cuda-pipeline/framework/port:status",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings:str_format",
    ],
)

cc_test(
    name = "frame_buffer_util_test",
    srcs = [
        "frame_buffer_util_test.cc",
    ],
    deps = [
        ":frame_buffer_util",
        "//cv-cuda-pipeline/framework/formats:frame_buffer",
        "//cv-cuda-pipeline/framework/formats:tensor",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:status",
        "@com_google_absl//absl/log:absl_check",
    ],
)

cc_library(
    name = "buffer",
    srcs = [
        "buffer_common.cc",
        "float_buffer.cc",
        "gray_buffer.cc",
        "rgb_buffer.cc",
        "yuv_buffer.cc",
    ],
    hdrs = [
        "buffer_common.h",
        "float_buffer.h",
        "gray_buffer.h",
        "rgb_buffer.h",
        "yuv_buffer.h",
    ],
    deps = [
        "//cv-cuda-pipeline/util/frame_buffer/halide:gray_flip_halide",
        "//cv-cuda-pipeline/util/frame_buffer/halide:gray_resize_halide",
        "//cv-cuda-pipeline/util/frame_buffer/halide:gray_rotate_halide",
        "//cv-cuda-pipeline/util/frame_buffer/halide:rgb_flip_halide",
        "//cv-cuda-pipeline/util/frame_buffer/halide:rgb_float_halide",
        "//cv-cuda-pipeline/util/frame_buffer/halide:rgb_gray_halide",
        "//cv-cuda-pipeline/util/frame_buffer/halide:rgb_resize_halide",
        "//cv-cuda-pipeline/util/frame_buffer/halide:rgb_rgb_halide",
        "//cv-cuda-pipeline/util/frame_buffer/halide:rgb_rotate_halide",
        "//cv-cuda-pipeline/util/frame_buffer/halide:rgb_yuv_halide",
        "//cv-cuda-pipeline/util/frame_buffer/halide:yuv_flip_halide",
        "//cv-cuda-pipeline/util/frame_buffer/halide:yuv_resize_halide",
        "//cv-cuda-pipeline/util/frame_buffer/halide:yuv_rgb_halide",
        "//cv-cuda-pipeline/util/frame_buffer/halide:yuv_rotate_halide",
        "@halide//:runtime",
    ],
)

# Tests:
cc_test(
    name = "rgb_buffer_test",
    srcs = ["rgb_buffer_test.cc"],
    deps = [
        ":buffer",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "@com_google_absl//absl/log:absl_log",
    ],
)

cc_test(
    name = "yuv_buffer_test",
    srcs = ["yuv_buffer_test.cc"],
    tags = ["nohwasan"],
    deps = [
        ":buffer",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "@com_google_absl//absl/log:absl_log",
    ],
)

cc_test(
    name = "gray_buffer_test",
    srcs = ["gray_buffer_test.cc"],
    deps = [
        ":buffer",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "@com_google_absl//absl/log:absl_log",
    ],
)
