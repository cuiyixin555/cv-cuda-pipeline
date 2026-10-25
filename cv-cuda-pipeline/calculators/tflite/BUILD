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
load("@litert//tflite/core/shims:cc_library_with_tflite.bzl", "cc_library_with_tflite")
load("@rules_cc//cc:cc_library.bzl", "cc_library")
load("@rules_cc//cc:cc_test.bzl", "cc_test")
load("//cv-cuda-pipeline/framework/port:build_config.bzl", "mediapipe_proto_library")

licenses(["notice"])

package(default_visibility = ["//visibility:public"])

mediapipe_proto_library(
    name = "ssd_anchors_calculator_proto",
    srcs = ["ssd_anchors_calculator.proto"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_options_proto",
        "//cv-cuda-pipeline/framework:calculator_proto",
        "//cv-cuda-pipeline/framework/formats/object_detection:anchor_proto",
    ],
)

mediapipe_proto_library(
    name = "tflite_custom_op_resolver_calculator_proto",
    srcs = ["tflite_custom_op_resolver_calculator.proto"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_options_proto",
        "//cv-cuda-pipeline/framework:calculator_proto",
    ],
)

mediapipe_proto_library(
    name = "tflite_inference_calculator_proto",
    srcs = ["tflite_inference_calculator.proto"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_options_proto",
        "//cv-cuda-pipeline/framework:calculator_proto",
    ],
)

mediapipe_proto_library(
    name = "tflite_converter_calculator_proto",
    srcs = ["tflite_converter_calculator.proto"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_options_proto",
        "//cv-cuda-pipeline/framework:calculator_proto",
    ],
)

mediapipe_proto_library(
    name = "tflite_tensors_to_segmentation_calculator_proto",
    srcs = ["tflite_tensors_to_segmentation_calculator.proto"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_options_proto",
        "//cv-cuda-pipeline/framework:calculator_proto",
    ],
)

mediapipe_proto_library(
    name = "tflite_tensors_to_detections_calculator_proto",
    srcs = ["tflite_tensors_to_detections_calculator.proto"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_options_proto",
        "//cv-cuda-pipeline/framework:calculator_proto",
    ],
)

mediapipe_proto_library(
    name = "tflite_tensors_to_classification_calculator_proto",
    srcs = ["tflite_tensors_to_classification_calculator.proto"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_options_proto",
        "//cv-cuda-pipeline/framework:calculator_proto",
    ],
)

mediapipe_proto_library(
    name = "tflite_tensors_to_landmarks_calculator_proto",
    srcs = ["tflite_tensors_to_landmarks_calculator.proto"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_options_proto",
        "//cv-cuda-pipeline/framework:calculator_proto",
    ],
)

cc_library(
    name = "ssd_anchors_calculator",
    srcs = ["ssd_anchors_calculator.cc"],
    deps = [
        ":ssd_anchors_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/formats/object_detection:anchor_cc_proto",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "@com_google_absl//absl/log:absl_check",
        "@com_google_absl//absl/log:absl_log",
    ],
    alwayslink = 1,
)

cc_library(
    name = "tflite_custom_op_resolver_calculator",
    srcs = ["tflite_custom_op_resolver_calculator.cc"],
    deps = [
        ":tflite_custom_op_resolver_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/api2:packet",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/util/tflite:cpu_op_resolver",
        "//cv-cuda-pipeline/util/tflite:op_resolver",
        "@litert//tflite/core/api:op_resolver",
    ],
    alwayslink = 1,
)

filegroup(
    name = "anchor_golden_files",
    srcs = [
        "testdata/anchor_golden_file_0.txt",
        "testdata/anchor_golden_file_1.txt",
        "testdata/anchor_golden_file_2.txt",
    ],
)

cc_test(
    name = "ssd_anchors_calculator_test",
    srcs = ["ssd_anchors_calculator_test.cc"],
    data = [":anchor_golden_files"],
    deps = [
        ":ssd_anchors_calculator",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:calculator_runner",
        "//cv-cuda-pipeline/framework/deps:file_path",
        "//cv-cuda-pipeline/framework/formats/object_detection:anchor_cc_proto",
        "//cv-cuda-pipeline/framework/port:file_helpers",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:integral_types",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
        "@com_google_absl//absl/flags:flag",
    ],
)

selects.config_setting_group(
    name = "gpu_inference_disabled",
    match_any = [
        "//cv-cuda-pipeline/gpu:disable_gpu",
    ],
)

config_setting(
    name = "edge_tpu_usb",
    define_values = {
        "MEDIAPIPE_EDGE_TPU": "usb",
    },
)

config_setting(
    name = "edge_tpu_pci",
    define_values = {
        "MEDIAPIPE_EDGE_TPU": "pci",
    },
)

config_setting(
    name = "edge_tpu_all",
    define_values = {
        "MEDIAPIPE_EDGE_TPU": "all",
    },
)

cc_library(
    name = "tflite_inference_calculator",
    srcs = ["tflite_inference_calculator.cc"],
    copts = select({
        "//conditions:default": [],
    }),
    defines = select({
        "//conditions:default": [],
        ":edge_tpu_usb": ["MEDIAPIPE_EDGE_TPU=usb"],
        ":edge_tpu_pci": ["MEDIAPIPE_EDGE_TPU=pci"],
        ":edge_tpu_all": ["MEDIAPIPE_EDGE_TPU=all"],
    }),
    deps = [
        ":tflite_inference_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/port:logging",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/stream_handler:fixed_size_input_stream_handler",
        "//cv-cuda-pipeline/util/tflite:config",
        "//cv-cuda-pipeline/util/tflite:tflite_model_loader",
        "@com_google_absl//absl/log:absl_check",
        "@com_google_absl//absl/log:absl_log",
        "@com_google_absl//absl/memory",
        "@litert//tflite:framework",
        "@litert//tflite/delegates/xnnpack:xnnpack_delegate",
        "@litert//tflite/kernels:builtin_ops",
    ] + selects.with_or({
        ":gpu_inference_disabled": [],
        "//conditions:default": [
            "//cv-cuda-pipeline/util/tflite:tflite_gpu_runner",
            "//cv-cuda-pipeline/gpu:gl_calculator_helper",
            "//cv-cuda-pipeline/gpu:gpu_buffer",
            "@litert//tflite/delegates/gpu/common:shape",
            "@litert//tflite/delegates/gpu:gl_delegate",
            "@litert//tflite/delegates/gpu/gl:gl_buffer",
            "@litert//tflite/delegates/gpu/gl:gl_program",
            "@litert//tflite/delegates/gpu/gl:gl_shader",
        ],
    }) + select({
        "//conditions:default": [],
    }) + select({
        "//cv-cuda-pipeline:emscripten": [],
        "//conditions:default": [
            "//cv-cuda-pipeline/util:cpu_util",
        ],
    }) + select({
        "//conditions:default": [],
        ":edge_tpu_usb": [
            "@libedgetpu//tflite/public:edgetpu",
            "@libedgetpu//tflite/public:oss_edgetpu_direct_usb",
        ],
        ":edge_tpu_pci": [
            "@libedgetpu//tflite/public:edgetpu",
            "@libedgetpu//tflite/public:oss_edgetpu_direct_pci",
        ],
        ":edge_tpu_all": [
            "@libedgetpu//tflite/public:edgetpu",
            "@libedgetpu//tflite/public:oss_edgetpu_direct_all",
        ],
    }),
    alwayslink = 1,
)

cc_library(
    name = "tflite_converter_calculator",
    srcs = ["tflite_converter_calculator.cc"],
    copts = select({
        "//conditions:default": [],
    }),
    deps = [
        ":tflite_converter_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/formats:image_frame",
        "//cv-cuda-pipeline/framework/formats:matrix",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/stream_handler:fixed_size_input_stream_handler",
        "//cv-cuda-pipeline/util:resource_util",
        "//cv-cuda-pipeline/util/tflite:config",
        "@com_google_absl//absl/log:absl_check",
        "@litert//tflite:framework",
        "@litert//tflite/kernels:builtin_ops",
    ] + selects.with_or({
        ":gpu_inference_disabled": [],
        "//conditions:default": [
            "//cv-cuda-pipeline/gpu:gl_calculator_helper",
            "@litert//tflite/delegates/gpu:gl_delegate",
            "@litert//tflite/delegates/gpu/gl:gl_buffer",
            "@litert//tflite/delegates/gpu/gl:gl_program",
            "@litert//tflite/delegates/gpu/gl:gl_shader",
        ],
    }) + select({
        "//cv-cuda-pipeline/gpu:disable_gpu": [],
        "//conditions:default": [
            "//cv-cuda-pipeline/gpu:gpu_buffer",
        ],
    }),
    alwayslink = 1,
)

# TODO: Re-evaluate which of these libraries we can avoid making
# cc_library_with_tflite and can be changed back to cc_library.
cc_library_with_tflite(
    name = "tflite_model_calculator",
    srcs = ["tflite_model_calculator.cc"],
    tflite_deps = [
        "@litert//tflite:framework_stable",
    ],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:packet",
        "//cv-cuda-pipeline/framework:resources",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings:string_view",
        "@com_google_absl//absl/types:span",
    ],
    alwayslink = 1,
)

cc_library(
    name = "tflite_tensors_to_segmentation_calculator",
    srcs = ["tflite_tensors_to_segmentation_calculator.cc"],
    deps = [
        ":tflite_tensors_to_segmentation_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_context",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/formats:image_frame",
        "//cv-cuda-pipeline/framework/formats:image_frame_opencv",
        "//cv-cuda-pipeline/framework/port:opencv_imgcodecs",
        "//cv-cuda-pipeline/framework/port:opencv_imgproc",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/util:resource_util",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_absl//absl/types:span",
        "@litert//tflite:framework",
    ] + selects.with_or({
        ":gpu_inference_disabled": [],
        "//conditions:default": [
            "//cv-cuda-pipeline/gpu:gl_calculator_helper",
            "//cv-cuda-pipeline/gpu:gl_simple_shaders",
            "//cv-cuda-pipeline/gpu:gpu_buffer",
            "//cv-cuda-pipeline/gpu:shader_util",
            "@litert//tflite/delegates/gpu:gl_delegate",
            "@litert//tflite/delegates/gpu/gl:gl_buffer",
            "@litert//tflite/delegates/gpu/gl:gl_program",
            "@litert//tflite/delegates/gpu/gl:gl_shader",
            "@litert//tflite/delegates/gpu/gl:gl_texture",
        ],
    }),
    alwayslink = 1,
)

cc_test(
    name = "tflite_tensors_to_classification_calculator_test",
    srcs = ["tflite_tensors_to_classification_calculator_test.cc"],
    data = ["testdata/labelmap.txt"],
    deps = [
        ":tflite_tensors_to_classification_calculator",
        ":tflite_tensors_to_classification_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:calculator_runner",
        "//cv-cuda-pipeline/framework/formats:classification_cc_proto",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
        "@com_google_absl//absl/memory",
        "@com_google_googletest//:gtest_main",
        "@litert//tflite:framework",
    ],
)

cc_library(
    name = "tflite_tensors_to_detections_calculator",
    srcs = ["tflite_tensors_to_detections_calculator.cc"],
    copts = select({
        "//conditions:default": [],
    }),
    deps = [
        ":tflite_tensors_to_detections_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/deps:file_path",
        "//cv-cuda-pipeline/framework/formats:detection_cc_proto",
        "//cv-cuda-pipeline/framework/formats:location",
        "//cv-cuda-pipeline/framework/formats/object_detection:anchor_cc_proto",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/util/tflite:config",
        "@com_google_absl//absl/log:absl_check",
        "@com_google_absl//absl/log:absl_log",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_absl//absl/types:span",
        "@litert//tflite:framework",
    ] + selects.with_or({
        ":gpu_inference_disabled": [],
        "//conditions:default": [
            "//cv-cuda-pipeline/gpu:gl_calculator_helper",
            "@litert//tflite/delegates/gpu:gl_delegate",
            "@litert//tflite/delegates/gpu/gl:gl_buffer",
            "@litert//tflite/delegates/gpu/gl:gl_program",
            "@litert//tflite/delegates/gpu/gl:gl_shader",
        ],
    }),
    alwayslink = 1,
)

cc_library(
    name = "tflite_tensors_to_classification_calculator",
    srcs = ["tflite_tensors_to_classification_calculator.cc"],
    deps = [
        ":tflite_tensors_to_classification_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/formats:classification_cc_proto",
        "//cv-cuda-pipeline/framework/formats:location",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/util:resource_util",
        "@com_google_absl//absl/container:node_hash_map",
        "@com_google_absl//absl/log:absl_check",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_absl//absl/types:span",
        "@litert//tflite:framework",
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
    name = "tflite_tensors_to_landmarks_calculator",
    srcs = ["tflite_tensors_to_landmarks_calculator.cc"],
    deps = [
        ":tflite_tensors_to_landmarks_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/formats:landmark_cc_proto",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "@com_google_absl//absl/log:absl_check",
        "@litert//tflite:framework",
    ],
    alwayslink = 1,
)

cc_library(
    name = "tflite_tensors_to_floats_calculator",
    srcs = ["tflite_tensors_to_floats_calculator.cc"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "@litert//tflite:framework",
    ],
    alwayslink = 1,
)

# To run this with native GPU on Linux, use:
# bazel test //cv-cuda-pipeline/calculators/tflite:tflite_inference_calculator_test --copt=-DTFLITE_GPU_EXTRA_GLES_DEPS --copt=-DMESA_EGL_NO_X11_HEADERS --copt=-DEGL_NO_X11 --config=grte_v5 --test_strategy=local
cc_test(
    name = "tflite_inference_calculator_test",
    srcs = [
        "tflite_inference_calculator_test.cc",
        "tflite_inference_calculator_test_common.h",
    ],
    data = ["testdata/add.bin"],
    linkstatic = 1,
    deps = [
        ":tflite_inference_calculator",
        ":tflite_inference_calculator_cc_proto",
        ":tflite_model_calculator",
        "//cv-cuda-pipeline/calculators/util:resource_provider_calculator",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:calculator_runner",
        "//cv-cuda-pipeline/framework/deps:file_path",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:integral_types",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
        "//cv-cuda-pipeline/framework/tool:validate_type",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/types:optional",
        "@litert//tflite:framework",
        "@litert//tflite:type_to_tflitetype",
        "@litert//tflite/kernels:builtin_ops",
        "@litert//tflite/kernels/internal:tensor",
    ],
)

cc_test(
    name = "tflite_converter_calculator_test",
    srcs = ["tflite_converter_calculator_test.cc"],
    deps = [
        ":tflite_converter_calculator",
        ":tflite_converter_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:calculator_runner",
        "//cv-cuda-pipeline/framework/formats:image_format_cc_proto",
        "//cv-cuda-pipeline/framework/formats:image_frame",
        "//cv-cuda-pipeline/framework/formats:image_frame_opencv",
        "//cv-cuda-pipeline/framework/formats:matrix",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:integral_types",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
        "//cv-cuda-pipeline/framework/tool:validate_type",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/strings",
        "@litert//tflite:framework",
    ],
)

cc_test(
    name = "tflite_model_calculator_test",
    srcs = ["tflite_model_calculator_test.cc"],
    data = ["testdata/add.bin"],
    deps = [
        ":tflite_model_calculator",
        "//cv-cuda-pipeline/calculators/core:constant_side_packet_calculator",
        "//cv-cuda-pipeline/calculators/util:local_file_contents_calculator",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:calculator_runner",
        "//cv-cuda-pipeline/framework:resources",
        "//cv-cuda-pipeline/framework/port:file_helpers",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
        "@com_google_absl//absl/types:span",
        "@litert//tflite:framework",
        "@litert//tflite/schema:schema_fbs",
    ],
)

cc_test(
    name = "tflite_custom_op_resolver_calculator_test",
    srcs = ["tflite_custom_op_resolver_calculator_test.cc"],
    data = ["testdata/add.bin"],
    deps = [
        ":tflite_custom_op_resolver_calculator",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:calculator_runner",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
        "@litert//tflite/kernels:builtin_ops",
    ],
)
