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

load("@bazel_skylib//lib:selects.bzl", "selects")
load("@rules_cc//cc:cc_library.bzl", "cc_library")
load("@rules_cc//cc:cc_test.bzl", "cc_test")
load("//cv-cuda-pipeline/framework:mediapipe_register_type.bzl", "mediapipe_register_type")
load("//cv-cuda-pipeline/framework/port:build_config.bzl", "mediapipe_proto_library")

package(
    default_visibility = ["//visibility:public"],
    features = ["-layering_check"],
)

licenses(["notice"])


mediapipe_proto_library(
    name = "detection_proto",
    srcs = ["detection.proto"],
    deps = [":location_data_proto"],
)

mediapipe_register_type(
    base_name = "detection",
    include_headers = ["cv-cuda-pipeline/framework/formats/detection.pb.h"],
    types = [
        "::mediapipe::Detection",
        "::mediapipe::DetectionList",
        "::std::vector<::mediapipe::Detection>",
        "::std::vector<::mediapipe::DetectionList>",
    ],
    deps = [":detection_cc_proto"],
)

mediapipe_proto_library(
    name = "classification_proto",
    srcs = ["classification.proto"],
)

mediapipe_register_type(
    base_name = "classification",
    include_headers = ["cv-cuda-pipeline/framework/formats/classification.pb.h"],
    types = [
        "::mediapipe::Classification",
        "::mediapipe::ClassificationList",
        "::mediapipe::ClassificationListCollection",
        "::std::vector<::mediapipe::Classification>",
        "::std::vector<::mediapipe::ClassificationList>",
    ],
    deps = [":classification_cc_proto"],
)

mediapipe_proto_library(
    name = "image_format_proto",
    srcs = ["image_format.proto"],
)

mediapipe_proto_library(
    name = "matrix_data_proto",
    srcs = ["matrix_data.proto"],
)

mediapipe_proto_library(
    name = "location_data_proto",
    srcs = ["location_data.proto"],
    deps = ["//cv-cuda-pipeline/framework/formats/annotation:rasterization_proto"],
)

mediapipe_proto_library(
    name = "affine_transform_data_proto",
    srcs = ["affine_transform_data.proto"],
)

mediapipe_proto_library(
    name = "time_series_header_proto",
    srcs = ["time_series_header.proto"],
)

mediapipe_proto_library(
    name = "image_file_properties_proto",
    srcs = ["image_file_properties.proto"],
)

cc_library(
    name = "deleting_file",
    srcs = ["deleting_file.cc"],
    hdrs = ["deleting_file.h"],
    deps = [
        "@com_google_absl//absl/log:absl_log",
    ],
)

cc_library(
    name = "matrix",
    srcs = ["matrix.cc"],
    hdrs = ["matrix.h"],
    deps = [
        ":matrix_data_cc_proto",
        "//cv-cuda-pipeline/framework:port",
        "//cv-cuda-pipeline/framework/port:core_proto",
        "//cv-cuda-pipeline/framework/port:logging",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "@com_google_absl//absl/log:absl_check",
        "@eigen//:eigen3",
    ],
)

cc_library(
    name = "affine_transform",
    srcs = ["affine_transform.cc"],
    hdrs = ["affine_transform.h"],
    deps = [
        ":affine_transform_data_cc_proto",
        "//cv-cuda-pipeline/framework:port",
        "//cv-cuda-pipeline/framework:type_map",
        "//cv-cuda-pipeline/framework/port:integral_types",
        "//cv-cuda-pipeline/framework/port:logging",
        "//cv-cuda-pipeline/framework/port:point",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/framework/port:statusor",
        "//cv-cuda-pipeline/framework/tool:status_util",
        "@com_google_absl//absl/base:core_headers",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/strings",
        "@com_google_protobuf//:protobuf",
    ],
)

cc_library(
    name = "image_frame",
    srcs = ["image_frame.cc"],
    hdrs = ["image_frame.h"],
    deps = [
        ":image_format_cc_proto",
        "//cv-cuda-pipeline/framework:port",
        "//cv-cuda-pipeline/framework/port:aligned_malloc_and_free",
        "//cv-cuda-pipeline/framework/port:core_proto",
        "//cv-cuda-pipeline/framework/port:integral_types",
        "//cv-cuda-pipeline/framework/port:source_location",
        "//cv-cuda-pipeline/framework/tool:type_util",
        "@com_google_absl//absl/base",
        "@com_google_absl//absl/base:core_headers",
        "@com_google_absl//absl/log:absl_check",
        "@com_google_absl//absl/log:absl_log",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/types:span",
    ] + select({
        "//conditions:default": [
        ],
        "//cv-cuda-pipeline/framework:disable_rtti_and_exceptions": [],
    }),
)

cc_library(
    name = "image_frame_opencv",
    srcs = ["image_frame_opencv.cc"],
    hdrs = ["image_frame_opencv.h"],
    deps = [
        ":image_format_cc_proto",
        ":image_frame",
        "//cv-cuda-pipeline/framework/port:opencv_core",
    ],
)

config_setting(
    name = "opencv",
    define_values = {
        "use_opencv": "true",
    },
)

config_setting(
    name = "portable_opencv",
    define_values = {
        "use_portable_opencv": "true",
        "use_opencv": "false",
    },
)

cc_library(
    name = "location",
    srcs = ["location.cc"],
    hdrs = ["location.h"],
    deps = [
        ":location_data_cc_proto",
        "//cv-cuda-pipeline/framework:port",
        "//cv-cuda-pipeline/framework:type_map",
        "//cv-cuda-pipeline/framework/formats/annotation:locus_cc_proto",
        "//cv-cuda-pipeline/framework/formats/annotation:rasterization_cc_proto",
        "//cv-cuda-pipeline/framework/port:integral_types",
        "//cv-cuda-pipeline/framework/port:point",
        "//cv-cuda-pipeline/framework/port:rectangle",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/framework/port:statusor",
        "//cv-cuda-pipeline/framework/tool:status_util",
        "@com_google_absl//absl/base:core_headers",
        "@com_google_absl//absl/log:absl_check",
        "@com_google_absl//absl/log:absl_log",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/strings",
        "@com_google_protobuf//:protobuf",
    ] + select({
        "//conditions:default": [
        ],
    }),
    alwayslink = 1,
)

cc_library(
    name = "location_opencv",
    srcs = ["location_opencv.cc"],
    hdrs = ["location_opencv.h"],
    deps = [
        ":location",
        "//cv-cuda-pipeline/framework/formats/annotation:rasterization_cc_proto",
        "//cv-cuda-pipeline/framework/port:opencv_imgproc",
        "@com_google_absl//absl/log:absl_log",
    ],
    alwayslink = 1,
)

cc_test(
    name = "location_opencv_test",
    srcs = ["location_opencv_test.cc"],
    deps = [
        ":location_opencv",
        "//cv-cuda-pipeline/framework/formats/annotation:rasterization_cc_proto",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:rectangle",
    ],
)

cc_library(
    name = "unique_fd",
    srcs = ["unique_fd.cc"],
    hdrs = ["unique_fd.h"],
    deps = [
        "//cv-cuda-pipeline/framework/port:ret_check",
        "@com_google_absl//absl/base:core_headers",
        "@com_google_absl//absl/log:absl_log",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
    ],
)

cc_test(
    name = "unique_fd_test",
    srcs = ["unique_fd_test.cc"],
    deps = [
        ":unique_fd",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:status_matchers",
        "//cv-cuda-pipeline/util:fd_test_util",
    ],
)

cc_library(
    name = "shared_fd",
    hdrs = ["shared_fd.h"],
    deps = [
        ":unique_fd",
        "@com_google_absl//absl/status:statusor",
    ],
)

cc_test(
    name = "shared_fd_test",
    srcs = ["shared_fd_test.cc"],
    deps = [
        ":shared_fd",
        ":unique_fd",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:status_matchers",
        "//cv-cuda-pipeline/util:fd_test_util",
    ],
)

cc_library(
    name = "video_stream_header",
    hdrs = ["video_stream_header.h"],
    deps = [
        ":image_format_cc_proto",
    ],
)

cc_library(
    name = "yuv_image",
    hdrs = ["yuv_image.h"],
    deps = [
        "@com_google_absl//absl/base:core_headers",
        "@com_google_absl//absl/functional:any_invocable",
        "@com_google_absl//absl/types:span",
        "@libyuv",
    ],
)

cc_test(
    name = "image_frame_opencv_test",
    size = "small",
    srcs = ["image_frame_opencv_test.cc"],
    deps = [
        ":image_format_cc_proto",
        ":image_frame",
        ":image_frame_opencv",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:integral_types",
        "//cv-cuda-pipeline/framework/port:logging",
    ],
)

mediapipe_proto_library(
    name = "rect_proto",
    srcs = ["rect.proto"],
)

mediapipe_register_type(
    base_name = "rect",
    include_headers = ["cv-cuda-pipeline/framework/formats/rect.pb.h"],
    types = [
        "::mediapipe::Rect",
        "::mediapipe::NormalizedRect",
        "::std::vector<::mediapipe::Rect>",
        "::std::vector<::mediapipe::NormalizedRect>",
    ],
    deps = [":rect_cc_proto"],
)

mediapipe_proto_library(
    name = "landmark_proto",
    srcs = ["landmark.proto"],
)

mediapipe_register_type(
    base_name = "landmark",
    include_headers = ["cv-cuda-pipeline/framework/formats/landmark.pb.h"],
    types = [
        "::mediapipe::Landmark",
        "::mediapipe::LandmarkList",
        "::mediapipe::LandmarkListCollection",
        "::mediapipe::NormalizedLandmark",
        "::mediapipe::NormalizedLandmarkList",
        "::mediapipe::NormalizedLandmarkListCollection",
        "::std::vector<::mediapipe::Landmark>",
        "::std::vector<::mediapipe::LandmarkList>",
        "::std::vector<::mediapipe::NormalizedLandmark>",
        "::std::vector<::mediapipe::NormalizedLandmarkList>",
    ],
    deps = [":landmark_cc_proto"],
)

cc_library(
    name = "image",
    srcs = ["image.cc"],
    hdrs = ["image.h"],
    copts = select({
        "//conditions:default": [],
    }),
    deps = [
        ":image_format_cc_proto",
        ":image_frame",
        "//cv-cuda-pipeline/framework:port",
        "//cv-cuda-pipeline/framework:type_map",
        "//cv-cuda-pipeline/framework/port:logging",
        "//cv-cuda-pipeline/gpu:gpu_buffer",
        "//cv-cuda-pipeline/gpu:gpu_buffer_format",
        "@com_google_absl//absl/log:absl_check",
        "@com_google_absl//absl/synchronization",
    ] + select({
        "//conditions:default": [
            "//cv-cuda-pipeline/gpu:gl_texture_buffer",
        ],
    }) + [],
)

cc_library(
    name = "image_multi_pool",
    srcs = ["image_multi_pool.cc"],
    hdrs = ["image_multi_pool.h"],
    deps = [
        ":image",
        ":image_frame_pool",
        "//cv-cuda-pipeline/framework:port",
        "//cv-cuda-pipeline/framework/port:logging",
        "@com_google_absl//absl/log:absl_check",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/synchronization",
    ] + select({
        "//conditions:default": [
            "//cv-cuda-pipeline/gpu:gl_base",
            "//cv-cuda-pipeline/gpu:gl_texture_buffer",
            "//cv-cuda-pipeline/gpu:gl_texture_buffer_pool",
            "//cv-cuda-pipeline/gpu:gpu_buffer",
        ],
        "//cv-cuda-pipeline/gpu:disable_gpu": [],
    }) + [],
)

cc_library(
    name = "image_opencv",
    srcs = [
        "image_opencv.cc",
    ],
    hdrs = [
        "image_opencv.h",
    ],
    deps = [
        ":image",
        ":image_format_cc_proto",
        "//cv-cuda-pipeline/framework/port:logging",
        "//cv-cuda-pipeline/framework/port:opencv_core",
        "//cv-cuda-pipeline/framework/port:statusor",
        "@com_google_absl//absl/log:absl_check",
    ],
)

cc_library(
    name = "image_frame_pool",
    srcs = ["image_frame_pool.cc"],
    hdrs = ["image_frame_pool.h"],
    deps = [
        ":image_frame",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/synchronization",
    ],
)

cc_test(
    name = "image_frame_pool_test",
    size = "small",
    srcs = ["image_frame_pool_test.cc"],
    tags = ["linux"],
    deps = [
        ":image_frame_pool",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:status",
        "@com_google_absl//absl/memory",
    ],
)
cc_library(
    name = "tensor",
    srcs = [
        "tensor.cc",
        "tensor_debugstring.cc",
    ],
    hdrs = [
        "tensor.h",
        "//cv-cuda-pipeline/framework/formats/tensor:internal.h",
    ] + [],
    copts = [],
    deps = [
        "//cv-cuda-pipeline/framework:memory_manager",
        "//cv-cuda-pipeline/framework:port",
        "//cv-cuda-pipeline/framework/deps:no_destructor",
        "//cv-cuda-pipeline/framework/port:aligned_malloc_and_free",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "@com_google_absl//absl/base:core_headers",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/functional:any_invocable",
        "@com_google_absl//absl/log:absl_check",
        "@com_google_absl//absl/log:absl_log",
        "@com_google_absl//absl/log:check",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_absl//absl/synchronization",
    ] + select({
        "//cv-cuda-pipeline/gpu:disable_gpu": [],
        "//conditions:default": [
            "//cv-cuda-pipeline/gpu:gl_base",
            "//cv-cuda-pipeline/gpu:gl_context",
        ],
    }),
)


cc_library(
    name = "frame_buffer",
    srcs = ["frame_buffer.cc"],
    hdrs = ["frame_buffer.h"],
    deps = [
        "//cv-cuda-pipeline/framework/port:integral_types",
        "@com_google_absl//absl/log:absl_check",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
    ],
)

mediapipe_proto_library(
    name = "body_rig_proto",
    srcs = ["body_rig.proto"],
)

mediapipe_register_type(
    base_name = "body_rig",
    include_headers = ["cv-cuda-pipeline/framework/formats/body_rig.pb.h"],
    types = [
        "::mediapipe::Joint",
        "::mediapipe::JointList",
        "::std::vector<::mediapipe::JointList>",
    ],
    deps = [":body_rig_cc_proto"],
)

cc_library(
    name = "tensor_opencv",
    srcs = ["tensor_opencv.cc"],
    hdrs = ["tensor_opencv.h"],
    deps = [
        ":tensor",
        "//cv-cuda-pipeline/framework/port:opencv_core",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "@com_google_absl//absl/log:absl_check",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
    ],
)

cc_library(
    name = "tensor_test_util",
    testonly = True,
    srcs = ["tensor_test_util.cc"],
    hdrs = ["tensor_test_util.h"],
    deps = [
        ":tensor",
        "//cv-cuda-pipeline/framework/port:gtest",
        "@com_google_absl//absl/types:span",
    ],
)

cc_test(
    name = "tensor_opencv_test",
    srcs = ["tensor_opencv_test.cc"],
    deps = [
        ":tensor",
        ":tensor_opencv",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:opencv_core",
        "@com_google_absl//absl/status",
    ],
)

cc_test(
    name = "yuv_image_test",
    srcs = ["yuv_image_test.cc"],
    deps = [
        ":yuv_image",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "@com_google_absl//absl/base:core_headers",
        "@com_google_absl//absl/types:span",
        "@libyuv",
    ],
)

cc_test(
    name = "tensor_debugstring_test",
    srcs = ["tensor_debugstring_test.cc"],
    deps = [
        ":tensor",
        "//cv-cuda-pipeline/framework/port:gtest_main",
    ],
)
