# Copyright 2019 The MediaPipe Authors.
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

mediapipe_simple_subgraph(
    name = "face_landmark_cpu",
    graph = "face_landmark_cpu.pbtxt",
    register_as = "FaceLandmarkCpu",
    deps = [
        ":face_landmarks_model_loader",
        ":tensors_to_face_landmarks",
        ":tensors_to_face_landmarks_with_attention",
        "//cv-cuda-pipeline/calculators/core:gate_calculator",
        "//cv-cuda-pipeline/calculators/core:split_vector_calculator",
        "//cv-cuda-pipeline/calculators/tensor:image_to_tensor_calculator",
        "//cv-cuda-pipeline/calculators/tensor:inference_calculator",
        "//cv-cuda-pipeline/calculators/tensor:tensors_to_floats_calculator",
        "//cv-cuda-pipeline/calculators/tensor:tensors_to_landmarks_calculator",
        "//cv-cuda-pipeline/calculators/tflite:tflite_custom_op_resolver_calculator",
        "//cv-cuda-pipeline/calculators/util:landmark_projection_calculator",
        "//cv-cuda-pipeline/calculators/util:thresholding_calculator",
        "//cv-cuda-pipeline/framework/tool:switch_container",
    ],
)

mediapipe_simple_subgraph(
    name = "face_landmark_gpu",
    graph = "face_landmark_gpu.pbtxt",
    register_as = "FaceLandmarkGpu",
    deps = [
        ":face_landmarks_model_loader",
        ":tensors_to_face_landmarks",
        ":tensors_to_face_landmarks_with_attention",
        "//cv-cuda-pipeline/calculators/core:gate_calculator",
        "//cv-cuda-pipeline/calculators/core:split_vector_calculator",
        "//cv-cuda-pipeline/calculators/tensor:image_to_tensor_calculator",
        "//cv-cuda-pipeline/calculators/tensor:inference_calculator",
        "//cv-cuda-pipeline/calculators/tensor:tensors_to_floats_calculator",
        "//cv-cuda-pipeline/calculators/tensor:tensors_to_landmarks_calculator",
        "//cv-cuda-pipeline/calculators/tflite:tflite_custom_op_resolver_calculator",
        "//cv-cuda-pipeline/calculators/util:landmark_projection_calculator",
        "//cv-cuda-pipeline/calculators/util:thresholding_calculator",
        "//cv-cuda-pipeline/framework/tool:switch_container",
    ],
)

mediapipe_simple_subgraph(
    name = "face_landmark_front_cpu",
    graph = "face_landmark_front_cpu.pbtxt",
    register_as = "FaceLandmarkFrontCpu",
    deps = [
        ":face_detection_front_detection_to_roi",
        ":face_landmark_cpu",
        ":face_landmark_landmarks_to_roi",
        "//cv-cuda-pipeline/calculators/core:begin_loop_calculator",
        "//cv-cuda-pipeline/calculators/core:clip_vector_size_calculator",
        "//cv-cuda-pipeline/calculators/core:constant_side_packet_calculator",
        "//cv-cuda-pipeline/calculators/core:end_loop_calculator",
        "//cv-cuda-pipeline/calculators/core:gate_calculator",
        "//cv-cuda-pipeline/calculators/core:previous_loopback_calculator",
        "//cv-cuda-pipeline/calculators/image:image_properties_calculator",
        "//cv-cuda-pipeline/calculators/util:association_norm_rect_calculator",
        "//cv-cuda-pipeline/calculators/util:collection_has_min_size_calculator",
        "//cv-cuda-pipeline/modules/face_detection:face_detection_short_range_cpu",
    ],
)

mediapipe_simple_subgraph(
    name = "face_landmark_front_gpu",
    graph = "face_landmark_front_gpu.pbtxt",
    register_as = "FaceLandmarkFrontGpu",
    deps = [
        ":face_detection_front_detection_to_roi",
        ":face_landmark_gpu",
        ":face_landmark_landmarks_to_roi",
        "//cv-cuda-pipeline/calculators/core:begin_loop_calculator",
        "//cv-cuda-pipeline/calculators/core:clip_vector_size_calculator",
        "//cv-cuda-pipeline/calculators/core:constant_side_packet_calculator",
        "//cv-cuda-pipeline/calculators/core:end_loop_calculator",
        "//cv-cuda-pipeline/calculators/core:gate_calculator",
        "//cv-cuda-pipeline/calculators/core:previous_loopback_calculator",
        "//cv-cuda-pipeline/calculators/image:image_properties_calculator",
        "//cv-cuda-pipeline/calculators/util:association_norm_rect_calculator",
        "//cv-cuda-pipeline/calculators/util:collection_has_min_size_calculator",
        "//cv-cuda-pipeline/modules/face_detection:face_detection_short_range_gpu",
    ],
)

mediapipe_simple_subgraph(
    name = "face_landmark_front_cpu_image",
    graph = "face_landmark_front_cpu_image.pbtxt",
    register_as = "FaceLandmarkFrontCpuImage",
    deps = [
        ":face_landmark_front_cpu",
        "//cv-cuda-pipeline/calculators/core:flow_limiter_calculator",
        "//cv-cuda-pipeline/calculators/image:image_transformation_calculator",
        "//cv-cuda-pipeline/calculators/util:from_image_calculator",
    ],
)

mediapipe_simple_subgraph(
    name = "face_landmark_front_gpu_image",
    graph = "face_landmark_front_gpu_image.pbtxt",
    register_as = "FaceLandmarkFrontGpuImage",
    deps = [
        ":face_landmark_front_gpu",
        "//cv-cuda-pipeline/calculators/core:flow_limiter_calculator",
        "//cv-cuda-pipeline/calculators/image:image_transformation_calculator",
        "//cv-cuda-pipeline/calculators/util:from_image_calculator",
    ],
)

mediapipe_files(
    srcs = [
        "face_landmark.tflite",
        "face_landmark_with_attention.tflite",
    ],
)

mediapipe_simple_subgraph(
    name = "face_detection_front_detection_to_roi",
    graph = "face_detection_front_detection_to_roi.pbtxt",
    register_as = "FaceDetectionFrontDetectionToRoi",
    deps = [
        "//cv-cuda-pipeline/calculators/util:detections_to_rects_calculator",
        "//cv-cuda-pipeline/calculators/util:rect_transformation_calculator",
    ],
)

mediapipe_simple_subgraph(
    name = "face_landmark_landmarks_to_roi",
    graph = "face_landmark_landmarks_to_roi.pbtxt",
    register_as = "FaceLandmarkLandmarksToRoi",
    deps = [
        "//cv-cuda-pipeline/calculators/util:detections_to_rects_calculator",
        "//cv-cuda-pipeline/calculators/util:landmarks_to_detection_calculator",
        "//cv-cuda-pipeline/calculators/util:rect_transformation_calculator",
    ],
)

mediapipe_simple_subgraph(
    name = "face_landmarks_model_loader",
    graph = "face_landmarks_model_loader.pbtxt",
    register_as = "FaceLandmarksModelLoader",
    deps = [
        "//cv-cuda-pipeline/calculators/core:constant_side_packet_calculator",
        "//cv-cuda-pipeline/calculators/tflite:tflite_model_calculator",
        "//cv-cuda-pipeline/calculators/util:resource_provider_calculator",
        "//cv-cuda-pipeline/framework/tool:switch_container",
    ],
)

mediapipe_simple_subgraph(
    name = "tensors_to_face_landmarks",
    graph = "tensors_to_face_landmarks.pbtxt",
    register_as = "TensorsToFaceLandmarks",
    deps = [
        "//cv-cuda-pipeline/calculators/tensor:tensors_to_landmarks_calculator",
    ],
)

mediapipe_simple_subgraph(
    name = "tensors_to_face_landmarks_with_attention",
    graph = "tensors_to_face_landmarks_with_attention.pbtxt",
    register_as = "TensorsToFaceLandmarksWithAttention",
    deps = [
        "//cv-cuda-pipeline/calculators/core:split_vector_calculator",
        "//cv-cuda-pipeline/calculators/tensor:tensors_to_landmarks_calculator",
        "//cv-cuda-pipeline/calculators/util:landmarks_refinement_calculator",
    ],
)
