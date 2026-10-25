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

package(default_visibility = ["//visibility:public"])

licenses(["notice"])

cc_library(
    name = "image_segmenter_result",
    hdrs = ["image_segmenter_result.h"],
    deps = ["//cv-cuda-pipeline/framework/formats:image"],
)

# Docs for Mediapipe Tasks Image Segmenter
# https://developers.google.com/mediapipe/solutions/vision/image_segmenter
cc_library(
    name = "image_segmenter",
    srcs = ["image_segmenter.cc"],
    hdrs = ["image_segmenter.h"],
    deps = [
        ":image_segmenter_graph",
        ":image_segmenter_result",
        "//cv-cuda-pipeline/framework:calculator_cc_proto",
        "//cv-cuda-pipeline/framework/api2:builder",
        "//cv-cuda-pipeline/framework/formats:image",
        "//cv-cuda-pipeline/framework/formats:rect_cc_proto",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/tasks/cc/core:base_options",
        "//cv-cuda-pipeline/tasks/cc/core:utils",
        "//cv-cuda-pipeline/tasks/cc/vision/core:base_vision_task_api",
        "//cv-cuda-pipeline/tasks/cc/vision/core:image_processing_options",
        "//cv-cuda-pipeline/tasks/cc/vision/core:running_mode",
        "//cv-cuda-pipeline/tasks/cc/vision/core:vision_task_api_factory",
        "//cv-cuda-pipeline/tasks/cc/vision/image_segmenter/calculators:tensors_to_segmentation_calculator_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/image_segmenter/proto:image_segmenter_graph_options_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/image_segmenter/proto:segmenter_options_cc_proto",
        "//cv-cuda-pipeline/util:label_map_cc_proto",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings:str_format",
        "@litert//tflite/kernels:builtin_ops",
    ],
)

cc_library(
    name = "image_segmenter_graph",
    srcs = ["image_segmenter_graph.cc"],
    deps = [
        "//cv-cuda-pipeline/calculators/core:merge_to_vector_calculator",
        "//cv-cuda-pipeline/calculators/image:image_clone_calculator_cc_proto",
        "//cv-cuda-pipeline/calculators/image:image_properties_calculator",
        "//cv-cuda-pipeline/calculators/image:image_transformation_calculator",
        "//cv-cuda-pipeline/calculators/image:image_transformation_calculator_cc_proto",
        "//cv-cuda-pipeline/calculators/image:set_alpha_calculator",
        "//cv-cuda-pipeline/calculators/image:set_alpha_calculator_cc_proto",
        "//cv-cuda-pipeline/calculators/tensor:image_to_tensor_calculator",
        "//cv-cuda-pipeline/calculators/tensor:inference_calculator",
        "//cv-cuda-pipeline/calculators/tensor:tensor_converter_calculator",
        "//cv-cuda-pipeline/calculators/tensor:tensor_converter_calculator_cc_proto",
        "//cv-cuda-pipeline/calculators/tensor:tensors_dequantization_calculator",
        "//cv-cuda-pipeline/calculators/util:from_image_calculator",
        "//cv-cuda-pipeline/framework/api2:builder",
        "//cv-cuda-pipeline/framework/api2:port",
        "//cv-cuda-pipeline/framework/formats:image",
        "//cv-cuda-pipeline/framework/formats:rect_cc_proto",
        "//cv-cuda-pipeline/framework/formats:tensor",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/tasks/cc:common",
        "//cv-cuda-pipeline/tasks/cc/components/processors:image_preprocessing_graph",
        "//cv-cuda-pipeline/tasks/cc/components/processors/proto:image_preprocessing_graph_options_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/core:model_resources",
        "//cv-cuda-pipeline/tasks/cc/core:model_task_graph",
        "//cv-cuda-pipeline/tasks/cc/core/proto:inference_subgraph_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/metadata:metadata_extractor",
        "//cv-cuda-pipeline/tasks/cc/vision/image_segmenter/calculators:tensors_to_segmentation_calculator",
        "//cv-cuda-pipeline/tasks/cc/vision/image_segmenter/calculators:tensors_to_segmentation_calculator_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/image_segmenter/proto:image_segmenter_graph_options_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/image_segmenter/proto:segmenter_options_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/utils:image_tensor_specs",
        "//cv-cuda-pipeline/tasks/metadata:image_segmenter_metadata_schema_cc",
        "//cv-cuda-pipeline/tasks/metadata:metadata_schema_cc",
        "//cv-cuda-pipeline/util:graph_builder_utils",
        "//cv-cuda-pipeline/util:label_map_cc_proto",
        "//cv-cuda-pipeline/util:label_map_util",
        "@com_google_absl//absl/log:absl_log",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings:str_format",
        "@litert//tflite/schema:schema_fbs",
    ],
    alwayslink = 1,
)

# TODO: This test fails in OSS
