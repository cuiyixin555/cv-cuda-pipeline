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
    "//cv-cuda-pipeline/framework/tool:mediapipe_graph.bzl",
    "mediapipe_simple_subgraph",
)

licenses(["notice"])

package(default_visibility = ["//visibility:public"])

mediapipe_simple_subgraph(
    name = "hand_renderer_cpu",
    graph = "hand_renderer_cpu.pbtxt",
    register_as = "HandRendererSubgraph",
    deps = [
        "//cv-cuda-pipeline/calculators/core:begin_loop_calculator",
        "//cv-cuda-pipeline/calculators/core:end_loop_calculator",
        "//cv-cuda-pipeline/calculators/core:gate_calculator",
        "//cv-cuda-pipeline/calculators/core:split_vector_calculator",
        "//cv-cuda-pipeline/calculators/util:annotation_overlay_calculator",
        "//cv-cuda-pipeline/calculators/util:collection_has_min_size_calculator",
        "//cv-cuda-pipeline/calculators/util:detections_to_render_data_calculator",
        "//cv-cuda-pipeline/calculators/util:labels_to_render_data_calculator",
        "//cv-cuda-pipeline/calculators/util:landmarks_to_render_data_calculator",
        "//cv-cuda-pipeline/calculators/util:rect_to_render_data_calculator",
    ],
)

mediapipe_simple_subgraph(
    name = "hand_renderer_gpu",
    graph = "hand_renderer_gpu.pbtxt",
    register_as = "HandRendererSubgraph",
    deps = [
        "//cv-cuda-pipeline/calculators/core:begin_loop_calculator",
        "//cv-cuda-pipeline/calculators/core:end_loop_calculator",
        "//cv-cuda-pipeline/calculators/core:gate_calculator",
        "//cv-cuda-pipeline/calculators/core:split_vector_calculator",
        "//cv-cuda-pipeline/calculators/util:annotation_overlay_calculator",
        "//cv-cuda-pipeline/calculators/util:collection_has_min_size_calculator",
        "//cv-cuda-pipeline/calculators/util:detections_to_render_data_calculator",
        "//cv-cuda-pipeline/calculators/util:labels_to_render_data_calculator",
        "//cv-cuda-pipeline/calculators/util:landmarks_to_render_data_calculator",
        "//cv-cuda-pipeline/calculators/util:rect_to_render_data_calculator",
    ],
)
