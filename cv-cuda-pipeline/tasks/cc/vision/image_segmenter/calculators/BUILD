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

load("@rules_cc//cc:cc_library.bzl", "cc_library")
load("@rules_cc//cc:cc_test.bzl", "cc_test")
load("//cv-cuda-pipeline/framework/port:build_config.bzl", "mediapipe_proto_library")

package(default_visibility = ["//visibility:public"])

licenses(["notice"])

mediapipe_proto_library(
    name = "tensors_to_segmentation_calculator_proto",
    srcs = ["tensors_to_segmentation_calculator.proto"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_options_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/image_segmenter/proto:segmenter_options_proto",
        "//cv-cuda-pipeline/util:label_map_proto",
    ],
)

cc_library(
    name = "tensors_to_segmentation_calculator",
    srcs = ["tensors_to_segmentation_calculator.cc"],
    hdrs = ["tensors_to_segmentation_calculator.h"],
    deps = [
        ":tensors_to_segmentation_calculator_cc_proto",
        "//cv-cuda-pipeline/framework/api3:calculator",
        "//cv-cuda-pipeline/framework/api3:calculator_context",
        "//cv-cuda-pipeline/framework/api3:calculator_contract",
        "//cv-cuda-pipeline/framework/api3:contract",
        "//cv-cuda-pipeline/framework/api3:node",
        "//cv-cuda-pipeline/framework/formats:image",
        "//cv-cuda-pipeline/framework/formats:image_frame",
        "//cv-cuda-pipeline/framework/formats:image_frame_opencv",
        "//cv-cuda-pipeline/framework/formats:tensor",
        "//cv-cuda-pipeline/framework/port:opencv_imgproc",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/tasks/cc/vision/image_segmenter/proto:segmenter_options_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/utils:image_utils",
        "//cv-cuda-pipeline/util:label_map_cc_proto",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/types:span",
    ],
    alwayslink = 1,
)

cc_library(
    name = "segmentation_postprocessor_gl",
    srcs = ["segmentation_postprocessor_gl.cc"],
    hdrs = ["segmentation_postprocessor_gl.h"],
    tags = ["nomac"],
    deps = [
        ":tensors_to_segmentation_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/formats:image",
        "//cv-cuda-pipeline/framework/formats:tensor",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/gpu:gl_calculator_helper",
        "//cv-cuda-pipeline/gpu:gl_simple_shaders",
        "//cv-cuda-pipeline/gpu:gpu_buffer_format",
        "//cv-cuda-pipeline/gpu:shader_util",
        "//cv-cuda-pipeline/tasks/cc/vision/image_segmenter/proto:segmenter_options_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/utils:image_utils",
        "@com_google_absl//absl/log:absl_log",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
    ] + select({
        "//conditions:default": [],
    }),
)

cc_test(
    name = "tensors_to_segmentation_calculator_test",
    srcs = ["tensors_to_segmentation_calculator_test.cc"],
    deps = [
        ":tensors_to_segmentation_calculator",
        ":tensors_to_segmentation_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:calculator_runner",
        "//cv-cuda-pipeline/framework:packet",
        "//cv-cuda-pipeline/framework/formats:image",
        "//cv-cuda-pipeline/framework/formats:image_frame",
        "//cv-cuda-pipeline/framework/formats:tensor",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/types:span",
    ],
)
