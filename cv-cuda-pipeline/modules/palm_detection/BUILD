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
    "palm_detection_full.tflite",
    "palm_detection_lite.tflite",
])

mediapipe_simple_subgraph(
    name = "palm_detection_model_loader",
    graph = "palm_detection_model_loader.pbtxt",
    register_as = "PalmDetectionModelLoader",
    deps = [
        "//cv-cuda-pipeline/calculators/core:constant_side_packet_calculator",
        "//cv-cuda-pipeline/calculators/tflite:tflite_model_calculator",
        "//cv-cuda-pipeline/calculators/util:resource_provider_calculator",
        "//cv-cuda-pipeline/framework/tool:switch_container",
    ],
)

mediapipe_simple_subgraph(
    name = "palm_detection_cpu",
    graph = "palm_detection_cpu.pbtxt",
    register_as = "PalmDetectionCpu",
    deps = [
        ":palm_detection_model_loader",
        "//cv-cuda-pipeline/calculators/tensor:image_to_tensor_calculator",
        "//cv-cuda-pipeline/calculators/tensor:inference_calculator",
        "//cv-cuda-pipeline/calculators/tensor:tensors_to_detections_calculator",
        "//cv-cuda-pipeline/calculators/tflite:ssd_anchors_calculator",
        "//cv-cuda-pipeline/calculators/tflite:tflite_custom_op_resolver_calculator",
        "//cv-cuda-pipeline/calculators/util:detection_letterbox_removal_calculator",
        "//cv-cuda-pipeline/calculators/util:non_max_suppression_calculator",
    ],
)

mediapipe_simple_subgraph(
    name = "palm_detection_gpu",
    graph = "palm_detection_gpu.pbtxt",
    register_as = "PalmDetectionGpu",
    deps = [
        ":palm_detection_model_loader",
        "//cv-cuda-pipeline/calculators/tensor:image_to_tensor_calculator",
        "//cv-cuda-pipeline/calculators/tensor:inference_calculator",
        "//cv-cuda-pipeline/calculators/tensor:tensors_to_detections_calculator",
        "//cv-cuda-pipeline/calculators/tflite:ssd_anchors_calculator",
        "//cv-cuda-pipeline/calculators/tflite:tflite_custom_op_resolver_calculator",
        "//cv-cuda-pipeline/calculators/util:detection_letterbox_removal_calculator",
        "//cv-cuda-pipeline/calculators/util:non_max_suppression_calculator",
    ],
)
