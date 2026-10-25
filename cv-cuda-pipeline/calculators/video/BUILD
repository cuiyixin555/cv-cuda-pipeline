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
#

load("@rules_cc//cc:cc_library.bzl", "cc_library")
load("@rules_cc//cc:cc_test.bzl", "cc_test")
load("//cv-cuda-pipeline/framework/port:build_config.bzl", "mediapipe_proto_library")
load(
    "//cv-cuda-pipeline/framework/tool:mediapipe_graph.bzl",
    "mediapipe_binary_graph",
)

licenses(["notice"])

package(default_visibility = ["//visibility:public"])

mediapipe_proto_library(
    name = "flow_to_image_calculator_proto",
    srcs = ["flow_to_image_calculator.proto"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_options_proto",
        "//cv-cuda-pipeline/framework:calculator_proto",
    ],
)

mediapipe_proto_library(
    name = "opencv_video_encoder_calculator_proto",
    srcs = ["opencv_video_encoder_calculator.proto"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_options_proto",
        "//cv-cuda-pipeline/framework:calculator_proto",
    ],
)

mediapipe_proto_library(
    name = "motion_analysis_calculator_proto",
    srcs = ["motion_analysis_calculator.proto"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_options_proto",
        "//cv-cuda-pipeline/framework:calculator_proto",
        "//cv-cuda-pipeline/util/tracking:motion_analysis_proto",
    ],
)

mediapipe_proto_library(
    name = "flow_packager_calculator_proto",
    srcs = ["flow_packager_calculator.proto"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_options_proto",
        "//cv-cuda-pipeline/framework:calculator_proto",
        "//cv-cuda-pipeline/util/tracking:flow_packager_proto",
    ],
)

mediapipe_proto_library(
    name = "box_tracker_calculator_proto",
    srcs = ["box_tracker_calculator.proto"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_options_proto",
        "//cv-cuda-pipeline/framework:calculator_proto",
        "//cv-cuda-pipeline/util/tracking:box_tracker_proto",
    ],
)

mediapipe_proto_library(
    name = "tracked_detection_manager_calculator_proto",
    srcs = ["tracked_detection_manager_calculator.proto"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_options_proto",
        "//cv-cuda-pipeline/framework:calculator_proto",
        "//cv-cuda-pipeline/util/tracking:tracked_detection_manager_config_proto",
    ],
)

mediapipe_proto_library(
    name = "box_detector_calculator_proto",
    srcs = ["box_detector_calculator.proto"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_options_proto",
        "//cv-cuda-pipeline/framework:calculator_proto",
        "//cv-cuda-pipeline/util/tracking:box_detector_proto",
    ],
)

mediapipe_proto_library(
    name = "video_pre_stream_calculator_proto",
    srcs = ["video_pre_stream_calculator.proto"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_options_proto",
        "//cv-cuda-pipeline/framework:calculator_proto",
    ],
)

cc_library(
    name = "flow_to_image_calculator",
    srcs = ["flow_to_image_calculator.cc"],
    deps = [
        ":flow_to_image_calculator_cc_proto",
        "//cv-cuda-pipeline/calculators/video/tool:flow_quantizer_model",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/formats:image_format_cc_proto",
        "//cv-cuda-pipeline/framework/formats:image_frame",
        "//cv-cuda-pipeline/framework/formats:image_frame_opencv",
        "//cv-cuda-pipeline/framework/formats/motion:optical_flow_field",
        "//cv-cuda-pipeline/framework/port:opencv_core",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
    ],
    alwayslink = 1,
)

cc_library(
    name = "opencv_video_decoder_calculator",
    srcs = ["opencv_video_decoder_calculator.cc"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/formats:image_format_cc_proto",
        "//cv-cuda-pipeline/framework/formats:image_frame",
        "//cv-cuda-pipeline/framework/formats:image_frame_opencv",
        "//cv-cuda-pipeline/framework/formats:video_stream_header",
        "//cv-cuda-pipeline/framework/port:opencv_imgproc",
        "//cv-cuda-pipeline/framework/port:opencv_video",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/framework/tool:status_util",
        "@com_google_absl//absl/log:absl_log",
    ],
    alwayslink = 1,
)

cc_library(
    name = "opencv_video_encoder_calculator",
    srcs = ["opencv_video_encoder_calculator.cc"],
    deps = [
        ":opencv_video_encoder_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/formats:image_frame",
        "//cv-cuda-pipeline/framework/formats:image_frame_opencv",
        "//cv-cuda-pipeline/framework/formats:video_stream_header",
        "//cv-cuda-pipeline/framework/port:file_helpers",
        "//cv-cuda-pipeline/framework/port:opencv_imgproc",
        "//cv-cuda-pipeline/framework/port:opencv_video",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:source_location",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/framework/tool:status_util",
        "@com_google_absl//absl/log:absl_log",
        "@com_google_absl//absl/strings",
    ],
    alwayslink = 1,
)

cc_library(
    name = "tvl1_optical_flow_calculator",
    srcs = ["tvl1_optical_flow_calculator.cc"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/formats:image_frame",
        "//cv-cuda-pipeline/framework/formats:image_frame_opencv",
        "//cv-cuda-pipeline/framework/formats/motion:optical_flow_field",
        "//cv-cuda-pipeline/framework/port:opencv_video",
        "@com_google_absl//absl/base:core_headers",
        "@com_google_absl//absl/log:absl_check",
        "@com_google_absl//absl/synchronization",
    ],
    alwayslink = 1,
)

cc_library(
    name = "motion_analysis_calculator",
    srcs = ["motion_analysis_calculator.cc"],
    deps = [
        ":motion_analysis_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/formats:image_frame",
        "//cv-cuda-pipeline/framework/formats:image_frame_opencv",
        "//cv-cuda-pipeline/framework/formats:video_stream_header",
        "//cv-cuda-pipeline/framework/port:integral_types",
        "//cv-cuda-pipeline/framework/port:logging",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/util/tracking:camera_motion",
        "//cv-cuda-pipeline/util/tracking:camera_motion_cc_proto",
        "//cv-cuda-pipeline/util/tracking:frame_selection_cc_proto",
        "//cv-cuda-pipeline/util/tracking:motion_analysis",
        "//cv-cuda-pipeline/util/tracking:motion_estimation",
        "//cv-cuda-pipeline/util/tracking:motion_models",
        "//cv-cuda-pipeline/util/tracking:region_flow_cc_proto",
        "@com_google_absl//absl/log:absl_check",
        "@com_google_absl//absl/log:absl_log",
        "@com_google_absl//absl/strings",
    ],
    alwayslink = 1,
)

cc_library(
    name = "flow_packager_calculator",
    srcs = ["flow_packager_calculator.cc"],
    deps = [
        ":flow_packager_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/port:integral_types",
        "//cv-cuda-pipeline/util/tracking:camera_motion_cc_proto",
        "//cv-cuda-pipeline/util/tracking:flow_packager",
        "//cv-cuda-pipeline/util/tracking:region_flow_cc_proto",
        "@com_google_absl//absl/log:absl_check",
        "@com_google_absl//absl/log:absl_log",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
    ],
    alwayslink = 1,
)

cc_library(
    name = "box_tracker_calculator",
    srcs = ["box_tracker_calculator.cc"],
    deps = [
        ":box_tracker_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/formats:image_frame",
        "//cv-cuda-pipeline/framework/formats:image_frame_opencv",
        "//cv-cuda-pipeline/framework/formats:video_stream_header",  # fixdeps: keep -- required for exobazel build.
        "//cv-cuda-pipeline/framework/port:integral_types",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/framework/tool:options_util",
        "//cv-cuda-pipeline/util/tracking",
        "//cv-cuda-pipeline/util/tracking:box_tracker",
        "//cv-cuda-pipeline/util/tracking:tracking_visualization_utilities",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/container:node_hash_map",
        "@com_google_absl//absl/container:node_hash_set",
        "@com_google_absl//absl/log:absl_check",
        "@com_google_absl//absl/log:absl_log",
        "@com_google_absl//absl/strings",
    ],
    alwayslink = 1,
)

cc_library(
    name = "box_detector_calculator",
    srcs = ["box_detector_calculator.cc"],
    deps = [
        ":box_detector_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/formats:image_frame",
        "//cv-cuda-pipeline/framework/formats:image_frame_opencv",
        "//cv-cuda-pipeline/framework/formats:video_stream_header",  # fixdeps: keep -- required for exobazel build.
        "//cv-cuda-pipeline/framework/port:integral_types",
        "//cv-cuda-pipeline/framework/port:opencv_core",
        "//cv-cuda-pipeline/framework/port:opencv_features2d",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/util:resource_util",
        "//cv-cuda-pipeline/util/tracking",
        "//cv-cuda-pipeline/util/tracking:box_detector",
        "//cv-cuda-pipeline/util/tracking:box_tracker",
        "//cv-cuda-pipeline/util/tracking:box_tracker_cc_proto",
        "//cv-cuda-pipeline/util/tracking:flow_packager_cc_proto",
        "//cv-cuda-pipeline/util/tracking:tracking_visualization_utilities",
        "@com_google_absl//absl/log:absl_check",
        "@com_google_absl//absl/log:absl_log",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/strings",
    ] + select({
        "//cv-cuda-pipeline:emscripten": [
        ],
        "//conditions:default": [
            "//cv-cuda-pipeline/framework/port:file_helpers",
        ],
    }),
    alwayslink = 1,
)

cc_library(
    name = "tracked_detection_manager_calculator",
    srcs = ["tracked_detection_manager_calculator.cc"],
    deps = [
        ":tracked_detection_manager_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/formats:detection_cc_proto",
        "//cv-cuda-pipeline/framework/formats:location_data_cc_proto",
        "//cv-cuda-pipeline/framework/formats:rect_cc_proto",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/util/tracking",
        "//cv-cuda-pipeline/util/tracking:box_tracker",
        "//cv-cuda-pipeline/util/tracking:tracked_detection",
        "//cv-cuda-pipeline/util/tracking:tracked_detection_manager",
        "//cv-cuda-pipeline/util/tracking:tracking_visualization_utilities",
        "@com_google_absl//absl/container:node_hash_map",
    ],
    alwayslink = 1,
)

cc_library(
    name = "video_pre_stream_calculator",
    srcs = ["video_pre_stream_calculator.cc"],
    deps = [
        ":video_pre_stream_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/formats:image_frame",
        "//cv-cuda-pipeline/framework/formats:video_stream_header",
    ],
    alwayslink = 1,
)

filegroup(
    name = "test_videos",
    srcs = [
        "testdata/format_FLV_H264_AAC.video",
        "testdata/format_MKV_VP8_VORBIS.video",
        "testdata/format_MP4_AVC720P_AAC.video",
    ],
)

cc_test(
    name = "opencv_video_decoder_calculator_test",
    srcs = ["opencv_video_decoder_calculator_test.cc"],
    data = [":test_videos"],
    deps = [
        ":opencv_video_decoder_calculator",
        "//cv-cuda-pipeline/framework:calculator_runner",
        "//cv-cuda-pipeline/framework/deps:file_path",
        "//cv-cuda-pipeline/framework/formats:image_frame",
        "//cv-cuda-pipeline/framework/formats:image_frame_opencv",
        "//cv-cuda-pipeline/framework/formats:video_stream_header",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:integral_types",
        "//cv-cuda-pipeline/framework/port:logging",
        "//cv-cuda-pipeline/framework/port:opencv_core",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
        "//cv-cuda-pipeline/framework/tool:test_util",
    ],
)

cc_test(
    name = "opencv_video_encoder_calculator_test",
    srcs = ["opencv_video_encoder_calculator_test.cc"],
    data = [":test_videos"],
    deps = [
        ":opencv_video_decoder_calculator",
        ":opencv_video_encoder_calculator",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:packet",
        "//cv-cuda-pipeline/framework/deps:file_path",
        "//cv-cuda-pipeline/framework/formats:deleting_file",
        "//cv-cuda-pipeline/framework/formats:image_frame",
        "//cv-cuda-pipeline/framework/formats:image_frame_opencv",
        "//cv-cuda-pipeline/framework/formats:video_stream_header",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:integral_types",
        "//cv-cuda-pipeline/framework/port:logging",
        "//cv-cuda-pipeline/framework/port:opencv_imgproc",
        "//cv-cuda-pipeline/framework/port:opencv_video",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
        "//cv-cuda-pipeline/framework/tool:test_util",
    ],
)

cc_test(
    name = "tvl1_optical_flow_calculator_test",
    srcs = ["tvl1_optical_flow_calculator_test.cc"],
    linkstatic = 1,
    deps = [
        ":tvl1_optical_flow_calculator",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:calculator_runner",
        "//cv-cuda-pipeline/framework/deps:file_path",
        "//cv-cuda-pipeline/framework/formats:image_frame_opencv",
        "//cv-cuda-pipeline/framework/formats/motion:optical_flow_field",
        "//cv-cuda-pipeline/framework/port:file_helpers",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:opencv_imgcodecs",
        "//cv-cuda-pipeline/framework/port:opencv_imgproc",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
    ],
)

mediapipe_binary_graph(
    name = "parallel_tracker_binarypb",
    graph = "testdata/parallel_tracker_graph.pbtxt",
    output_name = "testdata/parallel_tracker.binarypb",
    deps = [
        ":box_tracker_calculator",
        ":flow_packager_calculator",
        ":motion_analysis_calculator",
        "//cv-cuda-pipeline/framework/stream_handler:fixed_size_input_stream_handler",
        "//cv-cuda-pipeline/framework/stream_handler:sync_set_input_stream_handler",
    ],
)

mediapipe_binary_graph(
    name = "tracker_binarypb",
    graph = "testdata/tracker_graph.pbtxt",
    output_name = "testdata/tracker.binarypb",
    deps = [
        ":box_tracker_calculator",
        ":flow_packager_calculator",
        ":motion_analysis_calculator",
        "//cv-cuda-pipeline/framework/stream_handler:fixed_size_input_stream_handler",
        "//cv-cuda-pipeline/framework/stream_handler:sync_set_input_stream_handler",
    ],
)

cc_test(
    name = "tracking_graph_test",
    size = "small",
    srcs = ["tracking_graph_test.cc"],
    copts = ["-DPARALLEL_INVOKER_ACTIVE"] + select({
        "//cv-cuda-pipeline:emscripten": ["-UPARALLEL_INVOKER_ACTIVE"],
        "//conditions:default": [],
    }),
    data = [
        ":testdata/lenna.png",
        ":testdata/parallel_tracker.binarypb",
        ":testdata/tracker.binarypb",
    ],
    deps = [
        ":box_tracker_calculator",
        ":box_tracker_calculator_cc_proto",
        ":flow_packager_calculator",
        ":motion_analysis_calculator",
        "//cv-cuda-pipeline/framework:calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:packet",
        "//cv-cuda-pipeline/framework/deps:file_path",
        "//cv-cuda-pipeline/framework/formats:image_frame",
        "//cv-cuda-pipeline/framework/port:advanced_proto",
        "//cv-cuda-pipeline/framework/port:core_proto",
        "//cv-cuda-pipeline/framework/port:file_helpers",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:opencv_imgcodecs",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/framework/stream_handler:fixed_size_input_stream_handler",
        "//cv-cuda-pipeline/framework/stream_handler:sync_set_input_stream_handler",
        "//cv-cuda-pipeline/framework/tool:test_util",
        "//cv-cuda-pipeline/util/tracking:box_tracker_cc_proto",
        "//cv-cuda-pipeline/util/tracking:tracking_cc_proto",
        "@com_google_absl//absl/log:absl_check",
        "@com_google_absl//absl/log:absl_log",
    ],
)

cc_test(
    name = "video_pre_stream_calculator_test",
    srcs = ["video_pre_stream_calculator_test.cc"],
    deps = [
        ":video_pre_stream_calculator",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/formats:image_frame",
        "//cv-cuda-pipeline/framework/formats:video_stream_header",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
        "//cv-cuda-pipeline/framework/port:status",
    ],
)
