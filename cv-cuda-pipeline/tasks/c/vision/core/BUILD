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

package(default_visibility = ["//cv-cuda-pipeline/tasks:internal"])

licenses(["notice"])

cc_library(
    name = "image_hdr",
    hdrs = ["image.h"],
    visibility = ["//visibility:private"],
    deps = ["//cv-cuda-pipeline/tasks/c/core:mp_status"],
)

IMAGE_DEPS = [
    ":image_frame_util",
    "@easyexif//:easyexif",
    "@com_google_absl//absl/strings:string_view",
    "@com_google_absl//absl/log:absl_log",
    "@com_google_absl//absl/status",
    "@com_google_absl//absl/status:statusor",
    "@com_google_absl//absl/strings:str_format",
    "//cv-cuda-pipeline/framework/deps:file_helpers",
    "//cv-cuda-pipeline/framework/formats:image",
    "//cv-cuda-pipeline/framework/formats:image_frame",
    "//cv-cuda-pipeline/tasks/c/core:common",
    "//cv-cuda-pipeline/tasks/c/core:mp_status",
    "//cv-cuda-pipeline/tasks/c/core:mp_status_converter",
    "@stblib//:stb_image",
]

cc_library(
    name = "image",
    srcs = ["image.cc"],
    hdrs = ["image.h"],
    deps = IMAGE_DEPS,
    # The MpImage* C API is exported from shared libraries that are linked
    # without whole-archive; nothing inside e.g. the retrieval library
    # references it, so it has to be linked unconditionally.
    alwayslink = 1,
)


cc_library(
    name = "image_frame_util",
    hdrs = ["image_frame_util.h"],
    deps = [
        "//cv-cuda-pipeline/framework/formats:image",
        "//cv-cuda-pipeline/framework/formats:image_format_cc_proto",
        "//cv-cuda-pipeline/framework/formats:image_frame",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
    ],
)

cc_library(
    name = "image_processing_options",
    hdrs = ["image_processing_options.h"],
    deps = [
        "//cv-cuda-pipeline/tasks/c/components/containers:rect",
    ],
)

cc_library(
    name = "image_processing_options_converter",
    srcs = ["image_processing_options_converter.cc"],
    hdrs = ["image_processing_options_converter.h"],
    deps = [
        ":image_processing_options",
        "//cv-cuda-pipeline/tasks/cc/components/containers:rect",
        "//cv-cuda-pipeline/tasks/cc/vision/core:image_processing_options",
    ],
)

cc_test(
    name = "image_processing_options_converter_test",
    srcs = ["image_processing_options_converter_test.cc"],
    deps = [
        ":image_processing_options",
        ":image_processing_options_converter",
        "//cv-cuda-pipeline/framework/port:gtest",
        "//cv-cuda-pipeline/tasks/c/core:common",
        "//cv-cuda-pipeline/tasks/cc/components/containers:rect",
        "//cv-cuda-pipeline/tasks/cc/vision/core:image_processing_options",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "image_test_util",
    testonly = True,
    srcs = ["image_test_util.cc"],
    hdrs = ["image_test_util.h"],
    deps = [
        ":image_frame_util",
        ":image_hdr",
        "//cv-cuda-pipeline/framework/formats:image",
        "//cv-cuda-pipeline/gpu:gpu_buffer",
        "//cv-cuda-pipeline/tasks/c/core:mp_status",
        "@com_google_absl//absl/log:absl_check",
    ],
)
