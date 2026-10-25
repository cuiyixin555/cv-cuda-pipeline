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
    name = "template_matching_deps",
    deps = [
        "//cv-cuda-pipeline/calculators/image:feature_detector_calculator",
        "//cv-cuda-pipeline/calculators/image:image_properties_calculator",
        "//cv-cuda-pipeline/calculators/image:image_transformation_calculator",
        "//cv-cuda-pipeline/calculators/tflite:tflite_converter_calculator",
        "//cv-cuda-pipeline/calculators/tflite:tflite_inference_calculator",
        "//cv-cuda-pipeline/calculators/tflite:tflite_tensors_to_floats_calculator",
        "//cv-cuda-pipeline/calculators/util:annotation_overlay_calculator",
        "//cv-cuda-pipeline/calculators/util:landmarks_to_render_data_calculator",
        "//cv-cuda-pipeline/calculators/util:timed_box_list_id_to_label_calculator",
        "//cv-cuda-pipeline/calculators/util:timed_box_list_to_render_data_calculator",
        "//cv-cuda-pipeline/calculators/video:box_detector_calculator",
    ],
)

cc_library(
    name = "desktop_calculators",
    deps = [
        ":template_matching_deps",
        "//cv-cuda-pipeline/calculators/image:opencv_encoded_image_to_image_frame_calculator",
        "//cv-cuda-pipeline/calculators/util:local_file_pattern_contents_calculator",
        "//cv-cuda-pipeline/calculators/video:opencv_video_decoder_calculator",
        "//cv-cuda-pipeline/calculators/video:opencv_video_encoder_calculator",
    ],
)

cc_library(
    name = "mobile_calculators",
    deps = [
        ":template_matching_deps",
        "//cv-cuda-pipeline/calculators/core:flow_limiter_calculator",
        "//cv-cuda-pipeline/calculators/image:image_transformation_calculator",
        "//cv-cuda-pipeline/gpu:gpu_buffer_to_image_frame_calculator",
    ],
)

mediapipe_binary_graph(
    name = "mobile_cpu_binary_graph",
    graph = "template_matching_mobile_cpu.pbtxt",
    output_name = "mobile_cpu.binarypb",
    deps = [":mobile_calculators"],
)
