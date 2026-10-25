load("@rules_cc//cc:cc_binary.bzl", "cc_binary")
load("//cv-cuda-pipeline/framework/port:build_config.bzl", "mediapipe_proto_library")

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

package(default_visibility = [
    "//cv-cuda-pipeline/examples:__subpackages__",
    "//photos/editing/mobile/cv-cuda-pipeline/calculators:__subpackages__",
    "//photos/editing/mobile/cv-cuda-pipeline/proto:__subpackages__",
])

mediapipe_proto_library(
    name = "autoflip_messages_proto",
    srcs = ["autoflip_messages.proto"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_options_proto",
    ],
)

cc_binary(
    name = "run_autoflip",
    data = [
        "//cv-cuda-pipeline/modules/face_detection:face_detection_full_range_sparse.tflite",
    ],
    deps = [
        "//cv-cuda-pipeline/calculators/core:packet_thinner_calculator",
        "//cv-cuda-pipeline/calculators/image:scale_image_calculator",
        "//cv-cuda-pipeline/calculators/video:opencv_video_decoder_calculator",
        "//cv-cuda-pipeline/calculators/video:opencv_video_encoder_calculator",
        "//cv-cuda-pipeline/calculators/video:video_pre_stream_calculator",
        "//cv-cuda-pipeline/examples/desktop:simple_run_graph_main",
        "//cv-cuda-pipeline/examples/desktop/autoflip/calculators:border_detection_calculator",
        "//cv-cuda-pipeline/examples/desktop/autoflip/calculators:face_to_region_calculator",
        "//cv-cuda-pipeline/examples/desktop/autoflip/calculators:localization_to_region_calculator",
        "//cv-cuda-pipeline/examples/desktop/autoflip/calculators:scene_cropping_calculator",
        "//cv-cuda-pipeline/examples/desktop/autoflip/calculators:shot_boundary_calculator",
        "//cv-cuda-pipeline/examples/desktop/autoflip/calculators:signal_fusing_calculator",
        "//cv-cuda-pipeline/examples/desktop/autoflip/calculators:video_filtering_calculator",
        "//cv-cuda-pipeline/examples/desktop/autoflip/subgraph:autoflip_face_detection_subgraph",
        "//cv-cuda-pipeline/examples/desktop/autoflip/subgraph:autoflip_object_detection_subgraph",
    ],
)
