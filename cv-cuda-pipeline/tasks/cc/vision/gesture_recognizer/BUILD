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

# Docs for Mediapipe Tasks Gesture Recognizer
# https://developers.google.com/mediapipe/solutions/vision/gesture_recognizer
cc_library(
    name = "gesture_recognizer",
    srcs = ["gesture_recognizer.cc"],
    hdrs = ["gesture_recognizer.h"],
    deps = [
        ":gesture_recognizer_graph",
        ":gesture_recognizer_result",
        ":hand_gesture_recognizer_graph",
        "//cv-cuda-pipeline/framework:packet",
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
        "//cv-cuda-pipeline/tasks/cc/vision/gesture_recognizer/proto:gesture_classifier_graph_options_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/gesture_recognizer/proto:gesture_recognizer_graph_options_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/gesture_recognizer/proto:hand_gesture_recognizer_graph_options_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/hand_detector/proto:hand_detector_graph_options_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/hand_landmarker/proto:hand_landmarker_graph_options_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/hand_landmarker/proto:hand_landmarks_detector_graph_options_cc_proto",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@litert//tflite/kernels:builtin_ops",
    ],
)

cc_library(
    name = "handedness_util",
    srcs = ["handedness_util.cc"],
    hdrs = ["handedness_util.h"],
    deps = [
        "//cv-cuda-pipeline/framework/formats:classification_cc_proto",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
    ],
)

cc_test(
    name = "handedness_util_test",
    srcs = ["handedness_util_test.cc"],
    deps = [
        ":handedness_util",
        "//cv-cuda-pipeline/framework/formats:classification_cc_proto",
        "//cv-cuda-pipeline/framework/port:gtest_main",
    ],
)

cc_library(
    name = "hand_gesture_recognizer_graph",
    srcs = ["hand_gesture_recognizer_graph.cc"],
    deps = [
        "//cv-cuda-pipeline/calculators/core:begin_loop_calculator",
        "//cv-cuda-pipeline/calculators/core:concatenate_vector_calculator",
        "//cv-cuda-pipeline/calculators/core:end_loop_calculator",
        "//cv-cuda-pipeline/calculators/core:get_vector_item_calculator",
        "//cv-cuda-pipeline/calculators/tensor:tensor_converter_calculator",
        "//cv-cuda-pipeline/calculators/tensor:tensors_to_classification_calculator",
        "//cv-cuda-pipeline/calculators/tensor:tensors_to_classification_calculator_cc_proto",
        "//cv-cuda-pipeline/framework/api2:builder",
        "//cv-cuda-pipeline/framework/api2:port",
        "//cv-cuda-pipeline/framework/formats:classification_cc_proto",
        "//cv-cuda-pipeline/framework/formats:landmark_cc_proto",
        "//cv-cuda-pipeline/framework/formats:matrix",
        "//cv-cuda-pipeline/framework/formats:rect_cc_proto",
        "//cv-cuda-pipeline/framework/formats:tensor",
        "//cv-cuda-pipeline/tasks/cc:common",
        "//cv-cuda-pipeline/tasks/cc/components/processors:classification_postprocessing_graph",
        "//cv-cuda-pipeline/tasks/cc/core:model_asset_bundle_resources",
        "//cv-cuda-pipeline/tasks/cc/core:model_resources",
        "//cv-cuda-pipeline/tasks/cc/core:model_resources_cache",
        "//cv-cuda-pipeline/tasks/cc/core:model_task_graph",
        "//cv-cuda-pipeline/tasks/cc/core:utils",
        "//cv-cuda-pipeline/tasks/cc/core/proto:base_options_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/core/proto:external_file_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/core/proto:inference_subgraph_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/metadata/utils:zip_utils",
        "//cv-cuda-pipeline/tasks/cc/vision/gesture_recognizer/calculators:combined_prediction_calculator",
        "//cv-cuda-pipeline/tasks/cc/vision/gesture_recognizer/calculators:combined_prediction_calculator_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/gesture_recognizer/calculators:handedness_to_matrix_calculator",
        "//cv-cuda-pipeline/tasks/cc/vision/gesture_recognizer/calculators:landmarks_to_matrix_calculator",
        "//cv-cuda-pipeline/tasks/cc/vision/gesture_recognizer/calculators:landmarks_to_matrix_calculator_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/gesture_recognizer/proto:gesture_classifier_graph_options_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/gesture_recognizer/proto:gesture_embedder_graph_options_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/gesture_recognizer/proto:hand_gesture_recognizer_graph_options_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/hand_landmarker:hand_landmarks_detector_graph",
        "//cv-cuda-pipeline/tasks/metadata:metadata_schema_cc",
        "@com_google_absl//absl/log:absl_log",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
    ],
    alwayslink = 1,
)

cc_library(
    name = "gesture_recognizer_graph",
    srcs = ["gesture_recognizer_graph.cc"],
    deps = [
        ":hand_gesture_recognizer_graph",
        "//cv-cuda-pipeline/calculators/core:vector_indices_calculator",
        "//cv-cuda-pipeline/calculators/image:image_properties_calculator",
        "//cv-cuda-pipeline/framework/api2:builder",
        "//cv-cuda-pipeline/framework/api2:port",
        "//cv-cuda-pipeline/framework/formats:classification_cc_proto",
        "//cv-cuda-pipeline/framework/formats:detection_cc_proto",
        "//cv-cuda-pipeline/framework/formats:image",
        "//cv-cuda-pipeline/framework/formats:landmark_cc_proto",
        "//cv-cuda-pipeline/framework/formats:rect_cc_proto",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/tasks/cc:common",
        "//cv-cuda-pipeline/tasks/cc/components/processors/proto:classifier_options_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/core:model_asset_bundle_resources",
        "//cv-cuda-pipeline/tasks/cc/core:model_resources_cache",
        "//cv-cuda-pipeline/tasks/cc/core:model_task_graph",
        "//cv-cuda-pipeline/tasks/cc/core:utils",
        "//cv-cuda-pipeline/tasks/cc/metadata/utils:zip_utils",
        "//cv-cuda-pipeline/tasks/cc/vision/gesture_recognizer/proto:gesture_recognizer_graph_options_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/gesture_recognizer/proto:hand_gesture_recognizer_graph_options_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/hand_detector:hand_detector_graph",
        "//cv-cuda-pipeline/tasks/cc/vision/hand_detector/proto:hand_detector_graph_options_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/hand_landmarker:hand_landmarker_graph",
        "//cv-cuda-pipeline/tasks/cc/vision/hand_landmarker:hand_landmarks_detector_graph",
        "//cv-cuda-pipeline/tasks/cc/vision/hand_landmarker/proto:hand_landmarker_graph_options_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/hand_landmarker/proto:hand_landmarks_detector_graph_options_cc_proto",
        "//cv-cuda-pipeline/tasks/metadata:metadata_schema_cc",
        "@com_google_absl//absl/log:absl_log",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
    ],
    alwayslink = 1,
)

cc_library(
    name = "gesture_recognizer_result",
    hdrs = ["gesture_recognizer_result.h"],
    deps = [
        "//cv-cuda-pipeline/framework/formats:classification_cc_proto",
        "//cv-cuda-pipeline/framework/formats:landmark_cc_proto",
    ],
)
