# Copyright 2022 The MediaPipe Authors.
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

load("@litert//tflite/core/shims:cc_library_with_tflite.bzl", "cc_library_with_tflite", "cc_test_with_tflite")
load("@rules_cc//cc:cc_library.bzl", "cc_library")
load("@rules_cc//cc:cc_test.bzl", "cc_test")

package(default_visibility = ["//visibility:public"])

licenses(["notice"])

cc_library_with_tflite(
    name = "image_tensor_specs",
    srcs = ["image_tensor_specs.cc"],
    hdrs = ["image_tensor_specs.h"],
    tflite_deps = [
        "//cv-cuda-pipeline/tasks/cc/core:model_resources",
    ],
    deps = [
        "//cv-cuda-pipeline/framework/port:integral_types",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/tasks/cc:common",
        "//cv-cuda-pipeline/tasks/cc/metadata:metadata_extractor",
        "//cv-cuda-pipeline/tasks/metadata:metadata_schema_cc",
        "@com_google_absl//absl/algorithm:container",
        "@com_google_absl//absl/log:absl_log",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/types:optional",
        "@flatbuffers//:runtime_cc",
        "@litert//tflite/c:common",
        "@litert//tflite/schema:schema_fbs",
    ],
)

cc_test_with_tflite(
    name = "image_tensor_specs_test",
    srcs = ["image_tensor_specs_test.cc"],
    data = ["//cv-cuda-pipeline/tasks/testdata/vision:test_models"],
    tflite_deps = [
        ":image_tensor_specs",
        "//cv-cuda-pipeline/tasks/cc/core:model_resources",
        "@litert//tflite:test_util",
    ],
    deps = [
        "//cv-cuda-pipeline/framework/deps:file_path",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/tasks/cc:common",
        "//cv-cuda-pipeline/tasks/cc/core/proto:external_file_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/metadata:metadata_extractor",
        "//cv-cuda-pipeline/tasks/metadata:metadata_schema_cc",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/log:absl_check",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:cord",
        "@com_google_absl//absl/types:optional",
        "@litert//tflite/c:common",
    ],
)

cc_library(
    name = "image_utils",
    srcs = ["image_utils.cc"],
    hdrs = ["image_utils.h"],
    deps = [
        "//cv-cuda-pipeline/framework/formats:image",
        "//cv-cuda-pipeline/framework/formats:image_frame",
        "//cv-cuda-pipeline/framework/formats:tensor",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings:str_format",
        "@stblib//:stb_image",
    ],
)

cc_test(
    name = "image_utils_test",
    srcs = ["image_utils_test.cc"],
    deps = [
        ":image_utils",
        "//cv-cuda-pipeline/framework/formats:image",
        "//cv-cuda-pipeline/framework/formats:image_frame",
        "//cv-cuda-pipeline/framework/formats:tensor",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "landmarks_duplicates_finder",
    hdrs = ["landmarks_duplicates_finder.h"],
    deps = [
        "//cv-cuda-pipeline/framework/formats:landmark_cc_proto",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/status:statusor",
    ],
)

cc_library(
    name = "landmarks_utils",
    srcs = ["landmarks_utils.cc"],
    hdrs = ["landmarks_utils.h"],
    deps = ["//cv-cuda-pipeline/tasks/cc/components/containers:rect"],
)

cc_test(
    name = "landmarks_utils_test",
    srcs = ["landmarks_utils_test.cc"],
    deps = [
        ":landmarks_utils",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/tasks/cc/components/containers:rect",
    ],
)

cc_library(
    name = "data_renderer",
    srcs = ["data_renderer.cc"],
    hdrs = ["data_renderer.h"],
    deps = [
        "//cv-cuda-pipeline/calculators/util:annotation_overlay_calculator",
        "//cv-cuda-pipeline/calculators/util:landmarks_to_render_data_calculator",
        "//cv-cuda-pipeline/calculators/util:landmarks_to_render_data_calculator_cc_proto",
        "//cv-cuda-pipeline/calculators/util:rect_to_render_data_calculator_cc_proto",
        "//cv-cuda-pipeline/calculators/util:rect_to_render_scale_calculator",
        "//cv-cuda-pipeline/calculators/util:rect_to_render_scale_calculator_cc_proto",
        "//cv-cuda-pipeline/framework/api2:builder",
        "//cv-cuda-pipeline/framework/formats:image",
        "//cv-cuda-pipeline/framework/formats:landmark_cc_proto",
        "//cv-cuda-pipeline/framework/formats:rect_cc_proto",
        "//cv-cuda-pipeline/util:render_data_cc_proto",
        "@com_google_absl//absl/types:span",
    ],
)
