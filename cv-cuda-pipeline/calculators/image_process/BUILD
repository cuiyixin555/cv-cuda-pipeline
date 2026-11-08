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

load("@rules_cc//cc:cc_library.bzl", "cc_library")
load("@rules_cc//cc:cc_test.bzl", "cc_test")
load("//cv-cuda-pipeline/framework/port:build_config.bzl", "mediapipe_proto_library")

licenses(["notice"])

package(default_visibility = ["//visibility:public"])

mediapipe_proto_library(
    name = "opencv_image_encoder_calculator_proto",
    srcs = ["opencv_image_encoder_calculator.proto"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_options_proto",
        "//cv-cuda-pipeline/framework:calculator_proto",
    ],
)

mediapipe_proto_library(
    name = "scale_image_calculator_proto",
    srcs = ["scale_image_calculator.proto"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_options_proto",
        "//cv-cuda-pipeline/framework:calculator_proto",
        "//cv-cuda-pipeline/framework/formats:image_format_proto",
    ],
)

mediapipe_proto_library(
    name = "set_alpha_calculator_proto",
    srcs = ["set_alpha_calculator.proto"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_options_proto",
        "//cv-cuda-pipeline/framework:calculator_proto",
    ],
)

mediapipe_proto_library(
    name = "image_cropping_calculator_proto",
    srcs = ["image_cropping_calculator.proto"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_options_proto",
        "//cv-cuda-pipeline/framework:calculator_proto",
    ],
)

mediapipe_proto_library(
    name = "bilateral_filter_calculator_proto",
    srcs = ["bilateral_filter_calculator.proto"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_options_proto",
        "//cv-cuda-pipeline/framework:calculator_proto",
    ],
)

mediapipe_proto_library(
    name = "recolor_calculator_proto",
    srcs = ["recolor_calculator.proto"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_options_proto",
        "//cv-cuda-pipeline/framework:calculator_proto",
        "//cv-cuda-pipeline/util:color_proto",
    ],
)

mediapipe_proto_library(
    name = "segmentation_smoothing_calculator_proto",
    srcs = ["segmentation_smoothing_calculator.proto"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_options_proto",
        "//cv-cuda-pipeline/framework:calculator_proto",
    ],
)

cc_library(
    name = "color_convert_calculator",
    srcs = ["color_convert_calculator.cc"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:timestamp",
        "//cv-cuda-pipeline/framework/formats:image_frame",
        "//cv-cuda-pipeline/framework/formats:image_frame_opencv",
        "//cv-cuda-pipeline/framework/port:integral_types",
        "//cv-cuda-pipeline/framework/port:logging",
        "//cv-cuda-pipeline/framework/port:opencv_imgproc",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:source_location",
        "//cv-cuda-pipeline/framework/port:status",
        "@com_google_absl//absl/log:absl_check",
    ],
    alwayslink = 1,
)

cc_library(
    name = "opencv_encoded_image_to_image_frame_calculator",
    srcs = ["opencv_encoded_image_to_image_frame_calculator.cc"],
    deps = [
        ":opencv_encoded_image_to_image_frame_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/formats:image_frame_opencv",
        "//cv-cuda-pipeline/framework/port:opencv_imgcodecs",
        "//cv-cuda-pipeline/framework/port:opencv_imgproc",
        "//cv-cuda-pipeline/framework/port:status",
    ],
    alwayslink = 1,
)

cc_library(
    name = "opencv_image_encoder_calculator",
    srcs = ["opencv_image_encoder_calculator.cc"],
    deps = [
        ":opencv_image_encoder_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/formats:image_frame_opencv",
        "//cv-cuda-pipeline/framework/port:opencv_imgcodecs",
        "//cv-cuda-pipeline/framework/port:opencv_imgproc",
        "//cv-cuda-pipeline/framework/port:status",
        "@com_google_absl//absl/log:absl_check",
    ],
    alwayslink = 1,
)

cc_library(
    name = "opencv_put_text_calculator",
    srcs = ["opencv_put_text_calculator.cc"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/formats:image_frame_opencv",
        "//cv-cuda-pipeline/framework/port:opencv_imgproc",
        "//cv-cuda-pipeline/framework/port:status",
    ],
    alwayslink = 1,
)

cc_library(
    name = "set_alpha_calculator",
    srcs = ["set_alpha_calculator.cc"],
    deps = [
        ":set_alpha_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:calculator_options_cc_proto",
        "//cv-cuda-pipeline/framework/formats:image_format_cc_proto",
        "//cv-cuda-pipeline/framework/formats:image_frame",
        "//cv-cuda-pipeline/framework/formats:image_frame_opencv",
        "//cv-cuda-pipeline/framework/port:opencv_core",
        "//cv-cuda-pipeline/framework/port:opencv_imgproc",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/framework/port:vector",
        "@com_google_absl//absl/log:absl_log",
    ] + select({
        "//cv-cuda-pipeline/gpu:disable_gpu": [],
        "//conditions:default": [
            "//cv-cuda-pipeline/gpu:gl_calculator_helper",
            "//cv-cuda-pipeline/gpu:gl_quad_renderer",
            "//cv-cuda-pipeline/gpu:gl_simple_shaders",
            "//cv-cuda-pipeline/gpu:shader_util",
        ],
    }),
    alwayslink = 1,
)

cc_test(
    name = "set_alpha_calculator_test",
    srcs = ["set_alpha_calculator_test.cc"],
    deps = [
        ":set_alpha_calculator",
        ":set_alpha_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:calculator_runner",
        "//cv-cuda-pipeline/framework/formats:image_frame_opencv",
        "//cv-cuda-pipeline/framework/formats:rect_cc_proto",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:opencv_core",
        "//cv-cuda-pipeline/framework/port:opencv_imgproc",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
        "//cv-cuda-pipeline/framework/port:status",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "bilateral_filter_calculator",
    srcs = ["bilateral_filter_calculator.cc"],
    deps = [
        ":bilateral_filter_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:calculator_options_cc_proto",
        "//cv-cuda-pipeline/framework/formats:image_format_cc_proto",
        "//cv-cuda-pipeline/framework/formats:image_frame",
        "//cv-cuda-pipeline/framework/formats:image_frame_opencv",
        "//cv-cuda-pipeline/framework/port:logging",
        "//cv-cuda-pipeline/framework/port:opencv_core",
        "//cv-cuda-pipeline/framework/port:opencv_imgproc",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/framework/port:vector",
        "@com_google_absl//absl/log:absl_check",
        "@com_google_absl//absl/strings",
    ] + select({
        "//cv-cuda-pipeline/gpu:disable_gpu": [],
        "//conditions:default": [
            "//cv-cuda-pipeline/gpu:gl_calculator_helper",
            "//cv-cuda-pipeline/gpu:gl_quad_renderer",
            "//cv-cuda-pipeline/gpu:gl_simple_shaders",
            "//cv-cuda-pipeline/gpu:shader_util",
        ],
    }),
    alwayslink = 1,
)

mediapipe_proto_library(
    name = "rotation_mode_proto",
    srcs = ["rotation_mode.proto"],
)

mediapipe_proto_library(
    name = "image_transformation_calculator_proto",
    srcs = ["image_transformation_calculator.proto"],
    deps = [
        ":rotation_mode_proto",
        "//cv-cuda-pipeline/framework:calculator_options_proto",
        "//cv-cuda-pipeline/framework:calculator_proto",
        "//cv-cuda-pipeline/gpu:scale_mode_proto",
    ],
)

cc_library(
    name = "image_transformation_calculator",
    srcs = ["image_transformation_calculator.cc"],
    copts = [],
    linkopts = [],
    deps = [
        ":image_transformation_calculator_cc_proto",
        ":rotation_mode_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:packet",
        "//cv-cuda-pipeline/framework:timestamp",
        "//cv-cuda-pipeline/framework/formats:image_frame",
        "//cv-cuda-pipeline/framework/formats:image_frame_opencv",
        "//cv-cuda-pipeline/framework/formats:video_stream_header",
        "//cv-cuda-pipeline/framework/port:opencv_core",
        "//cv-cuda-pipeline/framework/port:opencv_imgproc",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/gpu:scale_mode_cc_proto",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings",
    ] + select({
        "//cv-cuda-pipeline/gpu:disable_gpu": [],
        "//conditions:default": [
            "//cv-cuda-pipeline/gpu:gl_base_hdr",
            "//cv-cuda-pipeline/gpu:gl_calculator_helper",
            "//cv-cuda-pipeline/gpu:gl_quad_renderer",
            "//cv-cuda-pipeline/gpu:gl_simple_shaders",
            "//cv-cuda-pipeline/gpu:shader_util",
        ],
    }),
    alwayslink = 1,
)


cc_library(
    name = "image_cropping_calculator",
    srcs = ["image_cropping_calculator.cc"],
    hdrs = ["image_cropping_calculator.h"],
    copts = [],
    linkopts = [],
    deps = [
        ":image_cropping_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/formats:image_frame",
        "//cv-cuda-pipeline/framework/formats:image_frame_opencv",
        "//cv-cuda-pipeline/framework/formats:rect_cc_proto",
        "//cv-cuda-pipeline/framework/port:opencv_core",
        "//cv-cuda-pipeline/framework/port:opencv_imgproc",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "@com_google_absl//absl/log:absl_log",
    ] + select({
        "//cv-cuda-pipeline/gpu:disable_gpu": [],
        "//conditions:default": [
            "//cv-cuda-pipeline/gpu:gl_calculator_helper",
            "//cv-cuda-pipeline/gpu:gl_quad_renderer",
            "//cv-cuda-pipeline/gpu:gl_simple_shaders",
            "//cv-cuda-pipeline/gpu:gpu_buffer",
            "//cv-cuda-pipeline/gpu:shader_util",
        ],
    }),
    alwayslink = 1,
)


cc_library(
    name = "luminance_calculator",
    srcs = ["luminance_calculator.cc"],
    deps = [
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/gpu:gl_simple_calculator",
        "//cv-cuda-pipeline/gpu:gl_simple_shaders",
        "//cv-cuda-pipeline/gpu:shader_util",
    ],
    alwayslink = 1,
)

cc_library(
    name = "sobel_edges_calculator",
    srcs = ["sobel_edges_calculator.cc"],
    deps = [
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/gpu:gl_simple_calculator",
        "//cv-cuda-pipeline/gpu:gl_simple_shaders",
        "//cv-cuda-pipeline/gpu:shader_util",
    ],
    alwayslink = 1,
)

cc_library(
    name = "recolor_calculator",
    srcs = ["recolor_calculator.cc"],
    deps = [
        ":recolor_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/formats:image_frame",
        "//cv-cuda-pipeline/framework/formats:image_frame_opencv",
        "//cv-cuda-pipeline/framework/port:opencv_core",
        "//cv-cuda-pipeline/framework/port:opencv_imgproc",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/util:color_cc_proto",
    ] + select({
        "//cv-cuda-pipeline/gpu:disable_gpu": [],
        "//conditions:default": [
            "//cv-cuda-pipeline/gpu:gl_calculator_helper",
            "//cv-cuda-pipeline/gpu:gl_quad_renderer",
            "//cv-cuda-pipeline/gpu:gl_simple_shaders",
            "//cv-cuda-pipeline/gpu:shader_util",
        ],
    }),
    alwayslink = 1,
)

cc_library(
    name = "scale_image_utils",
    srcs = ["scale_image_utils.cc"],
    hdrs = ["scale_image_utils.h"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/port:logging",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "@com_google_absl//absl/log:absl_check",
        "@com_google_absl//absl/strings",
    ],
)

cc_library(
    name = "scale_image_calculator",
    srcs = ["scale_image_calculator.cc"],
    deps = [
        ":scale_image_calculator_cc_proto",
        ":scale_image_utils",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/formats:image_format_cc_proto",
        "//cv-cuda-pipeline/framework/formats:image_frame",
        "//cv-cuda-pipeline/framework/formats:image_frame_opencv",
        "//cv-cuda-pipeline/framework/formats:video_stream_header",
        "//cv-cuda-pipeline/framework/formats:yuv_image",
        "//cv-cuda-pipeline/framework/port:core_proto",
        "//cv-cuda-pipeline/framework/port:image_resizer",
        "//cv-cuda-pipeline/framework/port:logging",
        "//cv-cuda-pipeline/framework/port:opencv_core",
        "//cv-cuda-pipeline/framework/port:opencv_imgproc",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/util:image_frame_util",
        "@com_google_absl//absl/log:absl_check",
        "@com_google_absl//absl/log:absl_log",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/types:span",
        "@libyuv",
    ],
    alwayslink = 1,
)

cc_test(
    name = "scale_image_calculator_test",
    srcs = ["scale_image_calculator_test.cc"],
    deps = [
        ":scale_image_calculator",
        "//cv-cuda-pipeline/framework:calculator_runner",
        "//cv-cuda-pipeline/framework:packet",
        "//cv-cuda-pipeline/framework:timestamp",
        "//cv-cuda-pipeline/framework/formats:image_frame",
        "//cv-cuda-pipeline/framework/formats:yuv_image",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
        "@com_google_absl//absl/log:absl_check",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/types:span",
        "@libyuv",
    ],
)

mediapipe_proto_library(
    name = "image_clone_calculator_proto",
    srcs = ["image_clone_calculator.proto"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_options_proto",
        "//cv-cuda-pipeline/framework:calculator_proto",
    ],
)

cc_library(
    name = "image_clone_calculator",
    srcs = ["image_clone_calculator.cc"],
    deps = [
        ":image_clone_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/api2:node",
        "//cv-cuda-pipeline/framework/formats:image",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
    ] + select({
        "//cv-cuda-pipeline/gpu:disable_gpu": [],
        "//conditions:default": [
            "//cv-cuda-pipeline/gpu:gl_calculator_helper",
        ],
    }),
    alwayslink = 1,
)

cc_library(
    name = "image_properties_calculator",
    srcs = ["image_properties_calculator.cc"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/api2:node",
        "//cv-cuda-pipeline/framework/formats:image",
        "//cv-cuda-pipeline/framework/formats:image_frame",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
    ] + select({
        "//cv-cuda-pipeline/gpu:disable_gpu": [],
        "//conditions:default": [
            "//cv-cuda-pipeline/gpu:gpu_buffer",
        ],
    }),
    alwayslink = 1,
)


cc_test(
    name = "opencv_encoded_image_to_image_frame_calculator_test",
    srcs = ["opencv_encoded_image_to_image_frame_calculator_test.cc"],
    data = ["//cv-cuda-pipeline/calculators/image/testdata:test_images"],
    deps = [
        ":opencv_encoded_image_to_image_frame_calculator",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:calculator_runner",
        "//cv-cuda-pipeline/framework/deps:file_path",
        "//cv-cuda-pipeline/framework/formats:image_frame_opencv",
        "//cv-cuda-pipeline/framework/port:file_helpers",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:opencv_imgcodecs",
        "//cv-cuda-pipeline/framework/port:opencv_imgproc",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
        "@com_google_absl//absl/status",
    ],
)

cc_test(
    name = "opencv_image_encoder_calculator_test",
    srcs = ["opencv_image_encoder_calculator_test.cc"],
    data = ["//cv-cuda-pipeline/calculators/image/testdata:test_images"],
    deps = [
        ":opencv_image_encoder_calculator",
        ":opencv_image_encoder_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:calculator_runner",
        "//cv-cuda-pipeline/framework/deps:file_path",
        "//cv-cuda-pipeline/framework/formats:image_frame_opencv",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:opencv_imgcodecs",
        "//cv-cuda-pipeline/framework/port:opencv_imgproc",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
    ],
)

cc_test(
    name = "scale_image_utils_test",
    srcs = ["scale_image_utils_test.cc"],
    deps = [
        ":scale_image_utils",
        "//cv-cuda-pipeline/framework/port:gtest_main",
    ],
)

mediapipe_proto_library(
    name = "mask_overlay_calculator_proto",
    srcs = ["mask_overlay_calculator.proto"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_options_proto",
        "//cv-cuda-pipeline/framework:calculator_proto",
    ],
)

mediapipe_proto_library(
    name = "opencv_encoded_image_to_image_frame_calculator_proto",
    srcs = ["opencv_encoded_image_to_image_frame_calculator.proto"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_options_proto",
        "//cv-cuda-pipeline/framework:calculator_proto",
    ],
)

mediapipe_proto_library(
    name = "feature_detector_calculator_proto",
    srcs = ["feature_detector_calculator.proto"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_options_proto",
        "//cv-cuda-pipeline/framework:calculator_proto",
    ],
)

cc_library(
    name = "mask_overlay_calculator",
    srcs = ["mask_overlay_calculator.cc"],
    deps = [
        ":mask_overlay_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/gpu:gl_calculator_helper",
        "//cv-cuda-pipeline/gpu:gl_simple_shaders",
        "//cv-cuda-pipeline/gpu:shader_util",
    ],
    alwayslink = 1,
)


cc_library(
    name = "feature_detector_calculator",
    srcs = ["feature_detector_calculator.cc"],
    deps = [
        ":feature_detector_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/formats:image_frame",
        "//cv-cuda-pipeline/framework/formats:image_frame_opencv",
        "//cv-cuda-pipeline/framework/formats:landmark_cc_proto",
        "//cv-cuda-pipeline/framework/formats:video_stream_header",
        "//cv-cuda-pipeline/framework/port:integral_types",
        "//cv-cuda-pipeline/framework/port:logging",
        "//cv-cuda-pipeline/framework/port:opencv_core",
        "//cv-cuda-pipeline/framework/port:opencv_features2d",
        "//cv-cuda-pipeline/framework/port:opencv_imgproc",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/framework/port:threadpool",
        "//cv-cuda-pipeline/framework/tool:options_util",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/synchronization",
        "@litert//tflite:framework",
    ],
    alwayslink = 1,
)

cc_library(
    name = "image_file_properties_calculator",
    srcs = ["image_file_properties_calculator.cc"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/formats:image_file_properties_cc_proto",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "@easyexif",
    ],
    alwayslink = 1,
)

cc_test(
    name = "image_file_properties_calculator_test",
    srcs = ["image_file_properties_calculator_test.cc"],
    data = ["//cv-cuda-pipeline/calculators/image/testdata:test_images"],
    deps = [
        ":image_file_properties_calculator",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:calculator_runner",
        "//cv-cuda-pipeline/framework/deps:file_path",
        "//cv-cuda-pipeline/framework/formats:image_file_properties_cc_proto",
        "//cv-cuda-pipeline/framework/port:file_helpers",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
    ],
)

cc_library(
    name = "segmentation_smoothing_calculator",
    srcs = ["segmentation_smoothing_calculator.cc"],
    deps = [
        ":segmentation_smoothing_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:calculator_options_cc_proto",
        "//cv-cuda-pipeline/framework/formats:image",
        "//cv-cuda-pipeline/framework/formats:image_format_cc_proto",
        "//cv-cuda-pipeline/framework/formats:image_frame",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/framework/port:vector",
        "@com_google_absl//absl/log:absl_log",
    ] + select({
        "//cv-cuda-pipeline/gpu:disable_gpu": [],
        "//conditions:default": [
            "//cv-cuda-pipeline/gpu:gl_calculator_helper",
            "//cv-cuda-pipeline/gpu:gl_quad_renderer",
            "//cv-cuda-pipeline/gpu:gl_simple_shaders",
            "//cv-cuda-pipeline/gpu:shader_util",
        ],
    }) + select({
        "//cv-cuda-pipeline/framework/port:disable_opencv": [],
        "//conditions:default": [
            "//cv-cuda-pipeline/framework/formats:image_frame_opencv",
            "//cv-cuda-pipeline/framework/formats:image_opencv",
            "//cv-cuda-pipeline/framework/port:opencv_core",
        ],
    }),
    alwayslink = 1,
)

cc_test(
    name = "segmentation_smoothing_calculator_test",
    srcs = ["segmentation_smoothing_calculator_test.cc"],
    tags = [
        "noasan",  # TODO: Remove noasan tag when SwiftShader supports sanitizers
        "not_run:arm",
    ],
    deps = [
        ":image_clone_calculator",
        ":image_clone_calculator_cc_proto",
        ":segmentation_smoothing_calculator",
        ":segmentation_smoothing_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:calculator_runner",
        "//cv-cuda-pipeline/framework/deps:file_path",
        "//cv-cuda-pipeline/framework/formats:image_frame",
        "//cv-cuda-pipeline/framework/formats:image_opencv",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:opencv_imgcodecs",
        "//cv-cuda-pipeline/framework/port:opencv_imgproc",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
        "@com_google_absl//absl/log:absl_log",
    ],
)

cc_library(
    name = "affine_transformation",
    hdrs = ["affine_transformation.h"],
    deps = ["@com_google_absl//absl/status:statusor"],
)

cc_library(
    name = "affine_transformation_runner_gl",
    srcs = ["affine_transformation_runner_gl.cc"],
    hdrs = ["affine_transformation_runner_gl.h"],
    deps = [
        ":affine_transformation",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/gpu:gl_calculator_helper",
        "//cv-cuda-pipeline/gpu:gl_simple_shaders",
        "//cv-cuda-pipeline/gpu:gpu_buffer",
        "//cv-cuda-pipeline/gpu:gpu_origin_cc_proto",
        "//cv-cuda-pipeline/gpu:gpu_origin_utils",
        "//cv-cuda-pipeline/gpu:shader_util",
        "@com_google_absl//absl/log:absl_log",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@eigen//:eigen3",
    ],
)

cc_library(
    name = "affine_transformation_runner_opencv",
    srcs = ["affine_transformation_runner_opencv.cc"],
    hdrs = ["affine_transformation_runner_opencv.h"],
    deps = [
        ":affine_transformation",
        "//cv-cuda-pipeline/framework/formats:image_frame",
        "//cv-cuda-pipeline/framework/formats:image_frame_opencv",
        "//cv-cuda-pipeline/framework/port:opencv_core",
        "//cv-cuda-pipeline/framework/port:opencv_imgproc",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/status:statusor",
    ],
)

mediapipe_proto_library(
    name = "warp_affine_calculator_proto",
    srcs = ["warp_affine_calculator.proto"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_proto",
        "//cv-cuda-pipeline/gpu:gpu_origin_proto",
    ],
)

cc_library(
    name = "warp_affine_calculator",
    srcs = ["warp_affine_calculator.cc"],
    hdrs = ["warp_affine_calculator.h"],
    deps = [
        ":affine_transformation",
        ":warp_affine_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/api2:node",
        "//cv-cuda-pipeline/framework/api2:port",
        "//cv-cuda-pipeline/framework/api3:calculator",
        "//cv-cuda-pipeline/framework/api3:calculator_context",
        "//cv-cuda-pipeline/framework/api3:calculator_contract",
        "//cv-cuda-pipeline/framework/api3:contract",
        "//cv-cuda-pipeline/framework/api3:node",
        "//cv-cuda-pipeline/framework/formats:image",
        "//cv-cuda-pipeline/framework/formats:image_frame",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings:string_view",
    ] + select({
        "//cv-cuda-pipeline/gpu:disable_gpu": [],
        "//conditions:default": [
            ":affine_transformation_runner_gl",
            "//cv-cuda-pipeline/gpu:gl_calculator_helper",
            "//cv-cuda-pipeline/gpu:gpu_buffer",
            "//cv-cuda-pipeline/gpu:gpu_service",
        ],
    }) + select({
        "//cv-cuda-pipeline/framework/port:disable_opencv": [],
        "//conditions:default": [":affine_transformation_runner_opencv"],
    }),
    alwayslink = 1,
)


cc_library(
    name = "yuv_to_image_calculator",
    srcs = ["yuv_to_image_calculator.cc"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_context",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/api2:node",
        "//cv-cuda-pipeline/framework/formats:image",
        "//cv-cuda-pipeline/framework/formats:image_frame",
        "//cv-cuda-pipeline/framework/formats:yuv_image",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings:str_format",
        "@libyuv",
    ],
    alwayslink = 1,
)
