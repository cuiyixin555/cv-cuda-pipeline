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

licenses(["notice"])

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "clipped_images_from_file_at_24fps_calculators",
    deps = [
        "//cv-cuda-pipeline/calculators/core:packet_resampler_calculator",
        "//cv-cuda-pipeline/calculators/image:opencv_image_encoder_calculator",
        "//cv-cuda-pipeline/calculators/image:scale_image_calculator",
        "//cv-cuda-pipeline/calculators/tensorflow:pack_media_sequence_calculator",
        "//cv-cuda-pipeline/calculators/tensorflow:string_to_sequence_example_calculator",
        "//cv-cuda-pipeline/calculators/tensorflow:unpack_media_sequence_calculator",
        "//cv-cuda-pipeline/calculators/video:opencv_video_decoder_calculator",
    ],
)

cc_library(
    name = "tvl1_flow_and_rgb_from_file_calculators",
    deps = [
        "//cv-cuda-pipeline/calculators/core:packet_inner_join_calculator",
        "//cv-cuda-pipeline/calculators/core:packet_resampler_calculator",
        "//cv-cuda-pipeline/calculators/core:sequence_shift_calculator",
        "//cv-cuda-pipeline/calculators/image:opencv_image_encoder_calculator",
        "//cv-cuda-pipeline/calculators/image:scale_image_calculator",
        "//cv-cuda-pipeline/calculators/tensorflow:pack_media_sequence_calculator",
        "//cv-cuda-pipeline/calculators/tensorflow:string_to_sequence_example_calculator",
        "//cv-cuda-pipeline/calculators/tensorflow:unpack_media_sequence_calculator",
        "//cv-cuda-pipeline/calculators/video:flow_to_image_calculator",
        "//cv-cuda-pipeline/calculators/video:opencv_video_decoder_calculator",
        "//cv-cuda-pipeline/calculators/video:tvl1_optical_flow_calculator",
    ],
)
