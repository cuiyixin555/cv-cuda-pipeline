#
# Copyright 2019 The MediaPipe Authors.
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
#
# Description:
#   Working with dense optical flow in mediapipe.

load("@rules_cc//cc:cc_library.bzl", "cc_library")
load("@rules_cc//cc:cc_test.bzl", "cc_test")
load("//cv-cuda-pipeline/framework/port:build_config.bzl", "mediapipe_proto_library")

licenses(["notice"])

package(default_visibility = ["//visibility:public"])

mediapipe_proto_library(
    name = "optical_flow_field_data_proto",
    srcs = ["optical_flow_field_data.proto"],
)

cc_library(
    name = "optical_flow_field",
    srcs = ["optical_flow_field.cc"],
    hdrs = ["optical_flow_field.h"],
    deps = [
        ":optical_flow_field_data_cc_proto",
        "//cv-cuda-pipeline/framework:type_map",
        "//cv-cuda-pipeline/framework/deps:mathutil",
        "//cv-cuda-pipeline/framework/formats:location",
        "//cv-cuda-pipeline/framework/formats:location_opencv",
        "//cv-cuda-pipeline/framework/port:file_helpers",
        "//cv-cuda-pipeline/framework/port:integral_types",
        "//cv-cuda-pipeline/framework/port:opencv_imgproc",
        "//cv-cuda-pipeline/framework/port:point",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "@com_google_absl//absl/log:absl_check",
        "@com_google_absl//absl/log:absl_log",
        "@com_google_absl//absl/strings",
        "@org_tensorflow//tensorflow/core:framework",
    ],
    alwayslink = 1,
)

cc_test(
    name = "optical_flow_field_test",
    srcs = ["optical_flow_field_test.cc"],
    linkstatic = 1,
    deps = [
        ":optical_flow_field",
        "//cv-cuda-pipeline/framework/deps:file_path",
        "//cv-cuda-pipeline/framework/formats:location_opencv",
        "//cv-cuda-pipeline/framework/port:file_helpers",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//third_party:opencv",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/log:absl_check",
        "@com_google_absl//absl/log:absl_log",
        "@org_tensorflow//tensorflow/core:framework",
    ],
)
