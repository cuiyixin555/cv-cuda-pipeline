# Copyright 2026 The MediaPipe Authors.
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

package(
    default_visibility = ["//visibility:public"],
    licenses = ["notice"],  # Apache 2.0
)

cc_library(
    name = "holistic_face_tracking",
    srcs = ["holistic_face_tracking.cc"],
    hdrs = ["holistic_face_tracking.h"],
    deps = [
        "//cv-cuda-pipeline/framework/api2:builder",
        "//cv-cuda-pipeline/framework/api2/stream:detections_to_rects",
        "//cv-cuda-pipeline/framework/api2/stream:image_size",
        "//cv-cuda-pipeline/framework/api2/stream:landmarks_to_detection",
        "//cv-cuda-pipeline/framework/api2/stream:loopback",
        "//cv-cuda-pipeline/framework/api2/stream:rect_transformation",
        "//cv-cuda-pipeline/framework/formats:classification_cc_proto",
        "//cv-cuda-pipeline/framework/formats:detection_cc_proto",
        "//cv-cuda-pipeline/framework/formats:image",
        "//cv-cuda-pipeline/framework/formats:landmark_cc_proto",
        "//cv-cuda-pipeline/framework/formats:rect_cc_proto",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/modules/holistic_landmark/calculators:roi_tracking_calculator",
        "//cv-cuda-pipeline/modules/holistic_landmark/calculators:roi_tracking_calculator_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/face_detector:face_detector_graph",
        "//cv-cuda-pipeline/tasks/cc/vision/face_detector/proto:face_detector_graph_options_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/face_landmarker:face_blendshapes_graph",
        "//cv-cuda-pipeline/tasks/cc/vision/face_landmarker:face_landmarker_graph",
        "//cv-cuda-pipeline/tasks/cc/vision/face_landmarker:face_landmarks_detector_graph",
        "//cv-cuda-pipeline/tasks/cc/vision/face_landmarker/proto:face_blendshapes_graph_options_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/face_landmarker/proto:face_landmarks_detector_graph_options_cc_proto",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings:str_format",
    ],
)

cc_library(
    name = "holistic_hand_tracking",
    srcs = ["holistic_hand_tracking.cc"],
    hdrs = ["holistic_hand_tracking.h"],
    deps = [
        "//cv-cuda-pipeline/calculators/util:align_hand_to_pose_in_world_calculator",
        "//cv-cuda-pipeline/calculators/util:align_hand_to_pose_in_world_calculator_cc_proto",
        "//cv-cuda-pipeline/calculators/util:landmark_visibility_calculator",
        "//cv-cuda-pipeline/framework/api2:builder",
        "//cv-cuda-pipeline/framework/api2/stream:image_size",
        "//cv-cuda-pipeline/framework/api2/stream:landmarks_to_detection",
        "//cv-cuda-pipeline/framework/api2/stream:loopback",
        "//cv-cuda-pipeline/framework/api2/stream:rect_transformation",
        "//cv-cuda-pipeline/framework/api2/stream:split",
        "//cv-cuda-pipeline/framework/api2/stream:threshold",
        "//cv-cuda-pipeline/framework/formats:image",
        "//cv-cuda-pipeline/framework/formats:landmark_cc_proto",
        "//cv-cuda-pipeline/framework/formats:rect_cc_proto",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/modules/holistic_landmark/calculators:hand_detections_from_pose_to_rects_calculator",
        "//cv-cuda-pipeline/modules/holistic_landmark/calculators:roi_tracking_calculator",
        "//cv-cuda-pipeline/modules/holistic_landmark/calculators:roi_tracking_calculator_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/components/utils:gate",
        "//cv-cuda-pipeline/tasks/cc/vision/hand_landmarker:hand_landmarker_graph",
        "//cv-cuda-pipeline/tasks/cc/vision/hand_landmarker:hand_landmarks_detector_graph",
        "//cv-cuda-pipeline/tasks/cc/vision/hand_landmarker:hand_roi_refinement_graph",
        "//cv-cuda-pipeline/tasks/cc/vision/hand_landmarker/proto:hand_landmarker_graph_options_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/hand_landmarker/proto:hand_landmarks_detector_graph_options_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/hand_landmarker/proto:hand_roi_refinement_graph_options_cc_proto",
        "@com_google_absl//absl/status:statusor",
    ],
)

cc_library(
    name = "holistic_pose_tracking",
    srcs = ["holistic_pose_tracking.cc"],
    hdrs = ["holistic_pose_tracking.h"],
    deps = [
        "//cv-cuda-pipeline/framework/api2:builder",
        "//cv-cuda-pipeline/framework/api2/stream:detections_to_rects",
        "//cv-cuda-pipeline/framework/api2/stream:image_size",
        "//cv-cuda-pipeline/framework/api2/stream:landmarks_to_detection",
        "//cv-cuda-pipeline/framework/api2/stream:loopback",
        "//cv-cuda-pipeline/framework/api2/stream:merge",
        "//cv-cuda-pipeline/framework/api2/stream:presence",
        "//cv-cuda-pipeline/framework/api2/stream:rect_transformation",
        "//cv-cuda-pipeline/framework/api2/stream:segmentation_smoothing",
        "//cv-cuda-pipeline/framework/api2/stream:smoothing",
        "//cv-cuda-pipeline/framework/api2/stream:split",
        "//cv-cuda-pipeline/framework/formats:detection_cc_proto",
        "//cv-cuda-pipeline/framework/formats:image",
        "//cv-cuda-pipeline/framework/formats:landmark_cc_proto",
        "//cv-cuda-pipeline/framework/formats:rect_cc_proto",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/tasks/cc/components/utils:gate",
        "//cv-cuda-pipeline/tasks/cc/vision/pose_detector:pose_detector_graph",
        "//cv-cuda-pipeline/tasks/cc/vision/pose_detector/proto:pose_detector_graph_options_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/pose_landmarker:pose_landmarks_detector_graph",
        "//cv-cuda-pipeline/tasks/cc/vision/pose_landmarker/proto:pose_landmarks_detector_graph_options_cc_proto",
        "@com_google_absl//absl/status:statusor",
    ],
)

cc_library(
    name = "holistic_landmarker_result",
    hdrs = ["holistic_landmarker_result.h"],
    deps = [
        "//cv-cuda-pipeline/framework/formats:classification_cc_proto",
        "//cv-cuda-pipeline/framework/formats:image",
        "//cv-cuda-pipeline/framework/formats:landmark_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/components/containers:category",
        "//cv-cuda-pipeline/tasks/cc/components/containers:landmark",
    ],
)

cc_library(
    name = "holistic_landmarker",
    srcs = ["holistic_landmarker.cc"],
    hdrs = ["holistic_landmarker.h"],
    deps = [
        ":holistic_landmarker_graph",
        ":holistic_landmarker_result",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/api2:builder",
        "//cv-cuda-pipeline/framework/formats:classification_cc_proto",
        "//cv-cuda-pipeline/framework/formats:image",
        "//cv-cuda-pipeline/framework/formats:landmark_cc_proto",
        "//cv-cuda-pipeline/framework/formats:rect_cc_proto",
        "//cv-cuda-pipeline/tasks/cc:common",
        "//cv-cuda-pipeline/tasks/cc/components/containers:category",
        "//cv-cuda-pipeline/tasks/cc/components/containers:landmark",
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
        "//cv-cuda-pipeline/tasks/cc/vision/face_detector/proto:face_detector_graph_options_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/face_landmarker/proto:face_landmarks_detector_graph_options_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/hand_landmarker/proto:hand_landmarks_detector_graph_options_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/holistic_landmarker/proto:holistic_landmarker_graph_options_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/pose_detector/proto:pose_detector_graph_options_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/pose_landmarker/proto:pose_landmarks_detector_graph_options_cc_proto",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
    ],
)

cc_library(
    name = "holistic_landmarker_graph",
    srcs = ["holistic_landmarker_graph.cc"],
    deps = [
        ":holistic_face_tracking",
        ":holistic_hand_tracking",
        ":holistic_pose_tracking",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/api2:builder",
        "//cv-cuda-pipeline/framework/api2/stream:split",
        "//cv-cuda-pipeline/framework/formats:classification_cc_proto",
        "//cv-cuda-pipeline/framework/formats:image",
        "//cv-cuda-pipeline/framework/formats:landmark_cc_proto",
        "//cv-cuda-pipeline/framework/formats:rect_cc_proto",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/tasks/cc/core:model_asset_bundle_resources",
        "//cv-cuda-pipeline/tasks/cc/core:model_resources_cache",
        "//cv-cuda-pipeline/tasks/cc/core:model_task_graph",
        "//cv-cuda-pipeline/tasks/cc/core:utils",
        "//cv-cuda-pipeline/tasks/cc/metadata/utils:zip_utils",
        "//cv-cuda-pipeline/tasks/cc/vision/face_detector/proto:face_detector_graph_options_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/face_landmarker/proto:face_blendshapes_graph_options_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/face_landmarker/proto:face_landmarks_detector_graph_options_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/hand_landmarker/proto:hand_landmarks_detector_graph_options_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/hand_landmarker/proto:hand_roi_refinement_graph_options_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/holistic_landmarker/proto:holistic_landmarker_graph_options_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/pose_detector/proto:pose_detector_graph_options_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/pose_landmarker:pose_topology",
        "//cv-cuda-pipeline/tasks/cc/vision/pose_landmarker/proto:pose_landmarks_detector_graph_options_cc_proto",
        "//cv-cuda-pipeline/util:graph_builder_utils",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
    ],
    alwayslink = 1,
)
