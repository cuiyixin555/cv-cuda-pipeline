load("//cv-cuda-pipeline/framework/tool:mediapipe_graph.bzl", "mediapipe_simple_subgraph")

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

licenses(["notice"])

FACE_DETECTION_DEPS = [
    "//cv-cuda-pipeline/calculators/image:image_transformation_calculator",
    "//cv-cuda-pipeline/calculators/tflite:ssd_anchors_calculator",
    "//cv-cuda-pipeline/calculators/tflite:tflite_converter_calculator",
    "//cv-cuda-pipeline/calculators/tflite:tflite_inference_calculator",
    "//cv-cuda-pipeline/calculators/tflite:tflite_tensors_to_detections_calculator",
    "//cv-cuda-pipeline/calculators/util:detection_label_id_to_text_calculator",
    "//cv-cuda-pipeline/calculators/util:detection_letterbox_removal_calculator",
    "//cv-cuda-pipeline/calculators/util:non_max_suppression_calculator",
]

mediapipe_simple_subgraph(
    name = "autoflip_face_detection_subgraph",
    graph = "face_detection_subgraph.pbtxt",
    register_as = "AutoFlipFaceDetectionSubgraph",
    visibility = ["//visibility:public"],
    deps = FACE_DETECTION_DEPS,
)

mediapipe_simple_subgraph(
    name = "autoflip_front_face_detection_subgraph",
    graph = "front_face_detection_subgraph.pbtxt",
    register_as = "AutoFlipFrontFaceDetectionSubgraph",
    visibility = ["//visibility:public"],
    deps = FACE_DETECTION_DEPS,
)

mediapipe_simple_subgraph(
    name = "autoflip_object_detection_subgraph",
    graph = "autoflip_object_detection_subgraph.pbtxt",
    register_as = "AutoFlipObjectDetectionSubgraph",
    visibility = ["//visibility:public"],
    deps = [
        "//cv-cuda-pipeline/graphs/object_detection:desktop_tflite_calculators",
    ],
)
