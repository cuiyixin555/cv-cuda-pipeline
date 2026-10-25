load("@rules_cc//cc:cc_library.bzl", "cc_library")
load("@rules_cc//cc:cc_test.bzl", "cc_test")
load("//cv-cuda-pipeline/framework/port:build_config.bzl", "mediapipe_proto_library")

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

licenses(["notice"])

package(default_visibility = [
    "//cv-cuda-pipeline/examples:__subpackages__",
])

mediapipe_proto_library(
    name = "cropping_proto",
    srcs = ["cropping.proto"],
    deps = [
        "//cv-cuda-pipeline/examples/desktop/autoflip:autoflip_messages_proto",
        "//cv-cuda-pipeline/examples/desktop/autoflip/quality:kinematic_path_solver_proto",
    ],
)

mediapipe_proto_library(
    name = "kinematic_path_solver_proto",
    srcs = ["kinematic_path_solver.proto"],
    visibility = [
        "//cv-cuda-pipeline/examples:__subpackages__",
    ],
)

mediapipe_proto_library(
    name = "focus_point_proto",
    srcs = ["focus_point.proto"],
    visibility = ["//cv-cuda-pipeline/examples:__subpackages__"],
)

cc_library(
    name = "frame_crop_region_computer",
    srcs = ["frame_crop_region_computer.cc"],
    hdrs = ["frame_crop_region_computer.h"],
    deps = [
        ":cropping_cc_proto",
        ":utils",
        "//cv-cuda-pipeline/examples/desktop/autoflip:autoflip_messages_cc_proto",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "@com_google_absl//absl/log:absl_log",
    ],
)

cc_library(
    name = "math_utils",
    hdrs = ["math_utils.h"],
)

cc_library(
    name = "piecewise_linear_function",
    srcs = ["piecewise_linear_function.cc"],
    hdrs = ["piecewise_linear_function.h"],
    deps = [
        "//cv-cuda-pipeline/framework/port:status",
        "@com_google_absl//absl/log:absl_check",
    ],
)

cc_library(
    name = "padding_effect_generator",
    srcs = ["padding_effect_generator.cc"],
    hdrs = ["padding_effect_generator.h"],
    deps = [
        "//cv-cuda-pipeline/framework/formats:image_frame",
        "//cv-cuda-pipeline/framework/formats:image_frame_opencv",
        "//cv-cuda-pipeline/framework/port:opencv_core",
        "//cv-cuda-pipeline/framework/port:opencv_imgproc",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "@com_google_absl//absl/flags:flag",
    ],
)

cc_library(
    name = "scene_camera_motion_analyzer",
    srcs = ["scene_camera_motion_analyzer.cc"],
    hdrs = ["scene_camera_motion_analyzer.h"],
    deps = [
        ":cropping_cc_proto",
        ":focus_point_cc_proto",
        ":math_utils",
        ":piecewise_linear_function",
        ":utils",
        "//cv-cuda-pipeline/examples/desktop/autoflip:autoflip_messages_cc_proto",
        "//cv-cuda-pipeline/framework:timestamp",
        "//cv-cuda-pipeline/framework/port:integral_types",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
    ],
)

cc_library(
    name = "scene_cropping_viz",
    srcs = ["scene_cropping_viz.cc"],
    hdrs = ["scene_cropping_viz.h"],
    deps = [
        ":cropping_cc_proto",
        ":focus_point_cc_proto",
        "//cv-cuda-pipeline/examples/desktop/autoflip:autoflip_messages_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/formats:image_format_cc_proto",
        "//cv-cuda-pipeline/framework/formats:image_frame",
        "//cv-cuda-pipeline/framework/formats:image_frame_opencv",
        "//cv-cuda-pipeline/framework/port:opencv_core",
        "//cv-cuda-pipeline/framework/port:opencv_imgproc",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "@com_google_absl//absl/memory",
    ],
)

cc_library(
    name = "polynomial_regression_path_solver",
    srcs = ["polynomial_regression_path_solver.cc"],
    hdrs = ["polynomial_regression_path_solver.h"],
    deps = [
        ":focus_point_cc_proto",
        "//cv-cuda-pipeline/framework/port:opencv_core",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "@ceres_solver//:ceres",
    ],
)

cc_library(
    name = "kinematic_path_solver",
    srcs = ["kinematic_path_solver.cc"],
    hdrs = ["kinematic_path_solver.h"],
    visibility = [
        "//cv-cuda-pipeline/examples:__subpackages__",
    ],
    deps = [
        ":kinematic_path_solver_cc_proto",
        "//cv-cuda-pipeline/framework/port:integral_types",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
    ],
)

cc_test(
    name = "kinematic_path_solver_test",
    srcs = ["kinematic_path_solver_test.cc"],
    deps = [
        ":kinematic_path_solver",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:integral_types",
        "//cv-cuda-pipeline/framework/port:status",
    ],
)

cc_library(
    name = "scene_cropper",
    srcs = ["scene_cropper.cc"],
    hdrs = ["scene_cropper.h"],
    deps = [
        ":cropping_cc_proto",
        ":focus_point_cc_proto",
        ":kinematic_path_solver",
        ":polynomial_regression_path_solver",
        ":utils",
        "//cv-cuda-pipeline/framework/port:opencv_core",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "@com_google_absl//absl/memory",
    ],
)

cc_library(
    name = "utils",
    srcs = ["utils.cc"],
    hdrs = ["utils.h"],
    deps = [
        ":cropping_cc_proto",
        ":math_utils",
        ":piecewise_linear_function",
        "//cv-cuda-pipeline/examples/desktop/autoflip:autoflip_messages_cc_proto",
        "//cv-cuda-pipeline/framework/port:opencv_core",
        "//cv-cuda-pipeline/framework/port:opencv_imgproc",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "@com_google_absl//absl/log:absl_log",
        "@com_google_absl//absl/memory",
    ],
)

cc_test(
    name = "frame_crop_region_computer_test",
    srcs = ["frame_crop_region_computer_test.cc"],
    deps = [
        ":cropping_cc_proto",
        ":frame_crop_region_computer",
        "//cv-cuda-pipeline/examples/desktop/autoflip:autoflip_messages_cc_proto",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "@com_google_absl//absl/memory",
    ],
)

cc_test(
    name = "piecewise_linear_function_test",
    srcs = ["piecewise_linear_function_test.cc"],
    deps = [
        ":piecewise_linear_function",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:status",
    ],
)

cc_test(
    name = "scene_camera_motion_analyzer_test",
    srcs = ["scene_camera_motion_analyzer_test.cc"],
    data = [
        "//cv-cuda-pipeline/examples/desktop/autoflip/quality/testdata:camera_motion_tracking_scene_frame_results.csv",
    ],
    deps = [
        ":focus_point_cc_proto",
        ":piecewise_linear_function",
        ":scene_camera_motion_analyzer",
        "//cv-cuda-pipeline/examples/desktop/autoflip:autoflip_messages_cc_proto",
        "//cv-cuda-pipeline/framework/deps:file_path",
        "//cv-cuda-pipeline/framework/port:file_helpers",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:status",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/log:absl_check",
        "@com_google_absl//absl/strings",
    ],
)

cc_test(
    name = "padding_effect_generator_test",
    srcs = ["padding_effect_generator_test.cc"],
    data = [
        "//cv-cuda-pipeline/examples/desktop/autoflip/quality/testdata:google.jpg",
        "//cv-cuda-pipeline/examples/desktop/autoflip/quality/testdata:result_0.3.jpg",
        "//cv-cuda-pipeline/examples/desktop/autoflip/quality/testdata:result_0.3_solid_background.jpg",
        "//cv-cuda-pipeline/examples/desktop/autoflip/quality/testdata:result_0.6.jpg",
        "//cv-cuda-pipeline/examples/desktop/autoflip/quality/testdata:result_0.6_solid_background.jpg",
        "//cv-cuda-pipeline/examples/desktop/autoflip/quality/testdata:result_1.6.jpg",
        "//cv-cuda-pipeline/examples/desktop/autoflip/quality/testdata:result_1.6_solid_background.jpg",
        "//cv-cuda-pipeline/examples/desktop/autoflip/quality/testdata:result_1.jpg",
        "//cv-cuda-pipeline/examples/desktop/autoflip/quality/testdata:result_1_solid_background.jpg",
        "//cv-cuda-pipeline/examples/desktop/autoflip/quality/testdata:result_2.5.jpg",
        "//cv-cuda-pipeline/examples/desktop/autoflip/quality/testdata:result_2.5_solid_background.jpg",
        "//cv-cuda-pipeline/examples/desktop/autoflip/quality/testdata:result_3.4.jpg",
        "//cv-cuda-pipeline/examples/desktop/autoflip/quality/testdata:result_3.4_solid_background.jpg",
    ],
    deps = [
        ":padding_effect_generator",
        "//cv-cuda-pipeline/framework/deps:file_path",
        "//cv-cuda-pipeline/framework/formats:image_frame",
        "//cv-cuda-pipeline/framework/formats:image_frame_opencv",
        "//cv-cuda-pipeline/framework/port:file_helpers",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:opencv_imgcodecs",
        "//cv-cuda-pipeline/framework/port:opencv_imgproc",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/strings",
    ],
)

cc_test(
    name = "polynomial_regression_path_solver_test",
    srcs = ["polynomial_regression_path_solver_test.cc"],
    deps = [
        ":focus_point_cc_proto",
        ":polynomial_regression_path_solver",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:opencv_core",
        "//cv-cuda-pipeline/framework/port:status",
        "@com_google_absl//absl/log:absl_check",
    ],
)

cc_test(
    name = "scene_cropper_test",
    size = "small",
    timeout = "short",
    srcs = ["scene_cropper_test.cc"],
    deps = [
        ":focus_point_cc_proto",
        ":scene_cropper",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:opencv_core",
    ],
)

cc_test(
    name = "utils_test",
    srcs = ["utils_test.cc"],
    deps = [
        ":cropping_cc_proto",
        ":utils",
        "//cv-cuda-pipeline/examples/desktop/autoflip:autoflip_messages_cc_proto",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:integral_types",
        "//cv-cuda-pipeline/framework/port:opencv_core",
    ],
)

mediapipe_proto_library(
    name = "visual_scorer_proto",
    srcs = ["visual_scorer.proto"],
    visibility = ["//cv-cuda-pipeline/examples:__subpackages__"],
)

cc_library(
    name = "visual_scorer",
    srcs = ["visual_scorer.cc"],
    hdrs = ["visual_scorer.h"],
    deps = [
        ":visual_scorer_cc_proto",
        "//cv-cuda-pipeline/examples/desktop/autoflip:autoflip_messages_cc_proto",
        "//cv-cuda-pipeline/framework/port:opencv_core",
        "//cv-cuda-pipeline/framework/port:opencv_imgproc",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "@com_google_absl//absl/log:absl_log",
    ],
)

cc_test(
    name = "visual_scorer_test",
    srcs = [
        "visual_scorer_test.cc",
    ],
    linkstatic = 1,
    deps = [
        ":visual_scorer",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:opencv_core",
        "//cv-cuda-pipeline/framework/port:opencv_imgproc",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
        "//cv-cuda-pipeline/framework/port:status",
    ],
)
