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
    name = "tensors_to_face_landmarks_graph",
    srcs = ["tensors_to_face_landmarks_graph.cc"],
    deps = [
        "//cv-cuda-pipeline/calculators/core:split_vector_calculator",
        "//cv-cuda-pipeline/calculators/core:split_vector_calculator_cc_proto",
        "//cv-cuda-pipeline/calculators/tensor:tensors_to_landmarks_calculator",
        "//cv-cuda-pipeline/calculators/tensor:tensors_to_landmarks_calculator_cc_proto",
        "//cv-cuda-pipeline/calculators/util:landmarks_refinement_calculator",
        "//cv-cuda-pipeline/calculators/util:landmarks_refinement_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:subgraph",
        "//cv-cuda-pipeline/framework/api2:builder",
        "//cv-cuda-pipeline/framework/formats:landmark_cc_proto",
        "//cv-cuda-pipeline/framework/formats:tensor",
        "//cv-cuda-pipeline/tasks/cc/vision/face_landmarker/proto:tensors_to_face_landmarks_graph_options_cc_proto",
    ],
    alwayslink = 1,
)

cc_library(
    name = "face_blendshapes_graph",
    srcs = ["face_blendshapes_graph.cc"],
    deps = [
        "//cv-cuda-pipeline/calculators/core:split_proto_list_calculator",
        "//cv-cuda-pipeline/calculators/core:split_vector_calculator",
        "//cv-cuda-pipeline/calculators/core:split_vector_calculator_cc_proto",
        "//cv-cuda-pipeline/calculators/tensor:inference_calculator",
        "//cv-cuda-pipeline/calculators/tensor:landmarks_to_tensor_calculator",
        "//cv-cuda-pipeline/calculators/tensor:landmarks_to_tensor_calculator_cc_proto",
        "//cv-cuda-pipeline/calculators/tensor:tensors_to_classification_calculator",
        "//cv-cuda-pipeline/calculators/tensor:tensors_to_classification_calculator_cc_proto",
        "//cv-cuda-pipeline/framework/api2:builder",
        "//cv-cuda-pipeline/framework/api2:port",
        "//cv-cuda-pipeline/framework/formats:classification_cc_proto",
        "//cv-cuda-pipeline/framework/formats:landmark_cc_proto",
        "//cv-cuda-pipeline/framework/formats:tensor",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/tasks/cc/core:model_asset_bundle_resources",
        "//cv-cuda-pipeline/tasks/cc/core:model_resources_cache",
        "//cv-cuda-pipeline/tasks/cc/core:model_task_graph",
        "//cv-cuda-pipeline/tasks/cc/vision/face_landmarker/proto:face_blendshapes_graph_options_cc_proto",
    ],
    alwayslink = 1,
)

cc_library(
    name = "face_landmarks_detector_graph",
    srcs = ["face_landmarks_detector_graph.cc"],
    deps = [
        ":face_blendshapes_graph",
        ":tensors_to_face_landmarks_graph",
        "//cv-cuda-pipeline/calculators/core:begin_loop_calculator",
        "//cv-cuda-pipeline/calculators/core:end_loop_calculator",
        "//cv-cuda-pipeline/calculators/core:get_vector_item_calculator",
        "//cv-cuda-pipeline/calculators/core:get_vector_item_calculator_cc_proto",
        "//cv-cuda-pipeline/calculators/core:split_vector_calculator",
        "//cv-cuda-pipeline/calculators/core:split_vector_calculator_cc_proto",
        "//cv-cuda-pipeline/calculators/image:image_properties_calculator",
        "//cv-cuda-pipeline/calculators/tensor:image_to_tensor_calculator_cc_proto",
        "//cv-cuda-pipeline/calculators/tensor:inference_calculator",
        "//cv-cuda-pipeline/calculators/tensor:tensors_to_floats_calculator",
        "//cv-cuda-pipeline/calculators/tensor:tensors_to_floats_calculator_cc_proto",
        "//cv-cuda-pipeline/calculators/tensor:tensors_to_landmarks_calculator",
        "//cv-cuda-pipeline/calculators/tensor:tensors_to_landmarks_calculator_cc_proto",
        "//cv-cuda-pipeline/calculators/util:detections_to_rects_calculator",
        "//cv-cuda-pipeline/calculators/util:detections_to_rects_calculator_cc_proto",
        "//cv-cuda-pipeline/calculators/util:landmark_letterbox_removal_calculator",
        "//cv-cuda-pipeline/calculators/util:landmark_projection_calculator",
        "//cv-cuda-pipeline/calculators/util:landmarks_smoothing_calculator",
        "//cv-cuda-pipeline/calculators/util:landmarks_smoothing_calculator_cc_proto",
        "//cv-cuda-pipeline/calculators/util:landmarks_to_detection_calculator",
        "//cv-cuda-pipeline/calculators/util:rect_transformation_calculator",
        "//cv-cuda-pipeline/calculators/util:rect_transformation_calculator_cc_proto",
        "//cv-cuda-pipeline/calculators/util:thresholding_calculator",
        "//cv-cuda-pipeline/calculators/util:thresholding_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:calculator_options_cc_proto",
        "//cv-cuda-pipeline/framework/api2:builder",
        "//cv-cuda-pipeline/framework/api2:port",
        "//cv-cuda-pipeline/framework/formats:classification_cc_proto",
        "//cv-cuda-pipeline/framework/formats:image",
        "//cv-cuda-pipeline/framework/formats:landmark_cc_proto",
        "//cv-cuda-pipeline/framework/formats:rect_cc_proto",
        "//cv-cuda-pipeline/framework/formats:tensor",
        "//cv-cuda-pipeline/tasks/cc:common",
        "//cv-cuda-pipeline/tasks/cc/components/processors:image_preprocessing_graph",
        "//cv-cuda-pipeline/tasks/cc/components/utils:gate",
        "//cv-cuda-pipeline/tasks/cc/core:model_resources",
        "//cv-cuda-pipeline/tasks/cc/core:model_task_graph",
        "//cv-cuda-pipeline/tasks/cc/core:utils",
        "//cv-cuda-pipeline/tasks/cc/vision/face_landmarker/proto:face_blendshapes_graph_options_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/face_landmarker/proto:face_landmarks_detector_graph_options_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/face_landmarker/proto:tensors_to_face_landmarks_graph_options_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/utils:image_tensor_specs",
        "//cv-cuda-pipeline/util:graph_builder_utils",
    ],
    alwayslink = 1,
)

cc_library(
    name = "face_landmarker_result",
    srcs = ["face_landmarker_result.cc"],
    hdrs = ["face_landmarker_result.h"],
    deps = [
        "//cv-cuda-pipeline/framework/formats:classification_cc_proto",
        "//cv-cuda-pipeline/framework/formats:landmark_cc_proto",
        "//cv-cuda-pipeline/framework/formats:matrix",
        "//cv-cuda-pipeline/framework/formats:matrix_data_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/components/containers:classification_result",
        "//cv-cuda-pipeline/tasks/cc/components/containers:landmark",
    ],
)

cc_library(
    name = "face_landmarker",
    srcs = ["face_landmarker.cc"],
    hdrs = ["face_landmarker.h"],
    deps = [
        ":face_landmarker_graph",
        ":face_landmarker_result",
        "//cv-cuda-pipeline/framework/api2:builder",
        "//cv-cuda-pipeline/framework/formats:classification_cc_proto",
        "//cv-cuda-pipeline/framework/formats:image",
        "//cv-cuda-pipeline/framework/formats:landmark_cc_proto",
        "//cv-cuda-pipeline/framework/formats:matrix",
        "//cv-cuda-pipeline/framework/formats:matrix_data_cc_proto",
        "//cv-cuda-pipeline/framework/formats:rect_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/components/containers:classification_result",
        "//cv-cuda-pipeline/tasks/cc/core:base_options",
        "//cv-cuda-pipeline/tasks/cc/core:base_task_api",
        "//cv-cuda-pipeline/tasks/cc/core:task_runner",
        "//cv-cuda-pipeline/tasks/cc/core:utils",
        "//cv-cuda-pipeline/tasks/cc/vision/core:base_vision_task_api",
        "//cv-cuda-pipeline/tasks/cc/vision/core:image_processing_options",
        "//cv-cuda-pipeline/tasks/cc/vision/core:running_mode",
        "//cv-cuda-pipeline/tasks/cc/vision/core:vision_task_api_factory",
        "//cv-cuda-pipeline/tasks/cc/vision/face_detector/proto:face_detector_graph_options_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/face_geometry/proto:face_geometry_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/face_landmarker/proto:face_landmarker_graph_options_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/face_landmarker/proto:face_landmarks_detector_graph_options_cc_proto",
        "@com_google_absl//absl/status:statusor",
    ],
)

cc_library(
    name = "face_landmarker_graph",
    srcs = ["face_landmarker_graph.cc"],
    deps = [
        ":face_landmarks_detector_graph",
        "//cv-cuda-pipeline/calculators/core:begin_loop_calculator",
        "//cv-cuda-pipeline/calculators/core:clip_vector_size_calculator_cc_proto",
        "//cv-cuda-pipeline/calculators/core:concatenate_vector_calculator",
        "//cv-cuda-pipeline/calculators/core:end_loop_calculator",
        "//cv-cuda-pipeline/calculators/core:gate_calculator",
        "//cv-cuda-pipeline/calculators/core:gate_calculator_cc_proto",
        "//cv-cuda-pipeline/calculators/core:get_vector_item_calculator",
        "//cv-cuda-pipeline/calculators/core:get_vector_item_calculator_cc_proto",
        "//cv-cuda-pipeline/calculators/core:pass_through_calculator",
        "//cv-cuda-pipeline/calculators/core:previous_loopback_calculator",
        "//cv-cuda-pipeline/calculators/image:image_properties_calculator",
        "//cv-cuda-pipeline/calculators/util:association_calculator_cc_proto",
        "//cv-cuda-pipeline/calculators/util:association_norm_rect_calculator",
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
        "//cv-cuda-pipeline/tasks/cc/vision/face_detector:face_detector_graph",
        "//cv-cuda-pipeline/tasks/cc/vision/face_detector/proto:face_detector_graph_options_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/face_geometry:face_geometry_from_landmarks_graph",
        "//cv-cuda-pipeline/tasks/cc/vision/face_geometry/calculators:geometry_pipeline_calculator_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/face_geometry/proto:environment_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/face_geometry/proto:face_geometry_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/face_geometry/proto:face_geometry_graph_options_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/face_landmarker/proto:face_blendshapes_graph_options_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/face_landmarker/proto:face_landmarker_graph_options_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/face_landmarker/proto:face_landmarks_detector_graph_options_cc_proto",
        "//cv-cuda-pipeline/util:graph_builder_utils",
        "@com_google_absl//absl/log:absl_log",
        "@com_google_absl//absl/strings:str_format",
    ],
    alwayslink = 1,
)

cc_library(
    name = "face_landmarks_connections",
    hdrs = ["face_landmarks_connections.h"],
)
