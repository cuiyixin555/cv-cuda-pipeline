# Copyright 2020 The MediaPipe Authors.
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

load(
    "//cv-cuda-pipeline/framework/tool:mediapipe_files.bzl",
    "mediapipe_files",
)
load(
    "//cv-cuda-pipeline/framework/tool:mediapipe_graph.bzl",
    "mediapipe_simple_subgraph",
)

licenses(["notice"])

package(default_visibility = ["//visibility:public"])

mediapipe_files(srcs = [
    "hand_landmark_full.tflite",
    "hand_landmark_lite.tflite",
])

exports_files(srcs = ["handedness.txt"])

mediapipe_simple_subgraph(
    name = "hand_landmark_model_loader",
    graph = "hand_landmark_model_loader.pbtxt",
    register_as = "HandLandmarkModelLoader",
    deps = [
        "//cv-cuda-pipeline/calculators/core:constant_side_packet_calculator",
        "//cv-cuda-pipeline/calculators/tflite:tflite_model_calculator",
        "//cv-cuda-pipeline/calculators/util:resource_provider_calculator",
        "//cv-cuda-pipeline/framework/tool:switch_container",
    ],
)

mediapipe_simple_subgraph(
    name = "hand_landmark_cpu",
    graph = "hand_landmark_cpu.pbtxt",
    register_as = "HandLandmarkCpu",
    deps = [
        ":hand_landmark_model_loader",
        "//cv-cuda-pipeline/calculators/core:gate_calculator",
        "//cv-cuda-pipeline/calculators/core:split_vector_calculator",
        "//cv-cuda-pipeline/calculators/tensor:image_to_tensor_calculator",
        "//cv-cuda-pipeline/calculators/tensor:inference_calculator",
        "//cv-cuda-pipeline/calculators/tensor:tensors_to_classification_calculator",
        "//cv-cuda-pipeline/calculators/tensor:tensors_to_floats_calculator",
        "//cv-cuda-pipeline/calculators/tensor:tensors_to_landmarks_calculator",
        "//cv-cuda-pipeline/calculators/util:landmark_letterbox_removal_calculator",
        "//cv-cuda-pipeline/calculators/util:landmark_projection_calculator",
        "//cv-cuda-pipeline/calculators/util:thresholding_calculator",
        "//cv-cuda-pipeline/calculators/util:world_landmark_projection_calculator",
    ],
)

mediapipe_simple_subgraph(
    name = "hand_landmark_gpu",
    graph = "hand_landmark_gpu.pbtxt",
    register_as = "HandLandmarkGpu",
    deps = [
        ":hand_landmark_model_loader",
        "//cv-cuda-pipeline/calculators/core:gate_calculator",
        "//cv-cuda-pipeline/calculators/core:split_vector_calculator",
        "//cv-cuda-pipeline/calculators/tensor:image_to_tensor_calculator",
        "//cv-cuda-pipeline/calculators/tensor:inference_calculator",
        "//cv-cuda-pipeline/calculators/tensor:tensors_to_classification_calculator",
        "//cv-cuda-pipeline/calculators/tensor:tensors_to_floats_calculator",
        "//cv-cuda-pipeline/calculators/tensor:tensors_to_landmarks_calculator",
        "//cv-cuda-pipeline/calculators/util:landmark_letterbox_removal_calculator",
        "//cv-cuda-pipeline/calculators/util:landmark_projection_calculator",
        "//cv-cuda-pipeline/calculators/util:thresholding_calculator",
        "//cv-cuda-pipeline/calculators/util:world_landmark_projection_calculator",
    ],
)

mediapipe_simple_subgraph(
    name = "hand_landmark_tracking_gpu",
    graph = "hand_landmark_tracking_gpu.pbtxt",
    register_as = "HandLandmarkTrackingGpu",
    deps = [
        ":hand_landmark_gpu",
        ":hand_landmark_landmarks_to_roi",
        ":palm_detection_detection_to_roi",
        "//cv-cuda-pipeline/calculators/core:begin_loop_calculator",
        "//cv-cuda-pipeline/calculators/core:clip_vector_size_calculator",
        "//cv-cuda-pipeline/calculators/core:constant_side_packet_calculator",
        "//cv-cuda-pipeline/calculators/core:end_loop_calculator",
        "//cv-cuda-pipeline/calculators/core:flow_limiter_calculator",
        "//cv-cuda-pipeline/calculators/core:gate_calculator",
        "//cv-cuda-pipeline/calculators/core:previous_loopback_calculator",
        "//cv-cuda-pipeline/calculators/image:image_properties_calculator",
        "//cv-cuda-pipeline/calculators/util:association_norm_rect_calculator",
        "//cv-cuda-pipeline/calculators/util:collection_has_min_size_calculator",
        "//cv-cuda-pipeline/calculators/util:filter_collection_calculator",
        "//cv-cuda-pipeline/modules/palm_detection:palm_detection_gpu",
    ],
)

mediapipe_simple_subgraph(
    name = "hand_landmark_tracking_cpu_image",
    graph = "hand_landmark_tracking_cpu_image.pbtxt",
    register_as = "HandLandmarkTrackingCpuImage",
    deps = [
        ":hand_landmark_tracking_cpu",
        "//cv-cuda-pipeline/calculators/core:flow_limiter_calculator",
        "//cv-cuda-pipeline/calculators/image:image_transformation_calculator",
        "//cv-cuda-pipeline/calculators/util:from_image_calculator",
    ],
)

mediapipe_simple_subgraph(
    name = "hand_landmark_tracking_gpu_image",
    graph = "hand_landmark_tracking_gpu_image.pbtxt",
    register_as = "HandLandmarkTrackingGpuImage",
    deps = [
        ":hand_landmark_tracking_gpu",
        "//cv-cuda-pipeline/calculators/core:flow_limiter_calculator",
        "//cv-cuda-pipeline/calculators/image:image_transformation_calculator",
        "//cv-cuda-pipeline/calculators/util:from_image_calculator",
    ],
)

mediapipe_simple_subgraph(
    name = "hand_landmark_tracking_cpu",
    graph = "hand_landmark_tracking_cpu.pbtxt",
    register_as = "HandLandmarkTrackingCpu",
    deps = [
        ":hand_landmark_cpu",
        ":hand_landmark_landmarks_to_roi",
        ":palm_detection_detection_to_roi",
        "//cv-cuda-pipeline/calculators/core:begin_loop_calculator",
        "//cv-cuda-pipeline/calculators/core:clip_vector_size_calculator",
        "//cv-cuda-pipeline/calculators/core:constant_side_packet_calculator",
        "//cv-cuda-pipeline/calculators/core:end_loop_calculator",
        "//cv-cuda-pipeline/calculators/core:flow_limiter_calculator",
        "//cv-cuda-pipeline/calculators/core:gate_calculator",
        "//cv-cuda-pipeline/calculators/core:previous_loopback_calculator",
        "//cv-cuda-pipeline/calculators/image:image_properties_calculator",
        "//cv-cuda-pipeline/calculators/util:association_norm_rect_calculator",
        "//cv-cuda-pipeline/calculators/util:collection_has_min_size_calculator",
        "//cv-cuda-pipeline/calculators/util:filter_collection_calculator",
        "//cv-cuda-pipeline/modules/palm_detection:palm_detection_cpu",
    ],
)

mediapipe_simple_subgraph(
    name = "palm_detection_detection_to_roi",
    graph = "palm_detection_detection_to_roi.pbtxt",
    register_as = "PalmDetectionDetectionToRoi",
    deps = [
        "//cv-cuda-pipeline/calculators/util:detections_to_rects_calculator",
        "//cv-cuda-pipeline/calculators/util:rect_transformation_calculator",
    ],
)

mediapipe_simple_subgraph(
    name = "hand_landmark_landmarks_to_roi",
    graph = "hand_landmark_landmarks_to_roi.pbtxt",
    register_as = "HandLandmarkLandmarksToRoi",
    deps = [
        "//cv-cuda-pipeline/calculators/util:rect_transformation_calculator",
        "//cv-cuda-pipeline/modules/hand_landmark/calculators:hand_landmarks_to_rect_calculator",
    ],
)
