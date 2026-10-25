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
    name = "pose_detector_graph",
    srcs = ["pose_detector_graph.cc"],
    deps = [
        "//cv-cuda-pipeline/calculators/core:clip_vector_size_calculator",
        "//cv-cuda-pipeline/calculators/core:clip_vector_size_calculator_cc_proto",
        "//cv-cuda-pipeline/calculators/tensor:image_to_tensor_calculator_cc_proto",
        "//cv-cuda-pipeline/calculators/tensor:inference_calculator",
        "//cv-cuda-pipeline/calculators/tensor:tensors_to_detections_calculator",
        "//cv-cuda-pipeline/calculators/tensor:tensors_to_detections_calculator_cc_proto",
        "//cv-cuda-pipeline/calculators/tflite:ssd_anchors_calculator",
        "//cv-cuda-pipeline/calculators/tflite:ssd_anchors_calculator_cc_proto",
        "//cv-cuda-pipeline/calculators/util:alignment_points_to_rects_calculator",
        "//cv-cuda-pipeline/calculators/util:detection_letterbox_removal_calculator",
        "//cv-cuda-pipeline/calculators/util:detection_projection_calculator",
        "//cv-cuda-pipeline/calculators/util:detection_transformation_calculator",
        "//cv-cuda-pipeline/calculators/util:detections_to_rects_calculator",
        "//cv-cuda-pipeline/calculators/util:detections_to_rects_calculator_cc_proto",
        "//cv-cuda-pipeline/calculators/util:non_max_suppression_calculator",
        "//cv-cuda-pipeline/calculators/util:non_max_suppression_calculator_cc_proto",
        "//cv-cuda-pipeline/calculators/util:rect_transformation_calculator",
        "//cv-cuda-pipeline/calculators/util:rect_transformation_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_cc_proto",
        "//cv-cuda-pipeline/framework:subgraph",
        "//cv-cuda-pipeline/framework/api2:builder",
        "//cv-cuda-pipeline/framework/formats:detection_cc_proto",
        "//cv-cuda-pipeline/framework/formats:image",
        "//cv-cuda-pipeline/framework/formats:rect_cc_proto",
        "//cv-cuda-pipeline/framework/formats:tensor",
        "//cv-cuda-pipeline/tasks/cc/components/processors:image_preprocessing_graph",
        "//cv-cuda-pipeline/tasks/cc/core:model_task_graph",
        "//cv-cuda-pipeline/tasks/cc/vision/pose_detector/proto:pose_detector_graph_options_cc_proto",
        "@com_google_absl//absl/status:statusor",
    ],
    alwayslink = 1,
)
