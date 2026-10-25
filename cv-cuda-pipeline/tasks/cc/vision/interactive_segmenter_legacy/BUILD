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

package(default_visibility = ["//visibility:public"])

licenses(["notice"])

cc_library(
    name = "interactive_segmenter_legacy",
    srcs = ["interactive_segmenter_legacy.cc"],
    hdrs = ["interactive_segmenter_legacy.h"],
    deps = [
        ":interactive_segmenter_legacy_graph",
        "//cv-cuda-pipeline/framework:calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/api2:builder",
        "//cv-cuda-pipeline/framework/formats:image",
        "//cv-cuda-pipeline/framework/formats:rect_cc_proto",
        "//cv-cuda-pipeline/tasks/cc:common",
        "//cv-cuda-pipeline/tasks/cc/components/containers:keypoint",
        "//cv-cuda-pipeline/tasks/cc/core:base_options",
        "//cv-cuda-pipeline/tasks/cc/core:running_mode",
        "//cv-cuda-pipeline/tasks/cc/vision/core:base_vision_task_api",
        "//cv-cuda-pipeline/tasks/cc/vision/core:image_processing_options",
        "//cv-cuda-pipeline/tasks/cc/vision/core:vision_task_api_factory",
        "//cv-cuda-pipeline/tasks/cc/vision/image_segmenter:image_segmenter_result",
        "//cv-cuda-pipeline/tasks/cc/vision/image_segmenter/proto:image_segmenter_graph_options_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/image_segmenter/proto:segmenter_options_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/interactive_segmenter_legacy/proto:region_of_interest_cc_proto",
        "//cv-cuda-pipeline/util:color_cc_proto",
        "//cv-cuda-pipeline/util:render_data_cc_proto",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
    ],
)

cc_library(
    name = "interactive_segmenter_legacy_graph",
    srcs = ["interactive_segmenter_legacy_graph.cc"],
    hdrs = ["interactive_segmenter_legacy_graph.h"],
    deps = [
        "//cv-cuda-pipeline/calculators/image:image_transformation_calculator",
        "//cv-cuda-pipeline/calculators/image:set_alpha_calculator",
        "//cv-cuda-pipeline/calculators/util:annotation_overlay_calculator",
        "//cv-cuda-pipeline/calculators/util:flat_color_image_calculator",
        "//cv-cuda-pipeline/calculators/util:flat_color_image_calculator_cc_proto",
        "//cv-cuda-pipeline/calculators/util:from_image_calculator",
        "//cv-cuda-pipeline/calculators/util:to_image_calculator",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/api2:builder",
        "//cv-cuda-pipeline/framework/api2:node",
        "//cv-cuda-pipeline/framework/api2:port",
        "//cv-cuda-pipeline/framework/formats:image",
        "//cv-cuda-pipeline/framework/formats:rect_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/components/processors:image_preprocessing_graph",
        "//cv-cuda-pipeline/tasks/cc/core:model_task_graph",
        "//cv-cuda-pipeline/tasks/cc/vision/image_segmenter:image_segmenter_graph",
        "//cv-cuda-pipeline/tasks/cc/vision/image_segmenter/proto:image_segmenter_graph_options_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/image_segmenter/proto:segmenter_options_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/interactive_segmenter_legacy/proto:region_of_interest_cc_proto",
        "//cv-cuda-pipeline/util:color_cc_proto",
        "//cv-cuda-pipeline/util:graph_builder_utils",
        "//cv-cuda-pipeline/util:label_map_cc_proto",
        "//cv-cuda-pipeline/util:render_data_cc_proto",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
    ] + select({
        "//cv-cuda-pipeline/gpu:disable_gpu": [],
        "//conditions:default": [
            "//cv-cuda-pipeline/gpu:gpu_buffer_to_image_frame_calculator",
            "//cv-cuda-pipeline/gpu:image_frame_to_gpu_buffer_calculator",
        ],
    }),
    alwayslink = 1,
)
