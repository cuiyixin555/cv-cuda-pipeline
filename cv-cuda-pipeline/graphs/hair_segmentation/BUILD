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

load("@rules_cc//cc:cc_library.bzl", "cc_library")
load(
    "//cv-cuda-pipeline/framework/tool:mediapipe_graph.bzl",
    "mediapipe_binary_graph",
)

licenses(["notice"])

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "mobile_calculators",
    deps = [
        "//cv-cuda-pipeline/calculators/core:flow_limiter_calculator",
        "//cv-cuda-pipeline/calculators/core:previous_loopback_calculator",
        "//cv-cuda-pipeline/calculators/image:image_transformation_calculator",
        "//cv-cuda-pipeline/calculators/image:recolor_calculator",
        "//cv-cuda-pipeline/calculators/image:set_alpha_calculator",
        "//cv-cuda-pipeline/calculators/tflite:tflite_converter_calculator",
        "//cv-cuda-pipeline/calculators/tflite:tflite_custom_op_resolver_calculator",
        "//cv-cuda-pipeline/calculators/tflite:tflite_inference_calculator",
        "//cv-cuda-pipeline/calculators/tflite:tflite_tensors_to_segmentation_calculator",
        "//cv-cuda-pipeline/gpu:gpu_buffer_to_image_frame_calculator",
        "//cv-cuda-pipeline/gpu:image_frame_to_gpu_buffer_calculator",
    ],
)

cc_library(
    name = "desktop_calculators",
    deps = [
        "//cv-cuda-pipeline/calculators/core:flow_limiter_calculator",
        "//cv-cuda-pipeline/calculators/core:previous_loopback_calculator",
        "//cv-cuda-pipeline/calculators/image:color_convert_calculator",
        "//cv-cuda-pipeline/calculators/image:image_transformation_calculator",
        "//cv-cuda-pipeline/calculators/image:recolor_calculator",
        "//cv-cuda-pipeline/calculators/image:set_alpha_calculator",
        "//cv-cuda-pipeline/calculators/tflite:tflite_converter_calculator",
        "//cv-cuda-pipeline/calculators/tflite:tflite_custom_op_resolver_calculator",
        "//cv-cuda-pipeline/calculators/tflite:tflite_inference_calculator",
        "//cv-cuda-pipeline/calculators/tflite:tflite_tensors_to_segmentation_calculator",
    ],
)

mediapipe_binary_graph(
    name = "mobile_gpu_binary_graph",
    graph = "hair_segmentation_mobile_gpu.pbtxt",
    output_name = "mobile_gpu.binarypb",
    deps = [":mobile_calculators"],
)
