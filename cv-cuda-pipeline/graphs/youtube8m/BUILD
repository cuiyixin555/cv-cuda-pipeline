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
    name = "yt8m_feature_extraction_calculators",
    visibility = ["//visibility:public"],
    deps = [
        "//cv-cuda-pipeline/calculators/audio:audio_decoder_calculator",
        "//cv-cuda-pipeline/calculators/audio:basic_time_series_calculators",
        "//cv-cuda-pipeline/calculators/audio:mfcc_mel_calculators",
        "//cv-cuda-pipeline/calculators/audio:rational_factor_resample_calculator",
        "//cv-cuda-pipeline/calculators/audio:spectrogram_calculator",
        "//cv-cuda-pipeline/calculators/audio:stabilized_log_calculator",
        "//cv-cuda-pipeline/calculators/audio:time_series_framer_calculator",
        "//cv-cuda-pipeline/calculators/core:add_header_calculator",
        "//cv-cuda-pipeline/calculators/core:matrix_multiply_calculator",
        "//cv-cuda-pipeline/calculators/core:matrix_subtract_calculator",
        "//cv-cuda-pipeline/calculators/core:matrix_to_vector_calculator",
        "//cv-cuda-pipeline/calculators/core:packet_cloner_calculator",
        "//cv-cuda-pipeline/calculators/core:packet_resampler_calculator",
        "//cv-cuda-pipeline/calculators/tensorflow:image_frame_to_tensor_calculator",
        "//cv-cuda-pipeline/calculators/tensorflow:matrix_to_tensor_calculator",
        "//cv-cuda-pipeline/calculators/tensorflow:pack_media_sequence_calculator",
        "//cv-cuda-pipeline/calculators/tensorflow:string_to_sequence_example_calculator",
        "//cv-cuda-pipeline/calculators/tensorflow:tensor_squeeze_dimensions_calculator",
        "//cv-cuda-pipeline/calculators/tensorflow:tensor_to_matrix_calculator",
        "//cv-cuda-pipeline/calculators/tensorflow:tensorflow_inference_calculator",
        "//cv-cuda-pipeline/calculators/tensorflow:tensorflow_session_from_frozen_graph_calculator",
        "//cv-cuda-pipeline/calculators/tensorflow:unpack_media_sequence_calculator",
        "//cv-cuda-pipeline/calculators/video:opencv_video_decoder_calculator",
    ],
)

cc_library(
    name = "yt8m_inference_calculators_deps",
    visibility = ["//visibility:public"],
    deps = [
        "//cv-cuda-pipeline/calculators/core:concatenate_vector_calculator",
        "//cv-cuda-pipeline/calculators/core:dequantize_byte_array_calculator",
        "//cv-cuda-pipeline/calculators/core:packet_cloner_calculator",
        "//cv-cuda-pipeline/calculators/core:side_packet_to_stream_calculator",
        "//cv-cuda-pipeline/calculators/core:string_to_int_calculator",
        "//cv-cuda-pipeline/calculators/tensorflow:lapped_tensor_buffer_calculator",
        "//cv-cuda-pipeline/calculators/tensorflow:string_to_sequence_example_calculator",
        "//cv-cuda-pipeline/calculators/tensorflow:tensor_to_vector_float_calculator",
        "//cv-cuda-pipeline/calculators/tensorflow:tensorflow_inference_calculator",
        "//cv-cuda-pipeline/calculators/tensorflow:tensorflow_session_from_saved_model_calculator",
        "//cv-cuda-pipeline/calculators/tensorflow:tfrecord_reader_calculator",
        "//cv-cuda-pipeline/calculators/tensorflow:unpack_media_sequence_calculator",
        "//cv-cuda-pipeline/calculators/tensorflow:unpack_yt8m_sequence_example_calculator",
        "//cv-cuda-pipeline/calculators/tensorflow:vector_float_to_tensor_calculator",
        "//cv-cuda-pipeline/calculators/tensorflow:vector_int_to_tensor_calculator",
        "//cv-cuda-pipeline/calculators/util:annotation_overlay_calculator",
        "//cv-cuda-pipeline/calculators/util:labels_to_render_data_calculator",
        "//cv-cuda-pipeline/calculators/util:local_file_contents_calculator",
        "//cv-cuda-pipeline/calculators/util:top_k_scores_calculator",
        "//cv-cuda-pipeline/calculators/video:opencv_video_decoder_calculator",
        "//cv-cuda-pipeline/calculators/video:opencv_video_encoder_calculator",
    ],
)
