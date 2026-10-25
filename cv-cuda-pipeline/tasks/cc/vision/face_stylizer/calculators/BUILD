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

load("@bazel_skylib//lib:selects.bzl", "selects")
load("@rules_cc//cc:cc_library.bzl", "cc_library")
load("//cv-cuda-pipeline/framework:more_selects.bzl", "more_selects")
load("//cv-cuda-pipeline/framework/port:build_config.bzl", "mediapipe_proto_library")

licenses(["notice"])

package(default_visibility = ["//visibility:public"])

config_setting(
    name = "disable_gpu",
    define_values = {
        "MEDIAPIPE_DISABLE_GPU": "1",
    },
)

more_selects.config_setting_negation(
    name = "not_disable_gpu",
    negate = ":disable_gpu",
)

selects.config_setting_group(
    name = "platform_apple_with_gpu",
    match_all = [
        ":not_disable_gpu",
        "//cv-cuda-pipeline:apple",
    ],
)

selects.config_setting_group(
    name = "platform_apple_without_gpu",
    match_all = [
        ":disable_gpu",
        "//cv-cuda-pipeline:apple",
    ],
)

mediapipe_proto_library(
    name = "tensors_to_image_calculator_proto",
    srcs = ["tensors_to_image_calculator.proto"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_options_proto",
        "//cv-cuda-pipeline/framework:calculator_proto",
        "//cv-cuda-pipeline/gpu:gpu_origin_proto",
    ],
)

cc_library(
    name = "tensors_to_image_calculator",
    srcs = ["tensors_to_image_calculator.cc"],
    copts = select({
        "//cv-cuda-pipeline:apple": [
            "-x objective-c++",
            "-fobjc-arc",  # enable reference-counting
        ],
        "//conditions:default": [],
    }),
    features = ["-layering_check"],  # allow depending on tensor_to_image_calculator_gpu_deps
    linkopts = select({
        "//cv-cuda-pipeline:apple": [
            "-framework CoreVideo",
            "-framework MetalKit",
        ],
        "//conditions:default": [],
    }),
    deps = [
        ":tensors_to_image_calculator_cc_proto",
        "//cv-cuda-pipeline/calculators/tensor:image_to_tensor_utils",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:calculator_options_cc_proto",
        "//cv-cuda-pipeline/framework/api2:builder",
        "//cv-cuda-pipeline/framework/api2:node",
        "//cv-cuda-pipeline/framework/api2:packet",
        "//cv-cuda-pipeline/framework/api2:port",
        "//cv-cuda-pipeline/framework/formats:image",
        "//cv-cuda-pipeline/framework/formats:image_frame_opencv",
        "//cv-cuda-pipeline/framework/formats:tensor",
        "//cv-cuda-pipeline/framework/port:logging",
        "//cv-cuda-pipeline/framework/port:opencv_core",
        "//cv-cuda-pipeline/framework/port:opencv_imgproc",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/framework/port:vector",
        "//cv-cuda-pipeline/gpu:gpu_origin_cc_proto",
        "@com_google_absl//absl/log:absl_check",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings",
    ] + select({
        "//cv-cuda-pipeline/gpu:disable_gpu": [],
        "//conditions:default": ["tensor_to_image_calculator_gpu_deps"],
    }) + select({
        "//cv-cuda-pipeline:apple": [
            "//third_party/apple_frameworks:CoreVideo",
            "//third_party/apple_frameworks:MetalKit",
        ],
        "//conditions:default": [],
    }),
    alwayslink = 1,
)

cc_library(
    name = "tensor_to_image_calculator_gpu_deps",
    visibility = ["//visibility:private"],
    deps = selects.with_or({
        (
        ): [
            "//cv-cuda-pipeline/gpu:gl_calculator_helper",
            "//cv-cuda-pipeline/gpu:gl_quad_renderer",
            "//cv-cuda-pipeline/gpu:gl_simple_shaders",
            "//cv-cuda-pipeline/gpu:gpu_buffer",
            "@litert//tflite/delegates/gpu:gl_delegate",
            "@litert//tflite/delegates/gpu/common:util",
            "@litert//tflite/delegates/gpu/gl:gl_program",
            "@litert//tflite/delegates/gpu/gl:gl_shader",
            "@litert//tflite/delegates/gpu/gl:gl_texture",
            "@litert//tflite/delegates/gpu/gl/converters:util",
        ],
        ":platform_apple_with_gpu": [
            "//cv-cuda-pipeline/gpu:MPPMetalHelper",
            "//cv-cuda-pipeline/gpu:MPPMetalUtil",
            "//cv-cuda-pipeline/gpu:gl_calculator_helper",
            "//cv-cuda-pipeline/gpu:gpu_buffer",
        ],
        # This setting is needed to allow bazel to build all targets on Mac with GPU disabled
        ":platform_apple_without_gpu": [],
        "//conditions:default": [
            "//cv-cuda-pipeline/gpu:gl_calculator_helper",
            "//cv-cuda-pipeline/gpu:gl_quad_renderer",
            "//cv-cuda-pipeline/gpu:gpu_buffer",
            "@litert//tflite/delegates/gpu:gl_delegate",
            "@litert//tflite/delegates/gpu/common:util",
            "@litert//tflite/delegates/gpu/gl:gl_program",
            "@litert//tflite/delegates/gpu/gl:gl_shader",
            "@litert//tflite/delegates/gpu/gl:gl_texture",
            "@litert//tflite/delegates/gpu/gl/converters:util",
        ],
    }),
)
