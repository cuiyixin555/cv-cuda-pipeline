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
load("@rules_cc//cc:cc_test.bzl", "cc_test")
load("//cv-cuda-pipeline/framework/port:build_config.bzl", "mediapipe_proto_library")

licenses(["notice"])

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "alignment_points_to_rects_calculator",
    srcs = ["alignment_points_to_rects_calculator.cc"],
    deps = [
        ":detections_to_rects_calculator",
        ":detections_to_rects_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:calculator_options_cc_proto",
        "//cv-cuda-pipeline/framework/formats:detection_cc_proto",
        "//cv-cuda-pipeline/framework/formats:location_data_cc_proto",
        "//cv-cuda-pipeline/framework/formats:rect_cc_proto",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
    ],
    alwayslink = 1,
)

mediapipe_proto_library(
    name = "align_hand_to_pose_in_world_calculator_proto",
    srcs = ["align_hand_to_pose_in_world_calculator.proto"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_options_proto",
        "//cv-cuda-pipeline/framework:calculator_proto",
    ],
)

cc_library(
    name = "align_hand_to_pose_in_world_calculator",
    srcs = ["align_hand_to_pose_in_world_calculator.cc"],
    hdrs = ["align_hand_to_pose_in_world_calculator.h"],
    deps = [
        ":align_hand_to_pose_in_world_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/api2:node",
        "//cv-cuda-pipeline/framework/api2:port",
        "//cv-cuda-pipeline/framework/formats:landmark_cc_proto",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "@com_google_absl//absl/status",
    ],
    alwayslink = 1,
)

mediapipe_proto_library(
    name = "annotation_overlay_calculator_proto",
    srcs = ["annotation_overlay_calculator.proto"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_options_proto",
        "//cv-cuda-pipeline/framework:calculator_proto",
        "//cv-cuda-pipeline/util:color_proto",
    ],
)

mediapipe_proto_library(
    name = "detection_label_id_to_text_calculator_proto",
    srcs = ["detection_label_id_to_text_calculator.proto"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_options_proto",
        "//cv-cuda-pipeline/framework:calculator_proto",
        "//cv-cuda-pipeline/util:label_map_proto",
    ],
)

mediapipe_proto_library(
    name = "filter_detections_calculator_proto",
    srcs = ["filter_detections_calculator.proto"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_options_proto",
        "//cv-cuda-pipeline/framework:calculator_proto",
    ],
)

mediapipe_proto_library(
    name = "timed_box_list_id_to_label_calculator_proto",
    srcs = ["timed_box_list_id_to_label_calculator.proto"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_options_proto",
        "//cv-cuda-pipeline/framework:calculator_proto",
    ],
)

mediapipe_proto_library(
    name = "latency_proto",
    srcs = ["latency.proto"],
)

mediapipe_proto_library(
    name = "non_max_suppression_calculator_proto",
    srcs = ["non_max_suppression_calculator.proto"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_options_proto",
        "//cv-cuda-pipeline/framework:calculator_proto",
    ],
)

mediapipe_proto_library(
    name = "packet_frequency_proto",
    srcs = ["packet_frequency.proto"],
)

mediapipe_proto_library(
    name = "packet_frequency_calculator_proto",
    srcs = ["packet_frequency_calculator.proto"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_options_proto",
        "//cv-cuda-pipeline/framework:calculator_proto",
    ],
)

mediapipe_proto_library(
    name = "packet_latency_calculator_proto",
    srcs = ["packet_latency_calculator.proto"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_options_proto",
        "//cv-cuda-pipeline/framework:calculator_proto",
    ],
)

mediapipe_proto_library(
    name = "collection_has_min_size_calculator_proto",
    srcs = ["collection_has_min_size_calculator.proto"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_options_proto",
        "//cv-cuda-pipeline/framework:calculator_proto",
    ],
)

mediapipe_proto_library(
    name = "association_calculator_proto",
    srcs = ["association_calculator.proto"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_options_proto",
        "//cv-cuda-pipeline/framework:calculator_proto",
    ],
)

cc_library(
    name = "packet_frequency_calculator",
    srcs = ["packet_frequency_calculator.cc"],
    deps = [
        "//cv-cuda-pipeline/calculators/util:packet_frequency_calculator_cc_proto",
        "//cv-cuda-pipeline/calculators/util:packet_frequency_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:calculator_options_cc_proto",
        "//cv-cuda-pipeline/framework:timestamp",
        "//cv-cuda-pipeline/framework/port:status",
        "@com_google_absl//absl/time",
    ],
    alwayslink = 1,
)

cc_test(
    name = "packet_frequency_calculator_test",
    size = "small",
    srcs = ["packet_frequency_calculator_test.cc"],
    deps = [
        ":packet_frequency_calculator",
        "//cv-cuda-pipeline/calculators/util:packet_frequency_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_runner",
        "//cv-cuda-pipeline/framework:timestamp",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
        "//cv-cuda-pipeline/framework/stream_handler:immediate_input_stream_handler",
    ],
)

cc_test(
    name = "filter_detections_calculator_test",
    size = "small",
    srcs = ["filter_detections_calculator_test.cc"],
    deps = [
        ":filter_detections_calculator",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:calculator_runner",
        "//cv-cuda-pipeline/framework/formats:detection_cc_proto",
        "//cv-cuda-pipeline/framework/formats:location_data_cc_proto",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
    ],
)

cc_library(
    name = "packet_latency_calculator",
    srcs = ["packet_latency_calculator.cc"],
    deps = [
        "//cv-cuda-pipeline/calculators/util:latency_cc_proto",
        "//cv-cuda-pipeline/calculators/util:packet_latency_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:calculator_options_cc_proto",
        "//cv-cuda-pipeline/framework:timestamp",
        "//cv-cuda-pipeline/framework/deps:clock",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "@com_google_absl//absl/log:absl_log",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/time",
    ],
    alwayslink = 1,
)

cc_test(
    name = "packet_latency_calculator_test",
    size = "small",
    srcs = ["packet_latency_calculator_test.cc"],
    deps = [
        ":packet_latency_calculator",
        "//cv-cuda-pipeline/calculators/util:latency_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_runner",
        "//cv-cuda-pipeline/framework:timestamp",
        "//cv-cuda-pipeline/framework/deps:clock",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
        "//cv-cuda-pipeline/framework/stream_handler:immediate_input_stream_handler",
        "//cv-cuda-pipeline/framework/tool:simulation_clock",
        "//cv-cuda-pipeline/framework/tool:simulation_clock_executor",
        "//cv-cuda-pipeline/framework/tool:sink",
        "@com_google_absl//absl/time",
    ],
)

cc_library(
    name = "clock_timestamp_calculator",
    srcs = ["clock_timestamp_calculator.cc"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:timestamp",
        "//cv-cuda-pipeline/framework/deps:clock",
        "//cv-cuda-pipeline/framework/port:logging",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "@com_google_absl//absl/time",
    ],
    alwayslink = 1,
)

cc_library(
    name = "clock_latency_calculator",
    srcs = ["clock_latency_calculator.cc"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:timestamp",
        "//cv-cuda-pipeline/framework/port:logging",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "@com_google_absl//absl/time",
    ],
    alwayslink = 1,
)

cc_library(
    name = "annotation_overlay_calculator",
    srcs = ["annotation_overlay_calculator.cc"],
    deps = [
        ":annotation_overlay_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:calculator_options_cc_proto",
        "//cv-cuda-pipeline/framework/formats:image",
        "//cv-cuda-pipeline/framework/formats:image_format_cc_proto",
        "//cv-cuda-pipeline/framework/formats:image_frame",
        "//cv-cuda-pipeline/framework/formats:image_frame_opencv",
        "//cv-cuda-pipeline/framework/formats:image_opencv",
        "//cv-cuda-pipeline/framework/formats:video_stream_header",
        "//cv-cuda-pipeline/framework/port:opencv_core",
        "//cv-cuda-pipeline/framework/port:opencv_imgproc",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/framework/port:vector",
        "//cv-cuda-pipeline/util:annotation_renderer",
        "//cv-cuda-pipeline/util:color_cc_proto",
        "//cv-cuda-pipeline/util:render_data_cc_proto",
        "@com_google_absl//absl/log:absl_log",
        "@com_google_absl//absl/strings",
    ] + select({
        "//cv-cuda-pipeline/gpu:disable_gpu": [],
        "//conditions:default": [
            "//cv-cuda-pipeline/gpu:gl_calculator_helper",
            "//cv-cuda-pipeline/gpu:gl_simple_shaders",
            "//cv-cuda-pipeline/gpu:gpu_buffer",
            "//cv-cuda-pipeline/gpu:gpu_buffer_format",
            "//cv-cuda-pipeline/gpu:shader_util",
        ],
    }),
    alwayslink = 1,
)

cc_library(
    name = "detection_label_id_to_text_calculator",
    srcs = ["detection_label_id_to_text_calculator.cc"],
    deps = [
        ":detection_label_id_to_text_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:packet",
        "//cv-cuda-pipeline/framework:resources",
        "//cv-cuda-pipeline/framework/formats:detection_cc_proto",
        "//cv-cuda-pipeline/framework/port:core_proto",
        "//cv-cuda-pipeline/framework/port:integral_types",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/util:label_map_cc_proto",
        "//cv-cuda-pipeline/util:label_map_util",
        "//cv-cuda-pipeline/util:resource_util",
        "@com_google_absl//absl/container:node_hash_map",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings:string_view",
    ] + select({
        "//cv-cuda-pipeline:emscripten": [
        ],
        "//cv-cuda-pipeline:macos": [
            "//cv-cuda-pipeline/framework/port:file_helpers",
        ],
        "//conditions:default": [
            "//cv-cuda-pipeline/framework/port:file_helpers",
        ],
    }),
    alwayslink = 1,
)

cc_test(
    name = "detection_label_id_to_text_calculator_test",
    srcs = ["detection_label_id_to_text_calculator_test.cc"],
    deps = [
        ":detection_label_id_to_text_calculator",
        ":detection_label_id_to_text_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:resources",
        "//cv-cuda-pipeline/framework:resources_service",
        "//cv-cuda-pipeline/framework/api2:builder",
        "//cv-cuda-pipeline/framework/formats:detection_cc_proto",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status_matchers",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:string_view",
    ],
)

cc_library(
    name = "timed_box_list_id_to_label_calculator",
    srcs = ["timed_box_list_id_to_label_calculator.cc"],
    deps = [
        ":timed_box_list_id_to_label_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:packet",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/util:resource_util",
        "//cv-cuda-pipeline/util/tracking:box_tracker_cc_proto",
        "@com_google_absl//absl/container:node_hash_map",
    ] + select({
        "//cv-cuda-pipeline:emscripten": [
        ],
        "//cv-cuda-pipeline:macos": [
            "//cv-cuda-pipeline/framework/port:file_helpers",
        ],
        "//conditions:default": [
            "//cv-cuda-pipeline/framework/port:file_helpers",
        ],
    }),
    alwayslink = 1,
)

cc_library(
    name = "detection_transformation_calculator",
    srcs = ["detection_transformation_calculator.cc"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/api2:node",
        "//cv-cuda-pipeline/framework/api2:packet",
        "//cv-cuda-pipeline/framework/api2:port",
        "//cv-cuda-pipeline/framework/formats:detection_cc_proto",
        "//cv-cuda-pipeline/framework/formats:location_data_cc_proto",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
    ],
    alwayslink = 1,
)

cc_test(
    name = "detection_transformation_calculator_test",
    size = "small",
    srcs = ["detection_transformation_calculator_test.cc"],
    deps = [
        ":detection_transformation_calculator",
        "//cv-cuda-pipeline/framework:calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:calculator_runner",
        "//cv-cuda-pipeline/framework:packet",
        "//cv-cuda-pipeline/framework/formats:detection_cc_proto",
        "//cv-cuda-pipeline/framework/formats:location_data_cc_proto",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
    ],
)

cc_library(
    name = "non_max_suppression_calculator",
    srcs = ["non_max_suppression_calculator.cc"],
    deps = [
        ":non_max_suppression_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/formats:detection_cc_proto",
        "//cv-cuda-pipeline/framework/formats:image_frame",
        "//cv-cuda-pipeline/framework/formats:location",
        "//cv-cuda-pipeline/framework/port:rectangle",
        "//cv-cuda-pipeline/framework/port:status",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/log:absl_check",
        "@com_google_absl//absl/log:absl_log",
    ],
    alwayslink = 1,
)

cc_library(
    name = "thresholding_calculator",
    srcs = ["thresholding_calculator.cc"],
    deps = [
        ":thresholding_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
    ],
    alwayslink = 1,
)

cc_library(
    name = "detection_to_landmarks_calculator",
    srcs = ["detection_to_landmarks_calculator.cc"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/formats:detection_cc_proto",
        "//cv-cuda-pipeline/framework/formats:landmark_cc_proto",
        "//cv-cuda-pipeline/framework/formats:location_data_cc_proto",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
    ],
    alwayslink = 1,
)

cc_library(
    name = "filter_detections_calculator",
    srcs = ["filter_detections_calculator.cc"],
    deps = [
        ":filter_detections_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/formats:detection_cc_proto",
        "//cv-cuda-pipeline/framework/formats:location_data_cc_proto",
        "//cv-cuda-pipeline/framework/port:status",
        "@com_google_absl//absl/memory",
    ],
    alwayslink = 1,
)

cc_library(
    name = "landmarks_to_detection_calculator",
    srcs = ["landmarks_to_detection_calculator.cc"],
    deps = [
        ":landmarks_to_detection_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/formats:detection_cc_proto",
        "//cv-cuda-pipeline/framework/formats:landmark_cc_proto",
        "//cv-cuda-pipeline/framework/formats:location_data_cc_proto",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
    ],
    alwayslink = 1,
)

cc_library(
    name = "detections_to_rects_calculator",
    srcs = [
        "detections_to_rects_calculator.cc",
    ],
    hdrs = [
        "detections_to_rects_calculator.h",
    ],
    deps = [
        ":detections_to_rects_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:calculator_options_cc_proto",
        "//cv-cuda-pipeline/framework/formats:detection_cc_proto",
        "//cv-cuda-pipeline/framework/formats:location_data_cc_proto",
        "//cv-cuda-pipeline/framework/formats:rect_cc_proto",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "@com_google_absl//absl/types:optional",
    ],
    alwayslink = 1,
)

cc_library(
    name = "detections_deduplicate_calculator",
    srcs = [
        "detections_deduplicate_calculator.cc",
    ],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/api2:node",
        "//cv-cuda-pipeline/framework/api2:port",
        "//cv-cuda-pipeline/framework/formats:detection_cc_proto",
        "//cv-cuda-pipeline/framework/formats:location_data_cc_proto",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/status",
    ],
    alwayslink = 1,
)

cc_library(
    name = "rect_transformation_calculator",
    srcs = ["rect_transformation_calculator.cc"],
    deps = [
        ":rect_transformation_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:calculator_options_cc_proto",
        "//cv-cuda-pipeline/framework/formats:rect_cc_proto",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
    ],
    alwayslink = 1,
)

cc_library(
    name = "rect_projection_calculator",
    srcs = ["rect_projection_calculator.cc"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/formats:rect_cc_proto",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
    ],
    alwayslink = 1,
)

cc_test(
    name = "detections_to_rects_calculator_test",
    size = "small",
    srcs = ["detections_to_rects_calculator_test.cc"],
    deps = [
        ":detections_to_rects_calculator",
        "//cv-cuda-pipeline/framework:calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:calculator_runner",
        "//cv-cuda-pipeline/framework:packet",
        "//cv-cuda-pipeline/framework/formats:detection_cc_proto",
        "//cv-cuda-pipeline/framework/formats:location_data_cc_proto",
        "//cv-cuda-pipeline/framework/formats:rect_cc_proto",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
    ],
)

mediapipe_proto_library(
    name = "rect_to_render_data_calculator_proto",
    srcs = ["rect_to_render_data_calculator.proto"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_options_proto",
        "//cv-cuda-pipeline/framework:calculator_proto",
        "//cv-cuda-pipeline/util:color_proto",
        "//cv-cuda-pipeline/util:render_data_proto",
    ],
)

mediapipe_proto_library(
    name = "rect_to_render_scale_calculator_proto",
    srcs = ["rect_to_render_scale_calculator.proto"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_options_proto",
        "//cv-cuda-pipeline/framework:calculator_proto",
    ],
)

mediapipe_proto_library(
    name = "detections_to_render_data_calculator_proto",
    srcs = ["detections_to_render_data_calculator.proto"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_options_proto",
        "//cv-cuda-pipeline/framework:calculator_proto",
        "//cv-cuda-pipeline/util:color_proto",
        "//cv-cuda-pipeline/util:render_data_proto",
    ],
)

mediapipe_proto_library(
    name = "landmarks_to_render_data_calculator_proto",
    srcs = ["landmarks_to_render_data_calculator.proto"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_options_proto",
        "//cv-cuda-pipeline/framework:calculator_proto",
        "//cv-cuda-pipeline/util:color_proto",
        "//cv-cuda-pipeline/util:render_data_proto",
    ],
)

mediapipe_proto_library(
    name = "timed_box_list_to_render_data_calculator_proto",
    srcs = ["timed_box_list_to_render_data_calculator.proto"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_options_proto",
        "//cv-cuda-pipeline/framework:calculator_proto",
        "//cv-cuda-pipeline/util:color_proto",
        "//cv-cuda-pipeline/util:render_data_proto",
    ],
)

mediapipe_proto_library(
    name = "labels_to_render_data_calculator_proto",
    srcs = ["labels_to_render_data_calculator.proto"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_options_proto",
        "//cv-cuda-pipeline/framework:calculator_proto",
        "//cv-cuda-pipeline/util:color_proto",
        "//cv-cuda-pipeline/util:render_data_proto",
    ],
)

mediapipe_proto_library(
    name = "thresholding_calculator_proto",
    srcs = ["thresholding_calculator.proto"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_options_proto",
        "//cv-cuda-pipeline/framework:calculator_proto",
        "//cv-cuda-pipeline/util:color_proto",
        "//cv-cuda-pipeline/util:render_data_proto",
    ],
)

mediapipe_proto_library(
    name = "detections_to_rects_calculator_proto",
    srcs = ["detections_to_rects_calculator.proto"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_options_proto",
        "//cv-cuda-pipeline/framework:calculator_proto",
    ],
)

mediapipe_proto_library(
    name = "landmark_projection_calculator_proto",
    srcs = ["landmark_projection_calculator.proto"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_options_proto",
        "//cv-cuda-pipeline/framework:calculator_proto",
    ],
)

cc_library(
    name = "landmark_visibility_calculator",
    srcs = ["landmark_visibility_calculator.cc"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/formats:landmark_cc_proto",
        "//cv-cuda-pipeline/framework/port:ret_check",
    ],
    alwayslink = 1,
)

cc_library(
    name = "set_landmark_visibility_calculator",
    srcs = ["set_landmark_visibility_calculator.cc"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/formats:landmark_cc_proto",
        "//cv-cuda-pipeline/framework/port:ret_check",
    ],
    alwayslink = 1,
)

mediapipe_proto_library(
    name = "landmarks_to_floats_calculator_proto",
    srcs = ["landmarks_to_floats_calculator.proto"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_options_proto",
        "//cv-cuda-pipeline/framework:calculator_proto",
    ],
)

mediapipe_proto_library(
    name = "rect_transformation_calculator_proto",
    srcs = ["rect_transformation_calculator.proto"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_options_proto",
        "//cv-cuda-pipeline/framework:calculator_proto",
    ],
)

mediapipe_proto_library(
    name = "landmarks_to_detection_calculator_proto",
    srcs = ["landmarks_to_detection_calculator.proto"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_options_proto",
        "//cv-cuda-pipeline/framework:calculator_proto",
        "//cv-cuda-pipeline/util:color_proto",
        "//cv-cuda-pipeline/util:render_data_proto",
    ],
)

cc_library(
    name = "detections_to_render_data_calculator",
    srcs = ["detections_to_render_data_calculator.cc"],
    deps = [
        ":detections_to_render_data_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:calculator_options_cc_proto",
        "//cv-cuda-pipeline/framework/formats:detection_cc_proto",
        "//cv-cuda-pipeline/framework/formats:location_data_cc_proto",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/util:color_cc_proto",
        "//cv-cuda-pipeline/util:render_data_cc_proto",
        "@com_google_absl//absl/log:absl_check",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/strings",
    ],
    alwayslink = 1,
)

cc_library(
    name = "landmarks_to_render_data_calculator",
    srcs = ["landmarks_to_render_data_calculator.cc"],
    hdrs = ["landmarks_to_render_data_calculator.h"],
    deps = [
        ":landmarks_to_render_data_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:calculator_options_cc_proto",
        "//cv-cuda-pipeline/framework/formats:landmark_cc_proto",
        "//cv-cuda-pipeline/framework/formats:location_data_cc_proto",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/util:color_cc_proto",
        "//cv-cuda-pipeline/util:render_data_cc_proto",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/strings",
    ],
    alwayslink = 1,
)

cc_library(
    name = "timed_box_list_to_render_data_calculator",
    srcs = ["timed_box_list_to_render_data_calculator.cc"],
    deps = [
        ":timed_box_list_to_render_data_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:calculator_options_cc_proto",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/util:color_cc_proto",
        "//cv-cuda-pipeline/util:render_data_cc_proto",
        "//cv-cuda-pipeline/util/tracking:box_tracker_cc_proto",
        "//cv-cuda-pipeline/util/tracking:tracking_cc_proto",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/strings",
    ],
    alwayslink = 1,
)

cc_library(
    name = "labels_to_render_data_calculator",
    srcs = ["labels_to_render_data_calculator.cc"],
    deps = [
        ":labels_to_render_data_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/formats:classification_cc_proto",
        "//cv-cuda-pipeline/framework/formats:video_stream_header",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/framework/port:statusor",
        "//cv-cuda-pipeline/util:color_cc_proto",
        "//cv-cuda-pipeline/util:render_data_cc_proto",
        "@com_google_absl//absl/log:absl_check",
        "@com_google_absl//absl/strings",
    ],
    alwayslink = 1,
)

cc_library(
    name = "rect_to_render_data_calculator",
    srcs = ["rect_to_render_data_calculator.cc"],
    deps = [
        ":rect_to_render_data_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/formats:rect_cc_proto",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/util:color_cc_proto",
        "//cv-cuda-pipeline/util:render_data_cc_proto",
        "@com_google_absl//absl/log:absl_check",
    ],
    alwayslink = 1,
)

cc_library(
    name = "rect_to_render_scale_calculator",
    srcs = ["rect_to_render_scale_calculator.cc"],
    deps = [
        ":rect_to_render_scale_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/formats:rect_cc_proto",
        "//cv-cuda-pipeline/framework/port:ret_check",
    ],
    alwayslink = 1,
)

cc_test(
    name = "detections_to_render_data_calculator_test",
    size = "small",
    srcs = ["detections_to_render_data_calculator_test.cc"],
    deps = [
        ":detections_to_render_data_calculator",
        ":detections_to_render_data_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:calculator_runner",
        "//cv-cuda-pipeline/framework:packet",
        "//cv-cuda-pipeline/framework/formats:detection_cc_proto",
        "//cv-cuda-pipeline/framework/formats:location_data_cc_proto",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/util:color_cc_proto",
        "//cv-cuda-pipeline/util:render_data_cc_proto",
        "@com_google_absl//absl/memory",
    ],
)

cc_library(
    name = "detection_letterbox_removal_calculator",
    srcs = ["detection_letterbox_removal_calculator.cc"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/formats:detection_cc_proto",
        "//cv-cuda-pipeline/framework/formats:location",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
    ],
    alwayslink = 1,
)

cc_library(
    name = "detection_projection_calculator",
    srcs = ["detection_projection_calculator.cc"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/formats:detection_cc_proto",
        "//cv-cuda-pipeline/framework/formats:location",
        "//cv-cuda-pipeline/framework/formats:rect_cc_proto",
        "//cv-cuda-pipeline/framework/port:point",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
    ],
    alwayslink = 1,
)

cc_test(
    name = "detection_projection_calculator_test",
    srcs = ["detection_projection_calculator_test.cc"],
    deps = [
        ":detection_projection_calculator",
        "//cv-cuda-pipeline/calculators/tensor:image_to_tensor_utils",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:calculator_runner",
        "//cv-cuda-pipeline/framework/formats:detection_cc_proto",
        "//cv-cuda-pipeline/framework/formats:location",
        "//cv-cuda-pipeline/framework/formats:rect_cc_proto",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
        "//cv-cuda-pipeline/framework/port:point",
    ],
)

cc_library(
    name = "landmark_letterbox_removal_calculator",
    srcs = ["landmark_letterbox_removal_calculator.cc"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/formats:landmark_cc_proto",
        "//cv-cuda-pipeline/framework/formats:location",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
    ],
    alwayslink = 1,
)

cc_library(
    name = "landmark_projection_calculator",
    srcs = ["landmark_projection_calculator.cc"],
    hdrs = ["landmark_projection_calculator.h"],
    deps = [
        ":landmark_projection_calculator_cc_proto",
        "//cv-cuda-pipeline/calculators/tensor:image_to_tensor_utils",
        "//cv-cuda-pipeline/framework/api3:calculator",
        "//cv-cuda-pipeline/framework/api3:calculator_context",
        "//cv-cuda-pipeline/framework/api3:calculator_contract",
        "//cv-cuda-pipeline/framework/api3:contract",
        "//cv-cuda-pipeline/framework/api3:node",
        "//cv-cuda-pipeline/framework/formats:landmark_cc_proto",
        "//cv-cuda-pipeline/framework/formats:rect_cc_proto",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "@com_google_absl//absl/log:absl_log",
        "@com_google_absl//absl/status",
    ],
    alwayslink = 1,
)

cc_test(
    name = "landmark_projection_calculator_test",
    srcs = ["landmark_projection_calculator_test.cc"],
    deps = [
        ":landmark_projection_calculator",
        "//cv-cuda-pipeline/calculators/tensor:image_to_tensor_utils",
        "//cv-cuda-pipeline/framework:calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:calculator_runner",
        "//cv-cuda-pipeline/framework/api3:contract",
        "//cv-cuda-pipeline/framework/api3:function_runner",
        "//cv-cuda-pipeline/framework/api3:graph",
        "//cv-cuda-pipeline/framework/api3:packet",
        "//cv-cuda-pipeline/framework/api3:stream",
        "//cv-cuda-pipeline/framework/formats:landmark_cc_proto",
        "//cv-cuda-pipeline/framework/formats:rect_cc_proto",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "world_landmark_projection_calculator",
    srcs = ["world_landmark_projection_calculator.cc"],
    hdrs = ["world_landmark_projection_calculator.h"],
    deps = [
        "//cv-cuda-pipeline/framework/api3:calculator",
        "//cv-cuda-pipeline/framework/api3:calculator_context",
        "//cv-cuda-pipeline/framework/api3:contract",
        "//cv-cuda-pipeline/framework/api3:node",
        "//cv-cuda-pipeline/framework/formats:landmark_cc_proto",
        "//cv-cuda-pipeline/framework/formats:rect_cc_proto",
        "@com_google_absl//absl/status",
    ],
    alwayslink = 1,
)

cc_test(
    name = "world_landmark_projection_calculator_test",
    srcs = ["world_landmark_projection_calculator_test.cc"],
    deps = [
        ":world_landmark_projection_calculator",
        "//cv-cuda-pipeline/framework:calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:calculator_runner",
        "//cv-cuda-pipeline/framework/api3:graph",
        "//cv-cuda-pipeline/framework/api3:stream",
        "//cv-cuda-pipeline/framework/formats:landmark_cc_proto",
        "//cv-cuda-pipeline/framework/formats:rect_cc_proto",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/util:packet_test_util",
        "@com_google_absl//absl/status",
        "@com_google_googletest//:gtest_main",
    ],
)

mediapipe_proto_library(
    name = "landmarks_smoothing_calculator_proto",
    srcs = ["landmarks_smoothing_calculator.proto"],
    deps = ["//cv-cuda-pipeline/framework:calculator_options_proto"],
)

cc_library(
    name = "landmarks_smoothing_calculator",
    srcs = ["landmarks_smoothing_calculator.cc"],
    hdrs = ["landmarks_smoothing_calculator.h"],
    deps = [
        ":landmarks_smoothing_calculator_cc_proto",
        ":landmarks_smoothing_calculator_utils",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:timestamp",
        "//cv-cuda-pipeline/framework/api2:node",
        "//cv-cuda-pipeline/framework/formats:landmark_cc_proto",
        "//cv-cuda-pipeline/framework/formats:rect_cc_proto",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "@com_google_absl//absl/status:status_macros",
    ],
    alwayslink = 1,
)

cc_library(
    name = "landmarks_smoothing_calculator_utils",
    srcs = ["landmarks_smoothing_calculator_utils.cc"],
    hdrs = ["landmarks_smoothing_calculator_utils.h"],
    deps = [
        ":landmarks_smoothing_calculator_cc_proto",
        "//cv-cuda-pipeline/framework/formats:landmark_cc_proto",
        "//cv-cuda-pipeline/framework/formats:rect_cc_proto",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/util/filtering:one_euro_filter",
        "//cv-cuda-pipeline/util/filtering:relative_velocity_filter",
        "@com_google_absl//absl/algorithm:container",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/time",
    ],
    alwayslink = 1,
)

cc_test(
    name = "landmarks_smoothing_calculator_utils_test",
    size = "small",
    srcs = ["landmarks_smoothing_calculator_utils_test.cc"],
    deps = [
        ":landmarks_smoothing_calculator_utils",
        "//cv-cuda-pipeline/framework/formats:landmark_cc_proto",
        "//cv-cuda-pipeline/framework/port:gtest_main",
    ],
)

cc_library(
    name = "multi_landmarks_smoothing_calculator",
    srcs = ["multi_landmarks_smoothing_calculator.cc"],
    hdrs = ["multi_landmarks_smoothing_calculator.h"],
    deps = [
        ":landmarks_smoothing_calculator_cc_proto",
        ":landmarks_smoothing_calculator_utils",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:timestamp",
        "//cv-cuda-pipeline/framework/api2:node",
        "//cv-cuda-pipeline/framework/formats:landmark_cc_proto",
        "//cv-cuda-pipeline/framework/formats:rect_cc_proto",
        "//cv-cuda-pipeline/framework/port:ret_check",
    ],
    alwayslink = 1,
)

cc_library(
    name = "multi_world_landmarks_smoothing_calculator",
    srcs = ["multi_world_landmarks_smoothing_calculator.cc"],
    hdrs = ["multi_world_landmarks_smoothing_calculator.h"],
    deps = [
        ":landmarks_smoothing_calculator_cc_proto",
        ":landmarks_smoothing_calculator_utils",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:timestamp",
        "//cv-cuda-pipeline/framework/api2:node",
        "//cv-cuda-pipeline/framework/formats:landmark_cc_proto",
        "//cv-cuda-pipeline/framework/formats:rect_cc_proto",
        "//cv-cuda-pipeline/framework/port:ret_check",
    ],
    alwayslink = 1,
)

mediapipe_proto_library(
    name = "visibility_smoothing_calculator_proto",
    srcs = ["visibility_smoothing_calculator.proto"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_options_proto",
        "//cv-cuda-pipeline/framework:calculator_proto",
    ],
)

cc_library(
    name = "visibility_smoothing_calculator",
    srcs = ["visibility_smoothing_calculator.cc"],
    deps = [
        ":visibility_smoothing_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:timestamp",
        "//cv-cuda-pipeline/framework/formats:landmark_cc_proto",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/util/filtering:low_pass_filter",
        "@com_google_absl//absl/algorithm:container",
    ],
    alwayslink = 1,
)

mediapipe_proto_library(
    name = "visibility_copy_calculator_proto",
    srcs = ["visibility_copy_calculator.proto"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_options_proto",
        "//cv-cuda-pipeline/framework:calculator_proto",
    ],
)

cc_library(
    name = "visibility_copy_calculator",
    srcs = ["visibility_copy_calculator.cc"],
    hdrs = ["visibility_copy_calculator.h"],
    deps = [
        ":visibility_copy_calculator_cc_proto",
        "//cv-cuda-pipeline/framework/api3:calculator",
        "//cv-cuda-pipeline/framework/api3:calculator_context",
        "//cv-cuda-pipeline/framework/api3:calculator_contract",
        "//cv-cuda-pipeline/framework/api3:contract",
        "//cv-cuda-pipeline/framework/api3:node",
        "//cv-cuda-pipeline/framework/formats:landmark_cc_proto",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "@com_google_absl//absl/status",
    ],
    alwayslink = 1,
)

cc_library(
    name = "landmarks_to_floats_calculator",
    srcs = ["landmarks_to_floats_calculator.cc"],
    deps = [
        ":landmarks_to_floats_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/formats:landmark_cc_proto",
        "//cv-cuda-pipeline/framework/formats:matrix",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "@eigen//:eigen3",
    ],
    alwayslink = 1,
)

cc_test(
    name = "detection_letterbox_removal_calculator_test",
    srcs = ["detection_letterbox_removal_calculator_test.cc"],
    deps = [
        ":detection_letterbox_removal_calculator",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:calculator_runner",
        "//cv-cuda-pipeline/framework/formats:detection_cc_proto",
        "//cv-cuda-pipeline/framework/formats:location",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:integral_types",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
        "//cv-cuda-pipeline/framework/tool:validate_type",
    ],
)

cc_test(
    name = "landmark_letterbox_removal_calculator_test",
    srcs = ["landmark_letterbox_removal_calculator_test.cc"],
    deps = [
        ":landmark_letterbox_removal_calculator",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:calculator_runner",
        "//cv-cuda-pipeline/framework/formats:landmark_cc_proto",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:integral_types",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
        "//cv-cuda-pipeline/framework/tool:validate_type",
    ],
)

mediapipe_proto_library(
    name = "top_k_scores_calculator_proto",
    srcs = ["top_k_scores_calculator.proto"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_options_proto",
        "//cv-cuda-pipeline/framework:calculator_proto",
    ],
)

cc_library(
    name = "top_k_scores_calculator",
    srcs = ["top_k_scores_calculator.cc"],
    deps = [
        ":top_k_scores_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/formats:classification_cc_proto",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/framework/port:statusor",
        "//cv-cuda-pipeline/util:resource_util",
        "@com_google_absl//absl/container:node_hash_map",
    ] + select({
        "//cv-cuda-pipeline:emscripten": [
        ],
        "//cv-cuda-pipeline:macos": [
            "//cv-cuda-pipeline/framework/port:file_helpers",
        ],
        "//conditions:default": [
            "//cv-cuda-pipeline/framework/port:file_helpers",
        ],
    }),
    alwayslink = 1,
)

cc_test(
    name = "top_k_scores_calculator_test",
    srcs = ["top_k_scores_calculator_test.cc"],
    deps = [
        ":top_k_scores_calculator",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:calculator_runner",
        "//cv-cuda-pipeline/framework:packet",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
        "//cv-cuda-pipeline/framework/port:status",
    ],
)

mediapipe_proto_library(
    name = "local_file_contents_calculator_proto",
    srcs = ["local_file_contents_calculator.proto"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_options_proto",
        "//cv-cuda-pipeline/framework:calculator_proto",
    ],
)

cc_library(
    name = "local_file_contents_calculator",
    srcs = ["local_file_contents_calculator.cc"],
    deps = [
        ":local_file_contents_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:resources",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/util:resource_util",
        "@com_google_absl//absl/status",
    ],
    alwayslink = 1,
)

mediapipe_proto_library(
    name = "resource_provider_calculator_proto",
    srcs = ["resource_provider_calculator.proto"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_options_proto",
        "//cv-cuda-pipeline/framework:calculator_proto",
    ],
)

cc_library(
    name = "resource_provider_calculator",
    srcs = ["resource_provider_calculator.cc"],
    hdrs = ["resource_provider_calculator.h"],
    deps = [
        ":resource_provider_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:resources",
        "//cv-cuda-pipeline/framework/api2:node",
        "//cv-cuda-pipeline/framework/api2:packet",
        "//cv-cuda-pipeline/framework/api2:port",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "@com_google_absl//absl/functional:any_invocable",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings:string_view",
    ],
    alwayslink = 1,
)

cc_test(
    name = "resource_provider_calculator_test",
    srcs = ["resource_provider_calculator_test.cc"],
    deps = [
        ":resource_provider_calculator",
        ":resource_provider_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:resources",
        "//cv-cuda-pipeline/framework:resources_service",
        "//cv-cuda-pipeline/framework/api2:builder",
        "//cv-cuda-pipeline/framework/api2:packet",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/util:resources_test_util",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/strings",
    ],
)

cc_library(
    name = "local_file_pattern_contents_calculator",
    srcs = ["local_file_pattern_contents_calculator.cc"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/port:file_helpers",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "@com_google_absl//absl/log:absl_log",
    ],
    alwayslink = 1,
)

cc_library(
    name = "filter_collection_calculator",
    srcs = ["filter_collection_calculator.cc"],
    hdrs = ["filter_collection_calculator.h"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/formats:classification_cc_proto",
        "//cv-cuda-pipeline/framework/formats:landmark_cc_proto",
        "//cv-cuda-pipeline/framework/formats:rect_cc_proto",
        "//cv-cuda-pipeline/framework/port:integral_types",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "@com_google_absl//absl/strings",
    ],
    alwayslink = 1,
)

cc_library(
    name = "collection_has_min_size_calculator",
    srcs = ["collection_has_min_size_calculator.cc"],
    hdrs = ["collection_has_min_size_calculator.h"],
    deps = [
        ":collection_has_min_size_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/formats:classification_cc_proto",
        "//cv-cuda-pipeline/framework/formats:landmark_cc_proto",
        "//cv-cuda-pipeline/framework/formats:rect_cc_proto",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
    ],
    alwayslink = 1,
)

cc_test(
    name = "collection_has_min_size_calculator_test",
    srcs = ["collection_has_min_size_calculator_test.cc"],
    deps = [
        ":collection_has_min_size_calculator",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:calculator_runner",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
    ],
)

cc_library(
    name = "association_calculator",
    hdrs = ["association_calculator.h"],
    deps = [
        ":association_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_context",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:collection_item_id",
        "//cv-cuda-pipeline/framework/port:rectangle",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/util:rectangle_util",
        "@com_google_absl//absl/log:absl_check",
        "@com_google_absl//absl/memory",
    ],
    alwayslink = 1,
)

cc_library(
    name = "association_norm_rect_calculator",
    srcs = ["association_norm_rect_calculator.cc"],
    deps = [
        ":association_calculator",
        "//cv-cuda-pipeline/framework:calculator_context",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/formats:rect_cc_proto",
        "//cv-cuda-pipeline/framework/port:rectangle",
        "//cv-cuda-pipeline/framework/port:status",
    ],
    alwayslink = 1,
)

cc_library(
    name = "association_detection_calculator",
    srcs = ["association_detection_calculator.cc"],
    deps = [
        ":association_calculator",
        "//cv-cuda-pipeline/framework:calculator_context",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/formats:detection_cc_proto",
        "//cv-cuda-pipeline/framework/formats:location",
        "//cv-cuda-pipeline/framework/port:rectangle",
        "//cv-cuda-pipeline/framework/port:status",
    ],
    alwayslink = 1,
)

cc_test(
    name = "association_calculator_test",
    srcs = ["association_calculator_test.cc"],
    deps = [
        ":association_detection_calculator",
        ":association_norm_rect_calculator",
        "//cv-cuda-pipeline/framework:calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:calculator_runner",
        "//cv-cuda-pipeline/framework:collection_item_id",
        "//cv-cuda-pipeline/framework:packet",
        "//cv-cuda-pipeline/framework/formats:detection_cc_proto",
        "//cv-cuda-pipeline/framework/formats:location_data_cc_proto",
        "//cv-cuda-pipeline/framework/formats:rect_cc_proto",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
    ],
)

cc_library(
    name = "detections_to_timed_box_list_calculator",
    srcs = ["detections_to_timed_box_list_calculator.cc"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/formats:detection_cc_proto",
        "//cv-cuda-pipeline/framework/formats:location_data_cc_proto",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/util/tracking:box_tracker",
    ],
    alwayslink = 1,
)

cc_library(
    name = "detection_unique_id_calculator",
    srcs = ["detection_unique_id_calculator.cc"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/formats:detection_cc_proto",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
    ],
    alwayslink = 1,
)

mediapipe_proto_library(
    name = "logic_calculator_proto",
    srcs = ["logic_calculator.proto"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_options_proto",
        "//cv-cuda-pipeline/framework:calculator_proto",
    ],
)

cc_library(
    name = "logic_calculator",
    srcs = ["logic_calculator.cc"],
    deps = [
        ":logic_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
    ],
    alwayslink = 1,
)

cc_library(
    name = "to_image_calculator",
    srcs = ["to_image_calculator.cc"],
    hdrs = ["to_image_calculator.h"],
    deps = [
        "//cv-cuda-pipeline/framework/api3:any",
        "//cv-cuda-pipeline/framework/api3:calculator",
        "//cv-cuda-pipeline/framework/api3:calculator_context",
        "//cv-cuda-pipeline/framework/api3:calculator_contract",
        "//cv-cuda-pipeline/framework/api3:contract",
        "//cv-cuda-pipeline/framework/api3:node",
        "//cv-cuda-pipeline/framework/api3:one_of",
        "//cv-cuda-pipeline/framework/api3:packet",
        "//cv-cuda-pipeline/framework/formats:image",
        "//cv-cuda-pipeline/framework/formats:image_frame",
        "//cv-cuda-pipeline/framework/port:status",
        "@com_google_absl//absl/functional:overload",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
    ] + select({
        "//cv-cuda-pipeline/gpu:disable_gpu": [],
        "//conditions:default": [
            "//cv-cuda-pipeline/gpu:gl_calculator_helper",
            "//cv-cuda-pipeline/gpu:gpu_buffer",
        ],
    }),
    alwayslink = 1,
)


mediapipe_proto_library(
    name = "flat_color_image_calculator_proto",
    srcs = ["flat_color_image_calculator.proto"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_options_proto",
        "//cv-cuda-pipeline/framework:calculator_proto",
        "//cv-cuda-pipeline/util:color_proto",
    ],
)

cc_library(
    name = "flat_color_image_calculator",
    srcs = ["flat_color_image_calculator.cc"],
    deps = [
        ":flat_color_image_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_contract",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/api2:node",
        "//cv-cuda-pipeline/framework/formats:image",
        "//cv-cuda-pipeline/framework/formats:image_frame",
        "//cv-cuda-pipeline/framework/formats:image_frame_opencv",
        "//cv-cuda-pipeline/framework/port:opencv_core",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/util:color_cc_proto",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings",
    ],
    alwayslink = 1,
)

cc_test(
    name = "flat_color_image_calculator_test",
    srcs = ["flat_color_image_calculator_test.cc"],
    deps = [
        ":flat_color_image_calculator",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:calculator_runner",
        "//cv-cuda-pipeline/framework:packet",
        "//cv-cuda-pipeline/framework/formats:image",
        "//cv-cuda-pipeline/framework/formats:image_frame",
        "//cv-cuda-pipeline/framework/port:gtest",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/util:color_cc_proto",
    ],
)

cc_library(
    name = "from_image_calculator",
    srcs = ["from_image_calculator.cc"],
    hdrs = ["from_image_calculator.h"],
    deps = [
        "//cv-cuda-pipeline/framework/api3:any",
        "//cv-cuda-pipeline/framework/api3:calculator",
        "//cv-cuda-pipeline/framework/api3:calculator_context",
        "//cv-cuda-pipeline/framework/api3:calculator_contract",
        "//cv-cuda-pipeline/framework/api3:contract",
        "//cv-cuda-pipeline/framework/api3:node",
        "//cv-cuda-pipeline/framework/api3:packet",
        "//cv-cuda-pipeline/framework/formats:image",
        "//cv-cuda-pipeline/framework/formats:image_format_cc_proto",
        "//cv-cuda-pipeline/framework/formats:image_frame",
        "//cv-cuda-pipeline/framework/port:logging",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/framework/port:vector",
        "//cv-cuda-pipeline/gpu:gpu_buffer",
        "@com_google_absl//absl/status",
    ] + select({
        "//cv-cuda-pipeline/gpu:disable_gpu": [],
        "//conditions:default": [
            "//cv-cuda-pipeline/gpu:gl_calculator_helper",
            "//cv-cuda-pipeline/gpu:gpu_service",
        ],
    }),
    alwayslink = 1,
)


cc_library(
    name = "detection_classifications_merger_calculator",
    srcs = ["detection_classifications_merger_calculator.cc"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/api2:node",
        "//cv-cuda-pipeline/framework/formats:classification_cc_proto",
        "//cv-cuda-pipeline/framework/formats:detection_cc_proto",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/framework/port:statusor",
        "@com_google_absl//absl/strings",
    ],
    alwayslink = 1,
)

cc_test(
    name = "detection_classifications_merger_calculator_test",
    srcs = ["detection_classifications_merger_calculator_test.cc"],
    deps = [
        ":detection_classifications_merger_calculator",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:calculator_runner",
        "//cv-cuda-pipeline/framework/formats:classification_cc_proto",
        "//cv-cuda-pipeline/framework/formats:detection_cc_proto",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
    ],
)

mediapipe_proto_library(
    name = "refine_landmarks_from_heatmap_calculator_proto",
    srcs = ["refine_landmarks_from_heatmap_calculator.proto"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_options_proto",
        "//cv-cuda-pipeline/framework:calculator_proto",
    ],
)

cc_library(
    name = "refine_landmarks_from_heatmap_calculator",
    srcs = ["refine_landmarks_from_heatmap_calculator.cc"],
    hdrs = ["refine_landmarks_from_heatmap_calculator.h"],
    copts = select({
        "//cv-cuda-pipeline:apple": [
            "-x objective-c++",
            "-fobjc-arc",  # enable reference-counting
        ],
        "//conditions:default": [],
    }),
    deps = [
        ":refine_landmarks_from_heatmap_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/api2:node",
        "//cv-cuda-pipeline/framework/formats:landmark_cc_proto",
        "//cv-cuda-pipeline/framework/formats:tensor",
        "//cv-cuda-pipeline/framework/port:statusor",
    ],
    alwayslink = 1,
)

mediapipe_proto_library(
    name = "landmarks_refinement_calculator_proto",
    srcs = ["landmarks_refinement_calculator.proto"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_options_proto",
        "//cv-cuda-pipeline/framework:calculator_proto",
    ],
)

cc_library(
    name = "landmarks_refinement_calculator",
    srcs = ["landmarks_refinement_calculator.cc"],
    hdrs = ["landmarks_refinement_calculator.h"],
    deps = [
        ":landmarks_refinement_calculator_cc_proto",
        "//cv-cuda-pipeline/framework/api3:calculator",
        "//cv-cuda-pipeline/framework/api3:calculator_context",
        "//cv-cuda-pipeline/framework/api3:contract",
        "//cv-cuda-pipeline/framework/api3:node",
        "//cv-cuda-pipeline/framework/formats:landmark_cc_proto",
        "//cv-cuda-pipeline/framework/port:core_proto",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "@com_google_absl//absl/log:absl_check",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
    ],
    alwayslink = 1,
)

cc_test(
    name = "refine_landmarks_from_heatmap_calculator_test",
    srcs = ["refine_landmarks_from_heatmap_calculator_test.cc"],
    deps = [
        ":refine_landmarks_from_heatmap_calculator",
        "//cv-cuda-pipeline/framework/port:gtest_main",
    ],
)

cc_library(
    name = "inverse_matrix_calculator",
    srcs = ["inverse_matrix_calculator.cc"],
    hdrs = ["inverse_matrix_calculator.h"],
    deps = [
        "//cv-cuda-pipeline/framework/api3:calculator",
        "//cv-cuda-pipeline/framework/api3:calculator_context",
        "//cv-cuda-pipeline/framework/api3:contract",
        "//cv-cuda-pipeline/framework/api3:node",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "@com_google_absl//absl/status",
        "@eigen//:eigen3",
    ],
    alwayslink = True,
)

cc_test(
    name = "inverse_matrix_calculator_test",
    srcs = ["inverse_matrix_calculator_test.cc"],
    tags = ["desktop_only_test"],
    deps = [
        ":inverse_matrix_calculator",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:calculator_runner",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
        "@com_google_absl//absl/types:span",
    ],
)

mediapipe_proto_library(
    name = "face_to_rect_calculator_proto",
    srcs = ["face_to_rect_calculator.proto"],
)

cc_library(
    name = "face_to_rect_calculator",
    srcs = ["face_to_rect_calculator.cc"],
    deps = [
        ":detections_to_rects_calculator",
        ":face_to_rect_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/formats:detection_cc_proto",
        "//cv-cuda-pipeline/framework/formats:location_data_cc_proto",
        "//cv-cuda-pipeline/framework/formats:rect_cc_proto",
        "//cv-cuda-pipeline/framework/port:vector",
        "@com_google_absl//absl/status",
    ],
    alwayslink = 1,
)

cc_test(
    name = "face_to_rect_calculator_test",
    size = "small",
    srcs = ["face_to_rect_calculator_test.cc"],
    deps = [
        ":face_to_rect_calculator",
        ":face_to_rect_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:calculator_runner",
        "//cv-cuda-pipeline/framework:packet",
        "//cv-cuda-pipeline/framework/formats:detection_cc_proto",
        "//cv-cuda-pipeline/framework/formats:location_data_cc_proto",
        "//cv-cuda-pipeline/framework/formats:rect_cc_proto",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
        "//cv-cuda-pipeline/framework/port:status_matchers",
        "@com_google_absl//absl/status",
    ],
)

mediapipe_proto_library(
    name = "landmarks_transformation_calculator_proto",
    srcs = ["landmarks_transformation_calculator.proto"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_options_proto",
        "//cv-cuda-pipeline/framework:calculator_proto",
    ],
)

cc_library(
    name = "landmarks_transformation_calculator",
    srcs = ["landmarks_transformation_calculator.cc"],
    hdrs = ["landmarks_transformation_calculator.h"],
    deps = [
        ":landmarks_transformation_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/api2:node",
        "//cv-cuda-pipeline/framework/formats:landmark_cc_proto",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:statusor",
    ],
    alwayslink = 1,
)

cc_test(
    name = "landmarks_transformation_calculator_test",
    srcs = ["landmarks_transformation_calculator_test.cc"],
    deps = [
        ":landmarks_transformation_calculator",
        ":landmarks_transformation_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:calculator_runner",
        "//cv-cuda-pipeline/framework:packet",
        "//cv-cuda-pipeline/framework/formats:landmark_cc_proto",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
        "//cv-cuda-pipeline/framework/port:status_matchers",
        "@com_google_absl//absl/strings",
    ],
)

cc_library(
    name = "combine_joints_calculator",
    srcs = ["combine_joints_calculator.cc"],
    hdrs = ["combine_joints_calculator.h"],
    deps = [
        ":combine_joints_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/api2:node",
        "//cv-cuda-pipeline/framework/formats:body_rig_cc_proto",
        "//cv-cuda-pipeline/framework/port:ret_check",
    ],
    alwayslink = 1,
)

mediapipe_proto_library(
    name = "combine_joints_calculator_proto",
    srcs = ["combine_joints_calculator.proto"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_options_proto",
        "//cv-cuda-pipeline/framework:calculator_proto",
        "//cv-cuda-pipeline/framework/formats:body_rig_proto",
    ],
)

cc_test(
    name = "combine_joints_calculator_test",
    srcs = ["combine_joints_calculator_test.cc"],
    deps = [
        ":combine_joints_calculator",
        ":combine_joints_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:calculator_runner",
        "//cv-cuda-pipeline/framework:packet",
        "//cv-cuda-pipeline/framework/formats:body_rig_cc_proto",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
        "//cv-cuda-pipeline/framework/port:status_matchers",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
    ],
)

cc_library(
    name = "set_joints_visibility_calculator",
    srcs = ["set_joints_visibility_calculator.cc"],
    hdrs = ["set_joints_visibility_calculator.h"],
    deps = [
        ":set_joints_visibility_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/api2:node",
        "//cv-cuda-pipeline/framework/formats:body_rig_cc_proto",
        "//cv-cuda-pipeline/framework/formats:landmark_cc_proto",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
    ],
    alwayslink = 1,
)

mediapipe_proto_library(
    name = "set_joints_visibility_calculator_proto",
    srcs = ["set_joints_visibility_calculator.proto"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_options_proto",
        "//cv-cuda-pipeline/framework:calculator_proto",
    ],
)

cc_test(
    name = "set_joints_visibility_calculator_test",
    srcs = ["set_joints_visibility_calculator_test.cc"],
    deps = [
        ":set_joints_visibility_calculator",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:calculator_runner",
        "//cv-cuda-pipeline/framework:packet",
        "//cv-cuda-pipeline/framework/formats:body_rig_cc_proto",
        "//cv-cuda-pipeline/framework/formats:landmark_cc_proto",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
        "//cv-cuda-pipeline/framework/port:status_matchers",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/types:optional",
    ],
)

cc_library(
    name = "pass_through_or_empty_detection_vector_calculator",
    srcs = ["pass_through_or_empty_detection_vector_calculator.cc"],
    hdrs = ["pass_through_or_empty_detection_vector_calculator.h"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_context",
        "//cv-cuda-pipeline/framework/api2:builder",
        "//cv-cuda-pipeline/framework/api2:node",
        "//cv-cuda-pipeline/framework/api2:packet",
        "//cv-cuda-pipeline/framework/api2:port",
        "//cv-cuda-pipeline/framework/formats:detection_cc_proto",
        "@com_google_absl//absl/status",
    ],
    alwayslink = 1,
)

cc_test(
    name = "pass_through_or_empty_detection_vector_calculator_test",
    srcs = ["pass_through_or_empty_detection_vector_calculator_test.cc"],
    tags = ["desktop_only_test"],
    deps = [
        ":pass_through_or_empty_detection_vector_calculator",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/api2:builder",
        "//cv-cuda-pipeline/framework/formats:detection_cc_proto",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:status_matchers",
        "@com_google_absl//absl/status",
    ],
)
