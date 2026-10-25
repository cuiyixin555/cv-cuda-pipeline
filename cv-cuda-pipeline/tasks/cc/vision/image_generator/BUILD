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

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "conditioned_image_graph",
    srcs = ["conditioned_image_graph.cc"],
    deps = [
        "//cv-cuda-pipeline/calculators/core:get_vector_item_calculator",
        "//cv-cuda-pipeline/calculators/core:get_vector_item_calculator_cc_proto",
        "//cv-cuda-pipeline/calculators/util:annotation_overlay_calculator",
        "//cv-cuda-pipeline/calculators/util:flat_color_image_calculator",
        "//cv-cuda-pipeline/calculators/util:flat_color_image_calculator_cc_proto",
        "//cv-cuda-pipeline/calculators/util:landmarks_to_render_data_calculator",
        "//cv-cuda-pipeline/calculators/util:landmarks_to_render_data_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/api2:builder",
        "//cv-cuda-pipeline/framework/formats:image",
        "//cv-cuda-pipeline/framework/formats:image_format_cc_proto",
        "//cv-cuda-pipeline/framework/formats:image_frame_opencv",
        "//cv-cuda-pipeline/framework/formats:landmark_cc_proto",
        "//cv-cuda-pipeline/framework/port:opencv_core",
        "//cv-cuda-pipeline/framework/port:opencv_imgcodecs",
        "//cv-cuda-pipeline/framework/port:opencv_imgproc",
        "//cv-cuda-pipeline/tasks/cc/core:model_task_graph",
        "//cv-cuda-pipeline/tasks/cc/vision/face_detector/proto:face_detector_graph_options_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/face_landmarker:face_landmarker_graph",
        "//cv-cuda-pipeline/tasks/cc/vision/face_landmarker:face_landmarks_connections",
        "//cv-cuda-pipeline/tasks/cc/vision/face_landmarker/proto:face_landmarker_graph_options_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/image_generator/proto:conditioned_image_graph_options_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/image_segmenter:image_segmenter_graph",
        "//cv-cuda-pipeline/tasks/cc/vision/image_segmenter/proto:image_segmenter_graph_options_cc_proto",
        "//cv-cuda-pipeline/util:color_cc_proto",
        "//cv-cuda-pipeline/util:image_frame_util",
        "//cv-cuda-pipeline/util:render_data_cc_proto",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings",
    ],
    alwayslink = 1,
)

cc_library(
    name = "image_generator_graph",
    srcs = ["image_generator_graph.cc"],
    deps = [
        ":conditioned_image_graph",
        "//cv-cuda-pipeline/calculators/core:pass_through_calculator",
        "//cv-cuda-pipeline/calculators/image:image_transformation_calculator",
        "//cv-cuda-pipeline/calculators/image:image_transformation_calculator_cc_proto",
        "//cv-cuda-pipeline/calculators/tensor:image_to_tensor_calculator",
        "//cv-cuda-pipeline/calculators/tensor:image_to_tensor_calculator_cc_proto",
        "//cv-cuda-pipeline/calculators/tensor:inference_calculator",
        "//cv-cuda-pipeline/calculators/tensor:inference_calculator_cc_proto",
        "//cv-cuda-pipeline/calculators/util:from_image_calculator",
        "//cv-cuda-pipeline/calculators/util:to_image_calculator",
        "//cv-cuda-pipeline/framework:calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:stream_handler_cc_proto",
        "//cv-cuda-pipeline/framework/api2:builder",
        "//cv-cuda-pipeline/framework/api2:port",
        "//cv-cuda-pipeline/framework/deps:file_path",
        "//cv-cuda-pipeline/framework/formats:image",
        "//cv-cuda-pipeline/framework/formats:tensor",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/framework/tool:switch_container",
        "//cv-cuda-pipeline/framework/tool:switch_container_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/core:model_asset_bundle_resources",
        "//cv-cuda-pipeline/tasks/cc/core:model_resources",
        "//cv-cuda-pipeline/tasks/cc/core:model_task_graph",
        "//cv-cuda-pipeline/tasks/cc/core/proto:external_file_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/image_generator/diffuser:diffusion_plugins_output_calculator",
        "//cv-cuda-pipeline/tasks/cc/vision/image_generator/diffuser:stable_diffusion_iterate_calculator",
        "//cv-cuda-pipeline/tasks/cc/vision/image_generator/diffuser:stable_diffusion_iterate_calculator_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/image_generator/proto:conditioned_image_graph_options_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/image_generator/proto:control_plugin_graph_options_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/image_generator/proto:image_generator_graph_options_cc_proto",
        "//cv-cuda-pipeline/util:graph_builder_utils",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings",
    ],
    alwayslink = 1,
)

cc_library(
    name = "image_generator_result",
    hdrs = ["image_generator_result.h"],
    deps = ["//cv-cuda-pipeline/framework/formats:image"],
)

cc_library(
    name = "image_generator",
    srcs = ["image_generator.cc"],
    hdrs = ["image_generator.h"],
    deps = [
        ":image_generator_graph",
        ":image_generator_result",
        "//cv-cuda-pipeline/framework:packet",
        "//cv-cuda-pipeline/framework:timestamp",
        "//cv-cuda-pipeline/framework/api2:builder",
        "//cv-cuda-pipeline/framework/api2:port",
        "//cv-cuda-pipeline/framework/formats:image",
        "//cv-cuda-pipeline/framework/formats:tensor",
        "//cv-cuda-pipeline/tasks/cc/core:base_options",
        "//cv-cuda-pipeline/tasks/cc/core:running_mode",
        "//cv-cuda-pipeline/tasks/cc/core:task_runner",
        "//cv-cuda-pipeline/tasks/cc/core/proto:external_file_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/core:base_vision_task_api",
        "//cv-cuda-pipeline/tasks/cc/vision/core:vision_task_api_factory",
        "//cv-cuda-pipeline/tasks/cc/vision/face_detector/proto:face_detector_graph_options_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/face_landmarker",
        "//cv-cuda-pipeline/tasks/cc/vision/face_landmarker/proto:face_landmarker_graph_options_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/face_landmarker/proto:face_landmarks_detector_graph_options_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/image_generator/diffuser:stable_diffusion_iterate_calculator_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/image_generator/proto:conditioned_image_graph_options_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/image_generator/proto:control_plugin_graph_options_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/image_generator/proto:image_generator_graph_options_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/image_segmenter",
        "//cv-cuda-pipeline/tasks/cc/vision/image_segmenter/proto:image_segmenter_graph_options_cc_proto",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_absl//absl/time",
    ],
)
