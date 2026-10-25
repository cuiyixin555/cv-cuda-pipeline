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

# Docs for Mediapipe Tasks Hand Landmarker
# https://developers.google.com/mediapipe/solutions/vision/hand_landmarker
cc_library(
    name = "hand_landmarker",
    srcs = ["hand_landmarker.cc"],
    hdrs = ["hand_landmarker.h"],
    deps = [
        ":hand_landmarker_graph",
        ":hand_landmarker_result",
        "//cv-cuda-pipeline/framework/api2:builder",
        "//cv-cuda-pipeline/framework/api2:port",
        "//cv-cuda-pipeline/framework/formats:classification_cc_proto",
        "//cv-cuda-pipeline/framework/formats:image",
        "//cv-cuda-pipeline/framework/formats:landmark_cc_proto",
        "//cv-cuda-pipeline/framework/formats:rect_cc_proto",
        "//cv-cuda-pipeline/tasks/cc:common",
        "//cv-cuda-pipeline/tasks/cc/components/processors:classifier_options",
        "//cv-cuda-pipeline/tasks/cc/components/processors/proto:classifier_options_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/core:base_options",
        "//cv-cuda-pipeline/tasks/cc/core:base_task_api",
        "//cv-cuda-pipeline/tasks/cc/core:model_resources",
        "//cv-cuda-pipeline/tasks/cc/core:task_runner",
        "//cv-cuda-pipeline/tasks/cc/core:utils",
        "//cv-cuda-pipeline/tasks/cc/core/proto:inference_subgraph_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/core:base_vision_task_api",
        "//cv-cuda-pipeline/tasks/cc/vision/core:image_processing_options",
        "//cv-cuda-pipeline/tasks/cc/vision/core:running_mode",
        "//cv-cuda-pipeline/tasks/cc/vision/core:vision_task_api_factory",
        "//cv-cuda-pipeline/tasks/cc/vision/hand_detector/proto:hand_detector_graph_options_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/hand_landmarker/proto:hand_landmarker_graph_options_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/hand_landmarker/proto:hand_landmarks_detector_graph_options_cc_proto",
        "@com_google_absl//absl/status:statusor",
    ],
)

cc_library(
    name = "hand_landmarks_detector_graph",
    srcs = ["hand_landmarks_detector_graph.cc"],
    deps = [
        "//cv-cuda-pipeline/tasks/cc/vision/hand_landmarker/proto:hand_landmarks_detector_graph_options_cc_proto",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "//cv-cuda-pipeline/calculators/core:split_vector_calculator",
        "//cv-cuda-pipeline/calculators/core:split_vector_calculator_cc_proto",
        "//cv-cuda-pipeline/calculators/image:image_properties_calculator",
        "//cv-cuda-pipeline/calculators/tensor:inference_calculator",
        "//cv-cuda-pipeline/calculators/tensor:tensors_to_classification_calculator",
        "//cv-cuda-pipeline/calculators/tensor:tensors_to_classification_calculator_cc_proto",
        "//cv-cuda-pipeline/calculators/tensor:tensors_to_floats_calculator",
        "//cv-cuda-pipeline/calculators/tensor:tensors_to_landmarks_calculator",
        "//cv-cuda-pipeline/calculators/tensor:tensors_to_landmarks_calculator_cc_proto",
        "//cv-cuda-pipeline/calculators/util:landmark_letterbox_removal_calculator",
        "//cv-cuda-pipeline/calculators/util:landmark_projection_calculator",
        "//cv-cuda-pipeline/calculators/util:rect_transformation_calculator",
        "//cv-cuda-pipeline/calculators/util:rect_transformation_calculator_cc_proto",
        "//cv-cuda-pipeline/calculators/util:thresholding_calculator",
        "//cv-cuda-pipeline/calculators/util:thresholding_calculator_cc_proto",
        "//cv-cuda-pipeline/calculators/util:world_landmark_projection_calculator",
        "//cv-cuda-pipeline/framework/api2:builder",
        "//cv-cuda-pipeline/framework/api2:port",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/framework/formats:classification_cc_proto",
        "//cv-cuda-pipeline/framework/formats:image",
        "//cv-cuda-pipeline/framework/formats:landmark_cc_proto",
        "//cv-cuda-pipeline/framework/formats:rect_cc_proto",
        "//cv-cuda-pipeline/framework/formats:tensor",
        # TODO: move calculators in modules/hand_landmark/calculators to tasks dir.
        "//cv-cuda-pipeline/modules/hand_landmark/calculators:hand_landmarks_to_rect_calculator",
        "//cv-cuda-pipeline/tasks/cc:common",
        "//cv-cuda-pipeline/tasks/cc/components/utils:gate",
        "//cv-cuda-pipeline/tasks/cc/components/processors:image_preprocessing_graph",
        "//cv-cuda-pipeline/tasks/cc/core:model_resources",
        "//cv-cuda-pipeline/tasks/cc/core:model_task_graph",
        "//cv-cuda-pipeline/tasks/cc/core:utils",
        "//cv-cuda-pipeline/tasks/cc/core/proto:inference_subgraph_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/utils:image_tensor_specs",
        "//cv-cuda-pipeline/util:label_map_cc_proto",
        "//cv-cuda-pipeline/util:label_map_util",
        "//cv-cuda-pipeline/tasks/cc/metadata:metadata_extractor",
        "//cv-cuda-pipeline/tasks/metadata:metadata_schema_cc",
    ],
    alwayslink = 1,
)

# TODO: Enable this test

cc_library(
    name = "hand_landmarker_graph",
    srcs = ["hand_landmarker_graph.cc"],
    deps = [
        ":hand_landmarks_detector_graph",
        "//cv-cuda-pipeline/calculators/core:begin_loop_calculator",
        "//cv-cuda-pipeline/calculators/core:clip_vector_size_calculator_cc_proto",
        "//cv-cuda-pipeline/calculators/core:end_loop_calculator",
        "//cv-cuda-pipeline/calculators/core:gate_calculator",
        "//cv-cuda-pipeline/calculators/core:gate_calculator_cc_proto",
        "//cv-cuda-pipeline/calculators/core:pass_through_calculator",
        "//cv-cuda-pipeline/calculators/core:previous_loopback_calculator",
        "//cv-cuda-pipeline/calculators/image:image_properties_calculator",
        "//cv-cuda-pipeline/calculators/util:collection_has_min_size_calculator",
        "//cv-cuda-pipeline/calculators/util:collection_has_min_size_calculator_cc_proto",
        "//cv-cuda-pipeline/framework/api2:builder",
        "//cv-cuda-pipeline/framework/api2:port",
        "//cv-cuda-pipeline/framework/formats:classification_cc_proto",
        "//cv-cuda-pipeline/framework/formats:detection_cc_proto",
        "//cv-cuda-pipeline/framework/formats:image",
        "//cv-cuda-pipeline/framework/formats:landmark_cc_proto",
        "//cv-cuda-pipeline/framework/formats:rect_cc_proto",
        "//cv-cuda-pipeline/framework/formats:tensor",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/tasks/cc:common",
        "//cv-cuda-pipeline/tasks/cc/components/utils:gate",
        "//cv-cuda-pipeline/tasks/cc/core:model_asset_bundle_resources",
        "//cv-cuda-pipeline/tasks/cc/core:model_resources_cache",
        "//cv-cuda-pipeline/tasks/cc/core:model_task_graph",
        "//cv-cuda-pipeline/tasks/cc/core:utils",
        "//cv-cuda-pipeline/tasks/cc/metadata/utils:zip_utils",
        "//cv-cuda-pipeline/tasks/cc/vision/hand_detector:hand_detector_graph",
        "//cv-cuda-pipeline/tasks/cc/vision/hand_detector/proto:hand_detector_graph_options_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/hand_landmarker/calculators:hand_association_calculator",
        "//cv-cuda-pipeline/tasks/cc/vision/hand_landmarker/calculators:hand_association_calculator_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/hand_landmarker/calculators:hand_landmarks_deduplication_calculator",
        "//cv-cuda-pipeline/tasks/cc/vision/hand_landmarker/proto:hand_landmarker_graph_options_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/hand_landmarker/proto:hand_landmarks_detector_graph_options_cc_proto",
        "//cv-cuda-pipeline/util:graph_builder_utils",
    ],
    alwayslink = 1,
)

cc_library(
    name = "hand_landmarks_connections",
    hdrs = ["hand_landmarks_connections.h"],
)

# TODO: open source hand joints graph

cc_library(
    name = "hand_roi_refinement_graph",
    srcs = ["hand_roi_refinement_graph.cc"],
    deps = [
        "//cv-cuda-pipeline/calculators/tensor:image_to_tensor_calculator_cc_proto",
        "//cv-cuda-pipeline/calculators/tensor:inference_calculator",
        "//cv-cuda-pipeline/calculators/tensor:tensors_to_landmarks_calculator",
        "//cv-cuda-pipeline/calculators/tensor:tensors_to_landmarks_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/api2:builder",
        "//cv-cuda-pipeline/framework/api2/stream:detections_to_rects",
        "//cv-cuda-pipeline/framework/api2/stream:landmarks_projection",
        "//cv-cuda-pipeline/framework/api2/stream:landmarks_to_detection",
        "//cv-cuda-pipeline/framework/api2/stream:rect_transformation",
        "//cv-cuda-pipeline/framework/formats:image",
        "//cv-cuda-pipeline/framework/formats:landmark_cc_proto",
        "//cv-cuda-pipeline/framework/formats:rect_cc_proto",
        "//cv-cuda-pipeline/framework/formats:tensor",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/framework/port:statusor",
        "//cv-cuda-pipeline/tasks/cc/components/processors:image_preprocessing_graph",
        "//cv-cuda-pipeline/tasks/cc/components/processors/proto:image_preprocessing_graph_options_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/core:model_task_graph",
        "//cv-cuda-pipeline/tasks/cc/vision/hand_landmarker/proto:hand_roi_refinement_graph_options_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/utils:image_tensor_specs",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
    ],
    alwayslink = 1,
)

cc_library(
    name = "hand_landmarker_result",
    srcs = ["hand_landmarker_result.cc"],
    hdrs = ["hand_landmarker_result.h"],
    deps = [
        "//cv-cuda-pipeline/framework/formats:classification_cc_proto",
        "//cv-cuda-pipeline/framework/formats:landmark_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/components/containers:classification_result",
        "//cv-cuda-pipeline/tasks/cc/components/containers:landmark",
    ],
)

# TODO: Enable this test

cc_library(
    name = "hand_topology",
    hdrs = ["hand_topology.h"],
)
