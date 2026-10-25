load("@rules_cc//cc:cc_library.bzl", "cc_library")
load("@rules_cc//cc:cc_test.bzl", "cc_test")

# Copyright 2025 The MediaPipe Authors.
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

package(default_visibility = ["//visibility:public"])

licenses(["notice"])

cc_library(
    name = "face_detector_graph",
    srcs = ["face_detector_graph.cc"],
    deps = [
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "//cv-cuda-pipeline/calculators/core:clip_vector_size_calculator",
        "//cv-cuda-pipeline/calculators/core:clip_vector_size_calculator_cc_proto",
        "//cv-cuda-pipeline/calculators/tensor:image_to_tensor_calculator_cc_proto",
        "//cv-cuda-pipeline/calculators/tensor:inference_calculator",
        # Registers InferenceCalculatorLiteRt, which this graph runs inference
        # on. Without it InferenceCalculatorSelectorImpl silently falls back to
        # the GL/CPU implementations.
        "//cv-cuda-pipeline/calculators/tensor:inference_calculator_litert",  # buildcleaner: keep
        "//cv-cuda-pipeline/calculators/tensor:tensors_to_detections_calculator",
        "//cv-cuda-pipeline/calculators/tensor:tensors_to_detections_calculator_cc_proto",
        "//cv-cuda-pipeline/calculators/tflite:ssd_anchors_calculator",
        "//cv-cuda-pipeline/calculators/tflite:ssd_anchors_calculator_cc_proto",
        "//cv-cuda-pipeline/calculators/util:detection_projection_calculator",
        "//cv-cuda-pipeline/calculators/util:detection_transformation_calculator",
        "//cv-cuda-pipeline/calculators/util:detections_to_rects_calculator",
        "//cv-cuda-pipeline/calculators/util:detections_to_rects_calculator_cc_proto",
        "//cv-cuda-pipeline/calculators/util:non_max_suppression_calculator",
        "//cv-cuda-pipeline/calculators/util:non_max_suppression_calculator_cc_proto",
        "//cv-cuda-pipeline/calculators/util:rect_transformation_calculator",
        "//cv-cuda-pipeline/calculators/util:rect_transformation_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/api2:builder",
        "//cv-cuda-pipeline/framework/api2:port",
        "//cv-cuda-pipeline/framework/formats:detection_cc_proto",
        "//cv-cuda-pipeline/framework/formats:image",
        "//cv-cuda-pipeline/framework/formats:rect_cc_proto",
        "//cv-cuda-pipeline/framework/formats:tensor",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/tasks/cc/components/processors:image_preprocessing_graph",
        "//cv-cuda-pipeline/tasks/cc/core:model_resources",
        "//cv-cuda-pipeline/tasks/cc/core:model_task_graph",
        "//cv-cuda-pipeline/tasks/cc/core/proto:inference_subgraph_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/metadata:metadata_extractor",
        "//cv-cuda-pipeline/tasks/cc/vision/face_detector/proto:face_detector_graph_options_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/utils:image_tensor_specs",
        "//cv-cuda-pipeline/tasks/metadata:face_detector_metadata_schema_cc",
    ],
    alwayslink = 1,
)

cc_library(
    name = "face_detector",
    srcs = ["face_detector.cc"],
    hdrs = ["face_detector.h"],
    deps = [
        ":face_detector_graph",
        "//cv-cuda-pipeline/framework/api2:builder",
        "//cv-cuda-pipeline/framework/formats:detection_cc_proto",
        "//cv-cuda-pipeline/framework/formats:image",
        "//cv-cuda-pipeline/tasks/cc/components/containers:detection_result",
        "//cv-cuda-pipeline/tasks/cc/core:base_options",
        "//cv-cuda-pipeline/tasks/cc/core:utils",
        "//cv-cuda-pipeline/tasks/cc/vision/core:base_vision_task_api",
        "//cv-cuda-pipeline/tasks/cc/vision/core:image_processing_options",
        "//cv-cuda-pipeline/tasks/cc/vision/core:running_mode",
        "//cv-cuda-pipeline/tasks/cc/vision/core:vision_task_api_factory",
        "//cv-cuda-pipeline/tasks/cc/vision/face_detector/proto:face_detector_graph_options_cc_proto",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
    ],
)
