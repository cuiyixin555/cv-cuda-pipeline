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
load("//cv-cuda-pipeline/framework:encode_binary_proto.bzl", "encode_binary_proto")
load("//cv-cuda-pipeline/framework:mediapipe_cc_test.bzl", "mediapipe_cc_test")
load("//cv-cuda-pipeline/framework:more_selects.bzl", "more_selects")
load("//cv-cuda-pipeline/framework/port:build_config.bzl", "mediapipe_proto_library")
load(
    "//cv-cuda-pipeline/framework/tool:mediapipe_graph.bzl",
    "mediapipe_binary_graph",
)
# Placeholder: load py_binary

licenses(["notice"])

package(default_visibility = ["//visibility:public"])

exports_files(
    glob(["testdata/image_to_tensor/*"]),
    visibility = [
        "//cv-cuda-pipeline/calculators/image:__subpackages__",
        "//cv-cuda-pipeline/util:__subpackages__",
    ],
)

config_setting(
    name = "disable_gpu",
    define_values = {
        "MEDIAPIPE_DISABLE_GPU": "1",
    },
    visibility = ["//visibility:public"],
)

more_selects.config_setting_negation(
    name = "not_disable_gpu",
    negate = ":disable_gpu",
)

selects.config_setting_group(
    name = "platform_apple_with_gpu",
    match_all = [
        ":not_disable_gpu",
        "//cv-cuda-pipeline:apple",
    ],
)

selects.config_setting_group(
    name = "platform_apple_without_gpu",
    match_all = [
        ":disable_gpu",
        "//cv-cuda-pipeline:apple",
    ],
)

selects.config_setting_group(
    name = "compute_shader_unavailable",
    match_any = [
        "//cv-cuda-pipeline/gpu:disable_gpu",
    ],
)

mediapipe_proto_library(
    name = "audio_to_tensor_calculator_proto",
    srcs = ["audio_to_tensor_calculator.proto"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_options_proto",
        "//cv-cuda-pipeline/framework:calculator_proto",
    ],
)

cc_library(
    name = "audio_to_tensor_calculator",
    srcs = ["audio_to_tensor_calculator.cc"],
    deps = [
        ":audio_to_tensor_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:memory_manager",
        "//cv-cuda-pipeline/framework:memory_manager_service",
        "//cv-cuda-pipeline/framework/api2:node",
        "//cv-cuda-pipeline/framework/api2:packet",
        "//cv-cuda-pipeline/framework/api2:port",
        "//cv-cuda-pipeline/framework/formats:matrix",
        "//cv-cuda-pipeline/framework/formats:tensor",
        "//cv-cuda-pipeline/framework/formats:time_series_header_cc_proto",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/util:time_series_util",
        "@com_google_absl//absl/log:absl_check",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_audio_tools//audio/dsp:resampler_q",
        "@com_google_audio_tools//audio/dsp:window_functions",
        "@litert//tflite/c:common",
        "@pffft",
    ],
    alwayslink = 1,
)

cc_test(
    name = "audio_to_tensor_calculator_test",
    srcs = ["audio_to_tensor_calculator_test.cc"],
    deps = [
        ":audio_to_tensor_calculator",
        ":audio_to_tensor_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:timestamp",
        "//cv-cuda-pipeline/framework/api2:packet",
        "//cv-cuda-pipeline/framework/formats:matrix",
        "//cv-cuda-pipeline/framework/formats:tensor",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
        "@com_google_absl//absl/strings",
        "@com_google_audio_tools//audio/dsp:resampler_q",
        "@litert//tflite/c:common",
    ],
)

mediapipe_proto_library(
    name = "tensors_to_audio_calculator_proto",
    srcs = ["tensors_to_audio_calculator.proto"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_options_proto",
        "//cv-cuda-pipeline/framework:calculator_proto",
    ],
)

cc_library(
    name = "tensors_to_audio_calculator",
    srcs = ["tensors_to_audio_calculator.cc"],
    deps = [
        ":tensors_to_audio_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/api2:node",
        "//cv-cuda-pipeline/framework/formats:matrix",
        "//cv-cuda-pipeline/framework/formats:tensor",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "@com_google_absl//absl/algorithm:container",
        "@com_google_absl//absl/status",
        "@com_google_audio_tools//audio/dsp:window_functions",
        "@pffft",
    ],
    alwayslink = 1,
)

cc_test(
    name = "tensors_to_audio_calculator_test",
    srcs = ["tensors_to_audio_calculator_test.cc"],
    deps = [
        ":audio_to_tensor_calculator",
        ":audio_to_tensor_calculator_cc_proto",
        ":tensors_to_audio_calculator",
        ":tensors_to_audio_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/formats:matrix",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings",
        "@com_google_audio_tools//audio/dsp:window_functions",
    ],
)

mediapipe_proto_library(
    name = "feedback_tensors_calculator_proto",
    srcs = ["feedback_tensors_calculator.proto"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_options_proto",
        "//cv-cuda-pipeline/framework:calculator_proto",
    ],
)

cc_library(
    name = "feedback_tensors_calculator",
    srcs = ["feedback_tensors_calculator.cc"],
    deps = [
        ":feedback_tensors_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:memory_manager",
        "//cv-cuda-pipeline/framework:memory_manager_service",
        "//cv-cuda-pipeline/framework/api2:contract",
        "//cv-cuda-pipeline/framework/api2:node",
        "//cv-cuda-pipeline/framework/formats:tensor",
        "@com_google_absl//absl/status",
    ],
    alwayslink = 1,
)

cc_test(
    name = "feedback_tensors_calculator_test",
    srcs = ["feedback_tensors_calculator_test.cc"],
    deps = [
        ":feedback_tensors_calculator",
        ":feedback_tensors_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:timestamp",
        "//cv-cuda-pipeline/framework/formats:tensor",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
        "@com_google_absl//absl/log:absl_check",
        "@litert//tflite/c:common",
    ],
)

mediapipe_proto_library(
    name = "bert_preprocessor_calculator_proto",
    srcs = ["bert_preprocessor_calculator.proto"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_options_proto",
        "//cv-cuda-pipeline/framework:calculator_proto",
    ],
)

cc_library(
    name = "bert_preprocessor_calculator",
    srcs = ["bert_preprocessor_calculator.cc"],
    deps = [
        ":bert_preprocessor_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:memory_manager",
        "//cv-cuda-pipeline/framework:memory_manager_service",
        "//cv-cuda-pipeline/framework/api2:node",
        "//cv-cuda-pipeline/framework/api2:port",
        "//cv-cuda-pipeline/framework/formats:tensor",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/tasks/cc/core:utils",
        "//cv-cuda-pipeline/tasks/cc/metadata:metadata_extractor",
        "//cv-cuda-pipeline/tasks/cc/text/tokenizers:tokenizer",
        "//cv-cuda-pipeline/tasks/cc/text/tokenizers:tokenizer_utils",
        "//cv-cuda-pipeline/tasks/metadata:metadata_schema_cc",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings",
    ],
    alwayslink = 1,
)

cc_test(
    name = "bert_preprocessor_calculator_test",
    srcs = ["bert_preprocessor_calculator_test.cc"],
    data = [
        "//cv-cuda-pipeline/tasks/testdata/text:bert_text_classifier_models",
    ],
    linkopts = ["-ldl"],
    deps = [
        ":bert_preprocessor_calculator",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/formats:tensor",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
        "//cv-cuda-pipeline/tasks/cc/core:utils",
        "//cv-cuda-pipeline/tasks/cc/metadata:metadata_extractor",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_sentencepiece//:sentencepiece_processor",  # fixdeps: keep
    ],
)

mediapipe_proto_library(
    name = "regex_preprocessor_calculator_proto",
    srcs = ["regex_preprocessor_calculator.proto"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_options_proto",
        "//cv-cuda-pipeline/framework:calculator_proto",
    ],
)

cc_library(
    name = "regex_preprocessor_calculator",
    srcs = ["regex_preprocessor_calculator.cc"],
    deps = [
        ":regex_preprocessor_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:memory_manager",
        "//cv-cuda-pipeline/framework:memory_manager_service",
        "//cv-cuda-pipeline/framework/api2:node",
        "//cv-cuda-pipeline/framework/api2:port",
        "//cv-cuda-pipeline/framework/formats:tensor",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/tasks/cc/metadata:metadata_extractor",
        "//cv-cuda-pipeline/tasks/cc/text/tokenizers:regex_tokenizer",
        "//cv-cuda-pipeline/tasks/cc/text/tokenizers:tokenizer_utils",
        "//cv-cuda-pipeline/tasks/metadata:metadata_schema_cc",
        "@com_google_absl//absl/status",
    ],
    alwayslink = 1,
)

cc_test(
    name = "regex_preprocessor_calculator_test",
    srcs = ["regex_preprocessor_calculator_test.cc"],
    data = ["//cv-cuda-pipeline/tasks/testdata/text:text_classifier_models"],
    linkopts = ["-ldl"],
    deps = [
        ":regex_preprocessor_calculator",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/formats:tensor",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
        "//cv-cuda-pipeline/framework/tool:sink",
        "//cv-cuda-pipeline/tasks/cc/core:utils",
        "//cv-cuda-pipeline/tasks/cc/metadata:metadata_extractor",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_sentencepiece//:sentencepiece_processor",  # fixdeps: keep
    ],
)

mediapipe_proto_library(
    name = "sentencepiece_preprocessor_calculator_proto",
    srcs = ["sentencepiece_preprocessor_calculator.proto"],
    deps = [
        "//cv-cuda-pipeline/tasks/cc/core/proto:external_file_proto",
    ],
)

cc_library(
    name = "sentencepiece_preprocessor_calculator",
    srcs = ["sentencepiece_preprocessor_calculator.cc"],
    hdrs = ["sentencepiece_preprocessor_calculator.h"],
    deps = [
        ":sentencepiece_preprocessor_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:memory_manager",
        "//cv-cuda-pipeline/framework:memory_manager_service",
        "//cv-cuda-pipeline/framework/api3:calculator",
        "//cv-cuda-pipeline/framework/api3:calculator_context",
        "//cv-cuda-pipeline/framework/api3:contract",
        "//cv-cuda-pipeline/framework/api3:node",
        "//cv-cuda-pipeline/framework/formats:tensor",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/tasks/cc/core:external_file_handler",
        "//cv-cuda-pipeline/tasks/cc/metadata:metadata_extractor",
        "//cv-cuda-pipeline/tasks/cc/text/tokenizers:sentencepiece_tokenizer",
        "//cv-cuda-pipeline/tasks/cc/text/tokenizers:tokenizer",
        "//cv-cuda-pipeline/tasks/cc/text/tokenizers:tokenizer_utils",
        "//cv-cuda-pipeline/tasks/metadata:metadata_schema_cc",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings:string_view",
    ],
    alwayslink = 1,
)

cc_library(
    name = "text_to_tensor_calculator",
    srcs = ["text_to_tensor_calculator.cc"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_context",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:memory_manager",
        "//cv-cuda-pipeline/framework:memory_manager_service",
        "//cv-cuda-pipeline/framework/api2:node",
        "//cv-cuda-pipeline/framework/api2:port",
        "//cv-cuda-pipeline/framework/formats:tensor",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings",
    ],
    alwayslink = 1,
)

cc_test(
    name = "text_to_tensor_calculator_test",
    srcs = ["text_to_tensor_calculator_test.cc"],
    deps = [
        ":text_to_tensor_calculator",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:calculator_graph",
        "//cv-cuda-pipeline/framework:packet",
        "//cv-cuda-pipeline/framework/formats:tensor",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
        "//cv-cuda-pipeline/framework/tool:options_map",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
    ],
)

cc_library(
    name = "universal_sentence_encoder_preprocessor_calculator",
    srcs = ["universal_sentence_encoder_preprocessor_calculator.cc"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_context",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:memory_manager",
        "//cv-cuda-pipeline/framework:memory_manager_service",
        "//cv-cuda-pipeline/framework/api2:node",
        "//cv-cuda-pipeline/framework/api2:port",
        "//cv-cuda-pipeline/framework/formats:tensor",
        "//cv-cuda-pipeline/tasks/cc/core:utils",
        "//cv-cuda-pipeline/tasks/cc/metadata:metadata_extractor",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings",
    ],
    alwayslink = 1,
)

cc_test(
    name = "universal_sentence_encoder_preprocessor_calculator_test",
    srcs = ["universal_sentence_encoder_preprocessor_calculator_test.cc"],
    data = ["//cv-cuda-pipeline/tasks/testdata/text:universal_sentence_encoder_qa"],
    deps = [
        ":universal_sentence_encoder_preprocessor_calculator",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:packet",
        "//cv-cuda-pipeline/framework/formats:tensor",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/framework/tool:options_map",
        "//cv-cuda-pipeline/tasks/cc/core:utils",
        "//cv-cuda-pipeline/tasks/cc/metadata:metadata_extractor",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
    ],
)

mediapipe_proto_library(
    name = "inference_calculator_proto",
    srcs = ["inference_calculator.proto"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_options_proto",
        "//cv-cuda-pipeline/framework:calculator_proto",
    ],
)

config_setting(
    name = "force_cpu_inference",
    values = {
        "define": "MEDIAPIPE_FORCE_CPU_INFERENCE=true",
    },
    visibility = ["//visibility:public"],
)

# This target defines the "InferenceCalculator" component, which looks for the available concrete
# implementations linked into the current binary and picks the one to use.
# You can depend on :inference_calculator instead if you want to automatically include a default
# set of implementations tailored for the current build configuration.
# If you want to have precise control of which implementations to include (e.g. for strict binary
# size concerns), depend on those implementations directly, and do not depend on
# :inference_calculator.
# In all cases, use "InferenceCalculator" in your graphs.
cc_library_with_tflite(
    name = "inference_calculator_interface",
    srcs = ["inference_calculator.cc"],
    hdrs = ["inference_calculator.h"],
    local_defines = select({
        ":force_cpu_inference": ["MEDIAPIPE_FORCE_CPU_INFERENCE=1"],
        "//conditions:default": ["MEDIAPIPE_FORCE_CPU_INFERENCE=0"],
    }),
    tflite_deps = [
        ":inference_runner",
        ":inference_io_mapper",
        "//cv-cuda-pipeline/util/tflite:tflite_model_loader",
        "@litert//tflite:framework_stable",
        "@litert//tflite/kernels:builtin_ops",
    ],
    deps = [
        ":inference_calculator_cc_proto",
        ":inference_calculator_options_lib",
        ":tensor_span",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:port",
        "//cv-cuda-pipeline/framework:resources",
        "//cv-cuda-pipeline/framework/api2:node",
        "//cv-cuda-pipeline/framework/api2:packet",
        "//cv-cuda-pipeline/framework/api2:port",
        "//cv-cuda-pipeline/framework/api3:contract",
        "//cv-cuda-pipeline/framework/api3:node",
        "//cv-cuda-pipeline/framework/formats:tensor",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/framework/stream_handler:fixed_size_input_stream_handler",
        "//cv-cuda-pipeline/framework/tool:subgraph_expansion",
        "//cv-cuda-pipeline/util:cpu_util",
        "@com_google_absl//absl/log:absl_check",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_absl//absl/time",
        "@litert//tflite/core/api:op_resolver",
    ],
    alwayslink = 1,
)

# Helper utility to more easily iterate over a collection of Tensor references
cc_library(
    name = "tensor_span",
    srcs = ["tensor_span.cc"],
    hdrs = ["tensor_span.h"],
    deps = ["//cv-cuda-pipeline/framework/formats:tensor"],
)

cc_library_with_tflite(
    name = "inference_io_mapper",
    srcs = ["inference_io_mapper.cc"],
    hdrs = ["inference_io_mapper.h"],
    tflite_deps = [
        "@litert//tflite/c:c_api_types",
        "@litert//tflite:framework_stable",
        "@litert//tflite/kernels:builtin_ops",
        "//cv-cuda-pipeline/util/tflite:tflite_signature_reader",
    ],
    deps = [
        ":inference_calculator_cc_proto",
        ":tensor_span",
        "//cv-cuda-pipeline/framework/formats:tensor",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/log:absl_log",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@litert//tflite/core/api:op_resolver",
    ],
)

# TODO b/336756170 - convert test to use cc_test_with_tflite
cc_test(
    name = "inference_io_mapper_test",
    srcs = ["inference_io_mapper_test.cc"],
    data = [
        ":testdata/3in3out_model_swaps_input_2_and_0.tflite",
        ":testdata/test_two_signature_keys_model.tflite",
    ],
    deps = [
        ":inference_calculator",
        ":inference_calculator_cc_proto",
        ":inference_calculator_cpu",
        ":inference_io_mapper",
        ":tensor_span",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:packet",
        "//cv-cuda-pipeline/framework:resources",
        "//cv-cuda-pipeline/framework/api2:packet",
        "//cv-cuda-pipeline/framework/formats:tensor",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
        "//cv-cuda-pipeline/framework/tool:sink",
        "//cv-cuda-pipeline/util/tflite:tflite_model_loader",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings",
        "@litert//tflite:framework_stable",
        "@litert//tflite/kernels:builtin_ops",
    ],
)

cc_library_with_tflite(
    name = "inference_feedback_manager",
    srcs = ["inference_feedback_manager.cc"],
    hdrs = ["inference_feedback_manager.h"],
    tflite_deps = [
        ":inference_io_mapper",
        "//cv-cuda-pipeline/util/tflite:utils",
        "//cv-cuda-pipeline/util/tflite:tflite_signature_reader",
        "@litert//tflite:framework_stable",
        "@litert//tflite:namespace",
        "@litert//tflite/c:common",
        "@litert//tflite/kernels:builtin_ops",
    ],
    deps = [
        ":inference_calculator_cc_proto",
        ":inference_calculator_utils",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/log:absl_log",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
    ],
)

cc_test(
    name = "inference_feedback_manager_test",
    srcs = ["inference_feedback_manager_test.cc"],
    data = [
        ":testdata/feedback_tensor_test_model.tflite",
        ":testdata/feedback_tensor_with_state_copy_model.tflite",
        ":testdata/feedback_tensor_without_passthrough_model.tflite",
    ],
    deps = [
        ":inference_calculator",
        ":inference_calculator_cc_proto",
        ":inference_calculator_cpu",
        ":inference_calculator_interface",
        ":inference_feedback_manager",
        ":inference_io_mapper",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:resources",
        "//cv-cuda-pipeline/framework/api2:packet",
        "//cv-cuda-pipeline/framework/formats:tensor",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
        "//cv-cuda-pipeline/framework/port:status_matchers",
        "//cv-cuda-pipeline/framework/tool:sink",
        "//cv-cuda-pipeline/util/tflite:tflite_model_loader",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings",
        "@litert//tflite:framework_stable",
        "@litert//tflite:util",
        "@litert//tflite/core:framework",
        "@litert//tflite/core/api:op_resolver",
        "@litert//tflite/delegates/xnnpack:xnnpack_delegate_hdrs_only",
        "@litert//tflite/kernels:builtin_ops",
    ],
)

cc_library(
    name = "inference_calculator_gl",
    srcs = ["inference_calculator_gl.cc"],
    tags = ["nomac"],  # config problem with cpuinfo via TF
    deps = [
        ":inference_calculator_cc_proto",
        ":inference_calculator_interface",
        ":inference_io_mapper",
        ":tensor_span",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:mediapipe_profiling",
        "//cv-cuda-pipeline/framework/api2:node",
        "//cv-cuda-pipeline/framework/api2:packet",
        "//cv-cuda-pipeline/framework/formats:tensor",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/gpu:gl_base_hdr",
        "//cv-cuda-pipeline/gpu:gl_calculator_helper",
        "//cv-cuda-pipeline/gpu:gl_context",
        "//cv-cuda-pipeline/util/tflite:tflite_model_loader",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_absl//absl/time",
        "@litert//tflite:framework_stable",
        "@litert//tflite/core:framework",
        "@litert//tflite/delegates/gpu:gl_delegate",
    ],
    alwayslink = 1,
)

cc_library(
    name = "inference_calculator_gl_advanced",
    srcs = ["inference_calculator_gl_advanced.cc"],
    tags = ["nomac"],
    deps = [
        ":inference_calculator_interface",
        ":inference_io_mapper",
        ":inference_on_disk_cache_helper",
        ":inference_runner",
        ":tensor_span",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:mediapipe_profiling",
        "//cv-cuda-pipeline/framework/api2:packet",
        "//cv-cuda-pipeline/framework/formats:tensor",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/gpu:gl_calculator_helper",
        "//cv-cuda-pipeline/gpu:gl_context",
        "//cv-cuda-pipeline/util/tflite:tflite_gpu_runner",
        "//cv-cuda-pipeline/util/tflite:tflite_model_loader",
        "@com_google_absl//absl/log:absl_log",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:string_view",
        "@com_google_absl//absl/time",
        "@litert//tflite:framework_stable",
    ],
    alwayslink = 1,
)

# The LiteRT v2.0.0 GPU accelerator is not open sourced.
# Prebuilts are published per platform at
# litert/prebuilt/<platform>/ (e.g. ios_arm64/libLiteRtMetalAccelerator.dylib)
cc_library(
    name = "inference_calculator_litert",
    srcs = ["inference_calculator_litert.cc"],
    copts = select({
        "//cv-cuda-pipeline:apple": [
            "-x objective-c++",
            "-fobjc-arc",
        ],
        "//conditions:default": [],
    }),
    deps = [
        ":inference_calculator_interface",
        ":inference_interpreter_delegate_runner",
        ":inference_runner",
        ":inference_runner_litert",
        ":tensor_span",
        "//cv-cuda-pipeline/calculators/tensor/litert:litert_service",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:memory_manager",
        "//cv-cuda-pipeline/framework:memory_manager_service",
        "//cv-cuda-pipeline/framework:port",
        "//cv-cuda-pipeline/framework/formats:tensor",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/util/tflite:tflite_model_loader",
        "@com_google_absl//absl/log:absl_log",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/time",
        "@com_google_absl//absl/types:optional",
        "@litert//litert/cc:litert_options",
    ] + select({
        "//cv-cuda-pipeline:apple": [
            "//cv-cuda-pipeline/gpu:MPPMetalHelper",
        ],
        "//conditions:default": [],
    }) + select({
        "//cv-cuda-pipeline/gpu:disable_gpu": [],
        "//conditions:default": [
            "//cv-cuda-pipeline/gpu:gl_calculator_helper",
        ],
    }),
    alwayslink = 1,
)

cc_library(
    name = "inference_calculator_metal",
    srcs = ["inference_calculator_metal.cc"],
    copts = [
        "-x objective-c++",
        "-fobjc-arc",  # enable reference-counting
    ],
    deps = [
        ":inference_calculator_interface",
        ":inference_io_mapper",
        ":tensor_span",
        "//cv-cuda-pipeline/framework/formats:tensor",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/gpu:MPPMetalHelper",
        "//cv-cuda-pipeline/gpu:MPPMetalUtil",
        "//cv-cuda-pipeline/gpu:gpu_buffer",
        "//cv-cuda-pipeline/util/tflite:config",
        "@com_google_absl//absl/log:absl_log",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/strings:str_format",
        "@litert//tflite/delegates/gpu:metal_delegate",
        "@litert//tflite/delegates/gpu:metal_delegate_internal",
        "@litert//tflite/delegates/gpu/common:shape",
        "@litert//tflite/delegates/gpu/metal:buffer_convert",
    ] + select({
        "//cv-cuda-pipeline:apple": [
            "//third_party/apple_frameworks:CoreVideo",
            "//third_party/apple_frameworks:MetalKit",
        ],
        "//conditions:default": [],
    }),
    alwayslink = 1,
)

cc_library_with_tflite(
    name = "inference_runner",
    hdrs = ["inference_runner.h"],
    tflite_deps = [
        ":inference_io_mapper",
    ],
    deps = [
        ":tensor_span",
        "//cv-cuda-pipeline/framework:calculator_context",
        "//cv-cuda-pipeline/framework/formats:tensor",
        "@com_google_absl//absl/status:statusor",
    ],
)

cc_library(
    name = "inference_feedback_manager_litert",
    srcs = ["inference_feedback_manager_litert.cc"],
    hdrs = ["inference_feedback_manager_litert.h"],
    deps = [
        ":inference_calculator_cc_proto",
        ":inference_calculator_utils",
        ":inference_io_mapper",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/util/tflite:tflite_signature_reader",
        "//cv-cuda-pipeline/util/tflite:utils",
        "@com_google_absl//absl/algorithm:container",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/log:absl_log",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_absl//absl/types:span",
        "@litert//litert/c:litert_model_types",
        "@litert//litert/cc:litert_api_types",
        "@litert//litert/cc:litert_common",
        "@litert//litert/cc:litert_compiled_model",
        "@litert//litert/cc:litert_expected",
        "@litert//litert/cc:litert_macros",
        "@litert//litert/cc:litert_model_types",
        "@litert//litert/cc:litert_ranked_tensor_type",
        "@litert//litert/cc:litert_tensor_buffer",
        "@litert//tflite:framework_stable",
        "@litert//tflite:namespace",
        "@litert//tflite/c:common",
        "@litert//tflite/kernels:builtin_ops",
    ],
)

cc_library(
    name = "inference_runner_litert",
    srcs = ["inference_runner_litert.cc"],
    hdrs = ["inference_runner_litert.h"],
    copts = select({
        "//cv-cuda-pipeline:apple": [
            "-x objective-c++",
            "-fobjc-arc",
        ],
        "//conditions:default": [],
    }),
    deps = [
        ":inference_calculator_cc_proto",
        ":inference_calculator_interface",
        ":inference_calculator_utils",
        ":inference_feedback_manager_litert",
        ":inference_io_mapper",
        ":inference_runner",
        ":tensor_span",
        "//cv-cuda-pipeline/calculators/tensor/litert:litert_service",
        "//cv-cuda-pipeline/calculators/tensor/litert:litert_utils",
        "//cv-cuda-pipeline/framework:calculator_context",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:memory_manager",
        "//cv-cuda-pipeline/framework:port",
        "//cv-cuda-pipeline/framework/api2:packet",
        "//cv-cuda-pipeline/framework/formats:tensor",
        "//cv-cuda-pipeline/framework/port",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/util/tflite:tflite_model_loader",
        "//cv-cuda-pipeline/util/tflite:tflite_signature_reader",
        "@com_google_absl//absl/algorithm:container",
        "@com_google_absl//absl/base:core_headers",
        "@com_google_absl//absl/base:nullability",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/log:absl_log",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_absl//absl/synchronization",
        "@com_google_absl//absl/types:optional",
        "@com_google_absl//absl/types:span",
        "@litert//litert/cc:litert_any",
        "@litert//litert/cc:litert_api_types",
        "@litert//litert/cc:litert_buffer_ref",
        "@litert//litert/cc:litert_common",
        "@litert//litert/cc:litert_compiled_model",
        "@litert//litert/cc:litert_environment",
        "@litert//litert/cc:litert_environment_options",
        "@litert//litert/cc:litert_event",
        "@litert//litert/cc:litert_layout",
        "@litert//litert/cc:litert_macros",
        "@litert//litert/cc:litert_model_types",
        "@litert//litert/cc:litert_opaque_options",
        "@litert//litert/cc:litert_options",
        "@litert//litert/cc:litert_ranked_tensor_type",
        "@litert//litert/cc:litert_tensor_buffer",
        "@litert//litert/cc:litert_tensor_buffer_requirements",
        "@litert//litert/cc:litert_tensor_buffer_types",
        "@litert//litert/cc/options:litert_cpu_options",
        "@litert//litert/cc/options:litert_google_tensor_options",
        "@litert//litert/cc/options:litert_gpu_options",
        "@litert//litert/cc/options:litert_qualcomm_options",
    ] + select({
        "//cv-cuda-pipeline:apple": [
            "//cv-cuda-pipeline/gpu:MPPMetalHelper",
            "@litert//tflite/delegates/gpu/common:shape",
            "@litert//tflite/delegates/gpu/metal:buffer_convert",
            "@litert//tflite/delegates/gpu/metal:metal_device",
        ],
        "//cv-cuda-pipeline/gpu/webgpu:use_webgpu": [
            "//cv-cuda-pipeline/gpu/webgpu:webgpu_service",
        ],
        "//conditions:default": [],
    }) + select({
        "//cv-cuda-pipeline/gpu:disable_gpu": [],
        "//conditions:default": [
            "//cv-cuda-pipeline/gpu:gl_context",
        ],
    }),
)

cc_library_with_tflite(
    name = "tflite_delegate_ptr",
    hdrs = ["tflite_delegate_ptr.h"],
    tflite_deps = [
        "@litert//tflite/c:c_api_types",
    ],
)

cc_library_with_tflite(
    name = "inference_interpreter_delegate_runner",
    srcs = ["inference_interpreter_delegate_runner.cc"],
    hdrs = ["inference_interpreter_delegate_runner.h"],
    tflite_deps = [
        ":inference_runner",
        ":tflite_delegate_ptr",
        ":inference_io_mapper",
        ":inference_feedback_manager",
        "//cv-cuda-pipeline/util/tflite:tflite_model_loader",
        "@litert//tflite:framework_stable",
        "@litert//tflite/c:c_api_types",
        "@litert//tflite/c:common",
    ],
    deps = [
        ":inference_calculator_utils",
        ":tensor_span",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:mediapipe_profiling",
        "//cv-cuda-pipeline/framework/api2:packet",
        "//cv-cuda-pipeline/framework/formats:tensor",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@litert//tflite:string_util",
        "@litert//tflite:util",
        "@litert//tflite/core/api:op_resolver",
    ],
)

cc_test(
    name = "inference_interpreter_delegate_runner_test",
    srcs = ["inference_interpreter_delegate_runner_test.cc"],
    data = [
        ":testdata/1x3_square_float32.tflite",
        ":testdata/1x3_square_int32.tflite",
        ":testdata/3in3out_model_swaps_input_2_and_0.tflite",
    ],
    deps = [
        ":inference_interpreter_delegate_runner",
        ":tensor_span",
        ":tflite_delegate_ptr",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:calculator_state",
        "//cv-cuda-pipeline/framework:resources",
        "//cv-cuda-pipeline/framework/api2:builder",
        "//cv-cuda-pipeline/framework/api2:node",
        "//cv-cuda-pipeline/framework/api2:packet",
        "//cv-cuda-pipeline/framework/api2:port",
        "//cv-cuda-pipeline/framework/formats:tensor",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/util/tflite:tflite_model_loader",
        "@com_google_absl//absl/functional:any_invocable",
        "@com_google_absl//absl/status",
        "@litert//tflite:framework_stable",
        "@litert//tflite:util",
        "@litert//tflite/core/api:op_resolver",
        "@litert//tflite/delegates/xnnpack:xnnpack_delegate_hdrs_only",
        "@litert//tflite/kernels:builtin_ops",
    ],
)

cc_library(
    name = "inference_calculator_cpu",
    srcs = [
        "inference_calculator_cpu.cc",
    ],
    local_defines = select({
        ":force_cpu_inference": ["MEDIAPIPE_FORCE_CPU_INFERENCE=1"],
        "//conditions:default": ["MEDIAPIPE_FORCE_CPU_INFERENCE=0"],
    }),
    deps = [
        ":inference_calculator_interface",
        ":inference_calculator_utils",
        ":inference_interpreter_delegate_runner",
        ":inference_runner",
        ":tensor_span",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/formats:tensor",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/time",
        "@litert//tflite:framework_stable",
        "@litert//tflite/c:c_api_types",
        "@litert//tflite/delegates/xnnpack:xnnpack_delegate",
    ] + select({
        "//conditions:default": [],
    }),
    alwayslink = 1,
)

cc_library(
    name = "inference_calculator_utils",
    srcs = ["inference_calculator_utils.cc"],
    hdrs = ["inference_calculator_utils.h"],
    deps = [
        ":inference_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:memory_manager",
        "//cv-cuda-pipeline/framework:port",
        "//cv-cuda-pipeline/framework/formats:tensor",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/log:absl_check",
        "@com_google_absl//absl/log:absl_log",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_absl//absl/types:span",
        "@litert//tflite:framework_stable",
        "@litert//tflite:string_util",
        "@litert//tflite:type_to_tflitetype",
        "@litert//tflite:util",
        "@litert//tflite/c:common",
        "@litert//tflite/core:framework",
    ] + select({
        "//cv-cuda-pipeline:emscripten": [],
        "//conditions:default": [
            "//cv-cuda-pipeline/util:cpu_util",
        ],
    }),
    alwayslink = 1,
)

cc_test(
    name = "inference_calculator_utils_test",
    srcs = ["inference_calculator_utils_test.cc"],
    deps = [
        ":inference_calculator_cc_proto",
        ":inference_calculator_utils",
        "//cv-cuda-pipeline/framework/formats:tensor",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/log:absl_check",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/types:span",
        "@litert//tflite:framework_stable",
        "@litert//tflite:string_util",
        "@litert//tflite:util",
        "@litert//tflite/c:common",
        "@litert//tflite/kernels:cast_test_common",
        "@litert//tflite/schema:schema_fbs",
    ],
)

cc_library(
    name = "inference_on_disk_cache_helper",
    srcs = ["inference_on_disk_cache_helper.cc"],
    hdrs = ["inference_on_disk_cache_helper.h"],
    tags = ["nomac"],
    deps = [
        ":inference_calculator_cc_proto",
        ":inference_calculator_interface",
        ":inference_io_mapper",
        "//cv-cuda-pipeline/framework/deps:file_path",
        "//cv-cuda-pipeline/framework/port:file_helpers",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/util/tflite:tflite_gpu_runner",
        "//cv-cuda-pipeline/util/tflite:tflite_model_loader",
        "@com_google_absl//absl/log:absl_log",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:string_view",
    ],
)

cc_library(
    name = "inference_calculator_xnnpack",
    srcs = [
        "inference_calculator_xnnpack.cc",
    ],
    deps = [
        ":inference_calculator_interface",
        ":inference_calculator_utils",
        ":inference_interpreter_delegate_runner",
        ":inference_runner",
        ":tensor_span",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/formats:tensor",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/time",
        "@litert//tflite/delegates/xnnpack:xnnpack_delegate",
    ],
    alwayslink = 1,
)

cc_library(
    name = "inference_calculator_gl_if_compute_shader_available",
    deps = selects.with_or({
        ":compute_shader_unavailable": [],
        "//conditions:default": [
            ":inference_calculator_gl",
            ":inference_calculator_gl_advanced",
        ],
    }),
)

# This target provides the InferenceCalculator and a default set of implementations tailored for the
# current build platforms. More implementations can be added as separate dependencies to a client;
# for clients that want a narrower set of implementations than the default should see the comment on
# inference_calculator_interface.
cc_library(
    name = "inference_calculator",
    deps = [
        ":inference_calculator_cpu",
        ":inference_calculator_interface",
        ":inference_calculator_xnnpack",
    ] + select({
        "//conditions:default": [":inference_calculator_gl_if_compute_shader_available"],
        ":platform_apple_with_gpu": [":inference_calculator_metal"],
    }),
    alwayslink = 1,
)

mediapipe_proto_library(
    name = "tensor_converter_calculator_proto",
    srcs = ["tensor_converter_calculator.proto"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_options_proto",
        "//cv-cuda-pipeline/framework:calculator_proto",
        "//cv-cuda-pipeline/gpu:gpu_origin_proto",
    ],
)

cc_library(
    name = "tensor_converter_calculator",
    srcs = ["tensor_converter_calculator.cc"],
    copts = select({
        "//cv-cuda-pipeline:apple": [
            "-x objective-c++",
            "-fobjc-arc",  # enable reference-counting
        ],
        "//conditions:default": [],
    }),
    features = ["-layering_check"],  # allow depending on tensor_converter_calculator_gpu_deps
    deps = [
        ":tensor_converter_calculator_cc_proto",
        ":tensor_converter_cpu",
        ":tensor_converter_gpu",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:memory_manager",
        "//cv-cuda-pipeline/framework:memory_manager_service",
        "//cv-cuda-pipeline/framework:port",
        "//cv-cuda-pipeline/framework/formats:image_frame",
        "//cv-cuda-pipeline/framework/formats:matrix",
        "//cv-cuda-pipeline/framework/formats:tensor",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/framework/port:statusor",
        "//cv-cuda-pipeline/gpu:gpu_origin_cc_proto",
        "//cv-cuda-pipeline/gpu:gpu_origin_utils",
        "//cv-cuda-pipeline/util:resource_util",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/log:absl_check",
        "@com_google_absl//absl/log:absl_log",
        "@com_google_absl//absl/log:check",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
    ] + select({
        "//cv-cuda-pipeline/gpu:disable_gpu": [],
        "//conditions:default": [
            ":tensor_converter_calculator_gpu_deps",
            "//cv-cuda-pipeline/gpu:gl_base",
            "//cv-cuda-pipeline/gpu:gl_calculator_helper",
            "//cv-cuda-pipeline/gpu:gpu_buffer",
        ],
    }) + select({
        "//cv-cuda-pipeline:apple": [
            ":tensor_converter_metal",
            "//third_party/apple_frameworks:MetalKit",
        ],
        "//conditions:default": [],
    }),
    alwayslink = 1,
)

cc_library(
    name = "tensor_converter_cpu",
    srcs = ["tensor_converter_cpu.cc"],
    hdrs = ["tensor_converter_cpu.h"],
    deps = [
        "//cv-cuda-pipeline/framework:memory_manager",
        "//cv-cuda-pipeline/framework/formats:image_frame",
        "//cv-cuda-pipeline/framework/formats:matrix",
        "//cv-cuda-pipeline/framework/formats:tensor",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
    ],
)

cc_test(
    name = "tensor_converter_cpu_test",
    srcs = ["tensor_converter_cpu_test.cc"],
    deps = [
        ":tensor_converter_cpu",
        "//cv-cuda-pipeline/framework:memory_manager",
        "//cv-cuda-pipeline/framework/formats:matrix",
        "//cv-cuda-pipeline/framework/formats:tensor",
        "//cv-cuda-pipeline/framework/port:gtest",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:status_matchers",
        "//cv-cuda-pipeline/util:image_test_utils",
    ],
)

cc_library(
    name = "tensor_converter_gpu",
    hdrs = ["tensor_converter_gpu.h"],
    deps = [
        "//cv-cuda-pipeline/framework:memory_manager",
        "//cv-cuda-pipeline/framework/formats:tensor",
        "//cv-cuda-pipeline/gpu:gpu_buffer",
        "@com_google_absl//absl/status",
    ],
)

cc_library(
    name = "tensor_converter_gl30",
    srcs = ["tensor_converter_gl30.cc"],
    hdrs = ["tensor_converter_gl30.h"],
    features = ["-layering_check"],  # allow depending on tensor_converter_calculator_gpu_deps
    deps = [
        ":tensor_converter_gpu",
        "//cv-cuda-pipeline/framework:port",
        "//cv-cuda-pipeline/framework/port:status",
    ] + select({
        "//cv-cuda-pipeline:apple": [],
        "//conditions:default": [
            "//cv-cuda-pipeline/framework:calculator_framework",
            "//cv-cuda-pipeline/framework:memory_manager",
            "//cv-cuda-pipeline/framework/formats:image_frame",
            "//cv-cuda-pipeline/framework/formats:matrix",
            "//cv-cuda-pipeline/framework/formats:tensor",
            "//cv-cuda-pipeline/framework/port:ret_check",
            "//cv-cuda-pipeline/gpu:gl_base_hdr",
            "//cv-cuda-pipeline/gpu:gl_calculator_helper",
            "//cv-cuda-pipeline/gpu:gl_simple_shaders",
            "//cv-cuda-pipeline/gpu:gl_texture_view",
            "//cv-cuda-pipeline/gpu:gpu_buffer",
            "//cv-cuda-pipeline/gpu:gpu_buffer_format",
            "//cv-cuda-pipeline/gpu:shader_util",
            "@com_google_absl//absl/log:absl_log",
            "@com_google_absl//absl/status",
            "@com_google_absl//absl/strings",
        ],
    }),
)

cc_library(
    name = "tensor_converter_metal",
    srcs = ["tensor_converter_metal.cc"],
    hdrs = ["tensor_converter_metal.h"],
    copts = select({
        "//cv-cuda-pipeline:apple": [
            "-x objective-c++",
            "-fobjc-arc",  # enable reference-counting
        ],
        "//conditions:default": [],
    }),
    features = ["-layering_check"],  # allow depending on tensor_converter_calculator_gpu_deps
    linkopts = select({
        "//cv-cuda-pipeline:apple": [
            "-framework CoreVideo",
            "-framework MetalKit",
        ],
        "//conditions:default": [],
    }),
    deps = [
        "//cv-cuda-pipeline/framework:port",
    ] + select({
        "//cv-cuda-pipeline:apple": [
            ":tensor_converter_calculator_gpu_deps",
            ":tensor_converter_gpu",
            "//cv-cuda-pipeline/framework/formats:tensor",
            "//cv-cuda-pipeline/framework/port:ret_check",
            "//cv-cuda-pipeline/framework/port:status",
            "//third_party/apple_frameworks:MetalKit",
        ],
        "//conditions:default": [],
    }),
)

cc_library(
    name = "tensor_converter_gl31",
    srcs = ["tensor_converter_gl31.cc"],
    hdrs = ["tensor_converter_gl31.h"],
    features = ["-layering_check"],  # allow depending on tensor_converter_calculator_gpu_deps
    deps = [
        ":tensor_converter_gpu",
        "//cv-cuda-pipeline/framework:port",
        "//cv-cuda-pipeline/framework/port:status",
    ] + select({
        "//cv-cuda-pipeline:apple": [],
        "//conditions:default": [
            "//cv-cuda-pipeline/framework:calculator_context",
            "//cv-cuda-pipeline/framework:memory_manager",
            "//cv-cuda-pipeline/framework/formats:tensor",
            "//cv-cuda-pipeline/framework/port:ret_check",
            "//cv-cuda-pipeline/gpu:gl_calculator_helper",
            "//cv-cuda-pipeline/gpu:gl_simple_shaders",
            "//cv-cuda-pipeline/gpu:gpu_buffer",
            "//cv-cuda-pipeline/gpu:shader_util",
            "@com_google_absl//absl/status",
            "@com_google_absl//absl/strings",
        ],
    }),
)

cc_library(
    name = "tensor_converter_calculator_gpu_deps",
    visibility = ["//visibility:private"],
    deps = select({
        ":platform_apple_with_gpu": [
            "//cv-cuda-pipeline/gpu:MPPMetalHelper",
            "//cv-cuda-pipeline/gpu:MPPMetalUtil",
        ],
        # This setting is needed to allow bazel to build all targets on Mac with GPU disabled
        ":platform_apple_without_gpu": [],
        "//conditions:default": [
            ":tensor_converter_gl30",
            ":tensor_converter_gl31",
            "//cv-cuda-pipeline/gpu:gl_calculator_helper",
            "//cv-cuda-pipeline/gpu:gl_simple_shaders",
            "//cv-cuda-pipeline/gpu:gpu_buffer",
            "//cv-cuda-pipeline/gpu:shader_util",
        ],
    }),
)

cc_test(
    name = "tensor_converter_calculator_test",
    srcs = ["tensor_converter_calculator_test.cc"],
    tags = [
        "noasan",  # TODO: Remove noasan tag when SwiftShader supports sanitizers
    ],
    deps = [
        ":tensor_converter_calculator",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:calculator_runner",
        "//cv-cuda-pipeline/framework:memory_manager",
        "//cv-cuda-pipeline/framework:memory_manager_service",
        "//cv-cuda-pipeline/framework/formats:image_format_cc_proto",
        "//cv-cuda-pipeline/framework/formats:image_frame",
        "//cv-cuda-pipeline/framework/formats:image_frame_opencv",
        "//cv-cuda-pipeline/framework/formats:matrix",
        "//cv-cuda-pipeline/framework/formats:tensor",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:opencv_core",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
        "//cv-cuda-pipeline/framework/tool:validate_type",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings",
    ],
)

mediapipe_proto_library(
    name = "tensors_to_detections_calculator_proto",
    srcs = ["tensors_to_detections_calculator.proto"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_options_proto",
        "//cv-cuda-pipeline/framework:calculator_proto",
    ],
)

cc_library(
    name = "tensors_to_detections_calculator",
    srcs = ["tensors_to_detections_calculator.cc"],
    copts = select({
        "//cv-cuda-pipeline:apple": [
            "-x objective-c++",
            "-fobjc-arc",  # enable reference-counting
        ],
        "//conditions:default": [],
    }),
    features = ["-layering_check"],  # allow depending on tensors_to_detections_calculator_gpu_deps
    deps = [
        ":tensors_to_detections_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:port",
        "//cv-cuda-pipeline/framework/api2:node",
        "//cv-cuda-pipeline/framework/deps:file_path",
        "//cv-cuda-pipeline/framework/formats:detection_cc_proto",
        "//cv-cuda-pipeline/framework/formats:location",
        "//cv-cuda-pipeline/framework/formats:tensor",
        "//cv-cuda-pipeline/framework/formats/object_detection:anchor_cc_proto",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "@com_google_absl//absl/log:absl_check",
        "@com_google_absl//absl/log:absl_log",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_absl//absl/types:span",
    ] + selects.with_or({
        ":compute_shader_unavailable": [],
        "//conditions:default": [":tensors_to_detections_calculator_gpu_deps"],
    }) + select({
        "//cv-cuda-pipeline:apple": [
            "//third_party/apple_frameworks:CoreVideo",
            "//third_party/apple_frameworks:MetalKit",
        ],
        "//conditions:default": [],
    }),
    alwayslink = 1,
)

cc_library(
    name = "tensors_to_detections_calculator_gpu_deps",
    visibility = ["//visibility:private"],
    deps = select({
        ":platform_apple_with_gpu": [
            "//cv-cuda-pipeline/gpu:MPPMetalHelper",
            "//cv-cuda-pipeline/gpu:MPPMetalUtil",
        ],
        # This setting is needed to allow bazel to build all targets on Mac with GPU disabled
        ":platform_apple_without_gpu": [],
        "//conditions:default": [
            "//cv-cuda-pipeline/gpu:gl_calculator_helper",
        ],
    }),
)

mediapipe_proto_library(
    name = "tensors_to_landmarks_calculator_proto",
    srcs = ["tensors_to_landmarks_calculator.proto"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_options_proto",
        "//cv-cuda-pipeline/framework:calculator_proto",
    ],
)

cc_library(
    name = "tensors_to_landmarks_calculator",
    srcs = ["tensors_to_landmarks_calculator.cc"],
    copts = select({
        "//cv-cuda-pipeline:apple": [
            "-x objective-c++",
            "-fobjc-arc",  # enable reference-counting
        ],
        "//conditions:default": [],
    }),
    deps = [
        ":tensors_to_landmarks_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/api2:node",
        "//cv-cuda-pipeline/framework/formats:landmark_cc_proto",
        "//cv-cuda-pipeline/framework/formats:tensor",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "@com_google_absl//absl/log:absl_check",
    ],
    alwayslink = 1,
)

mediapipe_proto_library(
    name = "landmarks_to_tensor_calculator_proto",
    srcs = ["landmarks_to_tensor_calculator.proto"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_options_proto",
        "//cv-cuda-pipeline/framework:calculator_proto",
    ],
)

cc_library(
    name = "landmarks_to_tensor_calculator",
    srcs = ["landmarks_to_tensor_calculator.cc"],
    hdrs = ["landmarks_to_tensor_calculator.h"],
    copts = select({
        "//cv-cuda-pipeline:apple": [
            "-x objective-c++",
            "-fobjc-arc",  # enable reference-counting
        ],
        "//conditions:default": [],
    }),
    deps = [
        ":landmarks_to_tensor_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:memory_manager",
        "//cv-cuda-pipeline/framework:memory_manager_service",
        "//cv-cuda-pipeline/framework/api2:node",
        "//cv-cuda-pipeline/framework/formats:landmark_cc_proto",
        "//cv-cuda-pipeline/framework/formats:tensor",
        "//cv-cuda-pipeline/framework/port:ret_check",
    ],
    alwayslink = 1,
)

cc_test(
    name = "landmarks_to_tensor_calculator_test",
    srcs = ["landmarks_to_tensor_calculator_test.cc"],
    deps = [
        ":landmarks_to_tensor_calculator",
        ":landmarks_to_tensor_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:calculator_runner",
        "//cv-cuda-pipeline/framework/formats:landmark_cc_proto",
        "//cv-cuda-pipeline/framework/formats:tensor",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
        "@com_google_absl//absl/memory",
        "@com_google_googletest//:gtest_main",
    ],
)

mediapipe_proto_library(
    name = "tensors_to_floats_calculator_proto",
    srcs = ["tensors_to_floats_calculator.proto"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_options_proto",
        "//cv-cuda-pipeline/framework:calculator_proto",
    ],
)

cc_library(
    name = "tensors_to_floats_calculator",
    srcs = ["tensors_to_floats_calculator.cc"],
    copts = select({
        "//cv-cuda-pipeline:apple": [
            "-x objective-c++",
            "-fobjc-arc",  # enable reference-counting
        ],
        "//conditions:default": [],
    }),
    deps = [
        ":tensors_to_floats_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/api2:node",
        "//cv-cuda-pipeline/framework/formats:tensor",
        "//cv-cuda-pipeline/framework/port:ret_check",
    ],
    alwayslink = 1,
)

cc_test(
    name = "tensors_to_floats_calculator_test",
    srcs = ["tensors_to_floats_calculator_test.cc"],
    deps = [
        ":tensors_to_floats_calculator",
        ":tensors_to_floats_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:calculator_runner",
        "//cv-cuda-pipeline/framework/formats:tensor",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
        "@com_google_absl//absl/memory",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "tensors_to_classification_calculator",
    srcs = ["tensors_to_classification_calculator.cc"],
    copts = select({
        "//cv-cuda-pipeline:apple": [
            "-x objective-c++",
            "-fobjc-arc",  # enable reference-counting
        ],
        "//conditions:default": [],
    }),
    deps = [
        ":tensors_to_classification_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:resources",
        "//cv-cuda-pipeline/framework/api2:node",
        "//cv-cuda-pipeline/framework/api2:port",
        "//cv-cuda-pipeline/framework/formats:classification_cc_proto",
        "//cv-cuda-pipeline/framework/formats:location",
        "//cv-cuda-pipeline/framework/formats:tensor",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/util:label_map_cc_proto",
        "//cv-cuda-pipeline/util:label_map_util",
        "//cv-cuda-pipeline/util:resource_util",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/container:node_hash_map",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_absl//absl/strings:string_view",
        "@com_google_absl//absl/types:span",
    ] + select({
        "//cv-cuda-pipeline:emscripten": [
        ],
        "//conditions:default": [
            "//cv-cuda-pipeline/framework/port:file_helpers",
        ],
    }),
    alwayslink = 1,
)

mediapipe_proto_library(
    name = "tensors_to_classification_calculator_proto",
    srcs = ["tensors_to_classification_calculator.proto"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_options_proto",
        "//cv-cuda-pipeline/framework:calculator_proto",
        "//cv-cuda-pipeline/util:label_map_proto",
    ],
)

cc_test(
    name = "tensors_to_classification_calculator_test",
    srcs = ["tensors_to_classification_calculator_test.cc"],
    data = ["testdata/labelmap.txt"],
    deps = [
        ":tensors_to_classification_calculator",
        ":tensors_to_classification_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:calculator_runner",
        "//cv-cuda-pipeline/framework/formats:classification_cc_proto",
        "//cv-cuda-pipeline/framework/formats:tensor",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
        "@com_google_absl//absl/memory",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "tensor_to_joints_calculator",
    srcs = ["tensor_to_joints_calculator.cc"],
    hdrs = ["tensor_to_joints_calculator.h"],
    deps = [
        ":tensor_to_joints_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/api2:node",
        "//cv-cuda-pipeline/framework/formats:body_rig_cc_proto",
        "//cv-cuda-pipeline/framework/formats:tensor",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
    ],
    alwayslink = 1,
)

mediapipe_proto_library(
    name = "tensor_to_joints_calculator_proto",
    srcs = ["tensor_to_joints_calculator.proto"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_options_proto",
        "//cv-cuda-pipeline/framework:calculator_proto",
    ],
)

cc_test(
    name = "tensor_to_joints_calculator_test",
    srcs = ["tensor_to_joints_calculator_test.cc"],
    deps = [
        ":tensor_to_joints_calculator",
        ":tensor_to_joints_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:calculator_runner",
        "//cv-cuda-pipeline/framework:timestamp",
        "//cv-cuda-pipeline/framework/formats:body_rig_cc_proto",
        "//cv-cuda-pipeline/framework/formats:tensor",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
        "@com_google_absl//absl/strings",
    ],
)

cc_library(
    name = "image_to_tensor_calculator",
    srcs = ["image_to_tensor_calculator.cc"],
    hdrs = ["image_to_tensor_calculator.h"],
    copts = select({
        "//cv-cuda-pipeline:apple": [
            "-x objective-c++",
            "-fobjc-arc",  # enable reference-counting
        ],
        "//conditions:default": [],
    }),
    features = ["-layering_check"],  # allow depending on image_to_tensor_calculator_gpu_deps
    deps = [
        ":image_to_tensor_calculator_cc_proto",
        ":image_to_tensor_converter",
        ":image_to_tensor_utils",
        ":loose_headers",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:memory_manager",
        "//cv-cuda-pipeline/framework:memory_manager_service",
        "//cv-cuda-pipeline/framework:port",
        "//cv-cuda-pipeline/framework/api2:node",
        "//cv-cuda-pipeline/framework/api3:calculator",
        "//cv-cuda-pipeline/framework/api3:calculator_context",
        "//cv-cuda-pipeline/framework/api3:calculator_contract",
        "//cv-cuda-pipeline/framework/api3:contract",
        "//cv-cuda-pipeline/framework/api3:node",
        "//cv-cuda-pipeline/framework/api3:one_of",
        "//cv-cuda-pipeline/framework/formats:image",
        "//cv-cuda-pipeline/framework/formats:image_frame",
        "//cv-cuda-pipeline/framework/formats:rect_cc_proto",
        "//cv-cuda-pipeline/framework/formats:tensor",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/framework/port:statusor",
        "//cv-cuda-pipeline/gpu:gpu_origin_cc_proto",
        "//cv-cuda-pipeline/gpu:gpu_origin_utils",
        "//cv-cuda-pipeline/gpu/webgpu:webgpu_check",
        "@com_google_absl//absl/log:absl_check",
        "@com_google_absl//absl/log:absl_log",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings:string_view",
    ] + select({
        "//cv-cuda-pipeline/gpu:disable_gpu": [],
        "//conditions:default": [":image_to_tensor_calculator_gpu_deps"],
    }) + select({
        "//cv-cuda-pipeline/framework/port:disable_opencv": [],
        "//conditions:default": [":image_to_tensor_converter_opencv"],
    }) + select({
        "//cv-cuda-pipeline/framework/port:enable_halide": [
            ":image_to_tensor_converter_frame_buffer",
        ],
        "//conditions:default": [],
    }),
    alwayslink = 1,
)

cc_library(
    name = "image_to_tensor_calculator_gpu_deps",
    visibility = ["//visibility:private"],
    deps = selects.with_or({
        "//cv-cuda-pipeline:emscripten": [
            ":image_to_tensor_converter_gl_texture",
            "//cv-cuda-pipeline/gpu:gl_calculator_helper",
            "//cv-cuda-pipeline/gpu:gpu_buffer",
            "//cv-cuda-pipeline/gpu:gpu_service",
        ],
        "//cv-cuda-pipeline:apple": [
            ":image_to_tensor_converter_metal",
            "//cv-cuda-pipeline/gpu:gl_calculator_helper",
            "//cv-cuda-pipeline/gpu:MPPMetalHelper",
            "//cv-cuda-pipeline/gpu:gpu_buffer",
            "//cv-cuda-pipeline/gpu:gpu_service",
        ],
        "//conditions:default": [
            ":image_to_tensor_converter_gl_buffer",
            "//cv-cuda-pipeline/gpu:gl_calculator_helper",
            "//cv-cuda-pipeline/gpu:gpu_buffer",
            "//cv-cuda-pipeline/gpu:gpu_service",
        ],
    }) + select({
        "//cv-cuda-pipeline/gpu/webgpu:use_webgpu": [
            "//cv-cuda-pipeline/gpu/webgpu:image_to_tensor_converter_webgpu_texture",
            "//cv-cuda-pipeline/gpu/webgpu:webgpu_texture_buffer",
        ],
        "//conditions:default": [],
    }),
)

mediapipe_proto_library(
    name = "image_to_tensor_calculator_proto",
    srcs = ["image_to_tensor_calculator.proto"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_options_proto",
        "//cv-cuda-pipeline/framework:calculator_proto",
        "//cv-cuda-pipeline/gpu:gpu_origin_proto",
    ],
)


cc_library(
    name = "image_to_tensor_converter",
    hdrs = ["image_to_tensor_converter.h"],
    copts = select({
        "//cv-cuda-pipeline:apple": [
            "-x objective-c++",
            "-fobjc-arc",  # enable reference-counting
        ],
        "//conditions:default": [],
    }),
    deps = [
        ":image_to_tensor_utils",
        "//cv-cuda-pipeline/framework/formats:image",
        "//cv-cuda-pipeline/framework/formats:tensor",
        "//cv-cuda-pipeline/framework/port:statusor",
    ],
)

cc_library(
    name = "image_to_tensor_converter_opencv",
    srcs = ["image_to_tensor_converter_opencv.cc"],
    hdrs = ["image_to_tensor_converter_opencv.h"],
    copts = select({
        "//cv-cuda-pipeline:apple": [
            "-x objective-c++",
            "-fobjc-arc",  # enable reference-counting
        ],
        "//conditions:default": [],
    }),
    deps = [
        ":image_to_tensor_converter",
        ":image_to_tensor_utils",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/formats:image",
        "//cv-cuda-pipeline/framework/formats:image_format_cc_proto",
        "//cv-cuda-pipeline/framework/formats:image_opencv",
        "//cv-cuda-pipeline/framework/formats:tensor",
        "//cv-cuda-pipeline/framework/port:opencv_core",
        "//cv-cuda-pipeline/framework/port:opencv_imgproc",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/framework/port:statusor",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings",
    ],
)

cc_library(
    name = "image_to_tensor_converter_frame_buffer",
    srcs = ["image_to_tensor_converter_frame_buffer.cc"],
    hdrs = ["image_to_tensor_converter_frame_buffer.h"],
    deps = [
        ":image_to_tensor_converter",
        ":image_to_tensor_utils",
        "//cv-cuda-pipeline/framework:calculator_context",
        "//cv-cuda-pipeline/framework/formats:frame_buffer",
        "//cv-cuda-pipeline/framework/formats:image",
        "//cv-cuda-pipeline/framework/formats:tensor",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/gpu:frame_buffer_view",
        "//cv-cuda-pipeline/util/frame_buffer:frame_buffer_util",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings:str_format",
    ],
)

cc_library(
    name = "image_to_tensor_converter_gl_buffer",
    srcs = ["image_to_tensor_converter_gl_buffer.cc"],
    hdrs = ["image_to_tensor_converter_gl_buffer.h"],
    visibility = ["//visibility:private"],
    deps = ["//cv-cuda-pipeline/framework:port"] + selects.with_or({
        "//cv-cuda-pipeline:emscripten": [],
        "//cv-cuda-pipeline:apple": [],
        "//conditions:default": [
            ":image_to_tensor_converter",
            ":image_to_tensor_converter_gl_utils",
            ":image_to_tensor_utils",
            "@com_google_absl//absl/strings",
            "//cv-cuda-pipeline/framework:calculator_framework",
            "//cv-cuda-pipeline/framework/formats:rect_cc_proto",
            "//cv-cuda-pipeline/framework/formats:tensor",
            "//cv-cuda-pipeline/framework/port:ret_check",
            "//cv-cuda-pipeline/framework/port:status",
            "//cv-cuda-pipeline/framework/port:statusor",
            "//cv-cuda-pipeline/gpu:gl_calculator_helper",
            "//cv-cuda-pipeline/framework/formats:image",
            "//cv-cuda-pipeline/gpu:gpu_buffer_format",
            "@litert//tflite/delegates/gpu/common:shape",
            "@litert//tflite/delegates/gpu/common:types",
            "@litert//tflite/delegates/gpu/gl:command_queue",
            "@litert//tflite/delegates/gpu/gl:gl_buffer",
            "@litert//tflite/delegates/gpu/gl:gl_call",
            "@litert//tflite/delegates/gpu/gl:gl_texture",
            "@litert//tflite/delegates/gpu/gl:request_gpu_info",
            "@litert//tflite/delegates/gpu/gl:variable",
            "@litert//tflite/delegates/gpu/gl/converters:util",
        ],
    }),
)

cc_library(
    name = "image_to_tensor_converter_gl_texture",
    srcs = ["image_to_tensor_converter_gl_texture.cc"],
    hdrs = ["image_to_tensor_converter_gl_texture.h"],
    visibility = ["//cv-cuda-pipeline/gpu/webgpu:__subpackages__"],
    deps = ["//cv-cuda-pipeline/framework:port"] + select({
        "//cv-cuda-pipeline/gpu:disable_gpu": [],
        "//conditions:default": [
            ":image_to_tensor_converter",
            ":image_to_tensor_converter_gl_utils",
            ":image_to_tensor_utils",
            "//cv-cuda-pipeline/framework:calculator_framework",
            "//cv-cuda-pipeline/framework/formats:image",
            "//cv-cuda-pipeline/framework/formats:tensor",
            "//cv-cuda-pipeline/framework/port:ret_check",
            "//cv-cuda-pipeline/framework/port:status",
            "//cv-cuda-pipeline/framework/port:statusor",
            "//cv-cuda-pipeline/gpu:gl_calculator_helper",
            "//cv-cuda-pipeline/gpu:gl_simple_shaders",
            "//cv-cuda-pipeline/gpu:shader_util",
            "@com_google_absl//absl/log:absl_log",
            "@com_google_absl//absl/strings",
        ],
    }),
)

cc_library(
    name = "image_to_tensor_converter_gl_utils",
    srcs = ["image_to_tensor_converter_gl_utils.cc"],
    hdrs = ["image_to_tensor_converter_gl_utils.h"],
    visibility = ["//visibility:private"],
    deps = ["//cv-cuda-pipeline/framework:port"] + select({
        "//cv-cuda-pipeline/gpu:disable_gpu": [],
        "//conditions:default": [
            "//cv-cuda-pipeline/framework/port:status",
            "//cv-cuda-pipeline/framework/port:statusor",
            "//cv-cuda-pipeline/gpu:gl_base",
            "//cv-cuda-pipeline/gpu:gl_context",
        ],
    }),
)

cc_library(
    name = "image_to_tensor_converter_metal",
    srcs = ["image_to_tensor_converter_metal.cc"],
    hdrs = ["image_to_tensor_converter_metal.h"],
    copts = select({
        "//cv-cuda-pipeline:apple": [
            "-x objective-c++",
            "-fobjc-arc",  # enable reference-counting
        ],
        "//conditions:default": [],
    }),
    linkopts = select({
        "//cv-cuda-pipeline:apple": [
            "-framework CoreVideo",
            "-framework MetalKit",
        ],
        "//conditions:default": [],
    }),
    visibility = ["//visibility:private"],
    deps = ["//cv-cuda-pipeline/framework:port"] + select({
        "//cv-cuda-pipeline:apple": [
            ":image_to_tensor_converter",
            ":image_to_tensor_utils",
            "//cv-cuda-pipeline/framework:calculator_framework",
            "//cv-cuda-pipeline/framework/formats:image",
            "//cv-cuda-pipeline/framework/formats:tensor",
            "//cv-cuda-pipeline/framework/port:ret_check",
            "//cv-cuda-pipeline/framework/port:status",
            "//cv-cuda-pipeline/framework/port:statusor",
            "//cv-cuda-pipeline/gpu:MPPMetalHelper",
            "//cv-cuda-pipeline/gpu:gpu_buffer_format",
            "@com_google_absl//absl/strings",
            "@litert//tflite/delegates/gpu/common:shape",
            "@litert//tflite/delegates/gpu/common:types",
        ],
        "//conditions:default": [],
    }),
)

cc_library(
    name = "image_to_tensor_utils",
    srcs = ["image_to_tensor_utils.cc"],
    hdrs = ["image_to_tensor_utils.h"],
    copts = select({
        "//cv-cuda-pipeline:apple": [
            "-x objective-c++",
            "-fobjc-arc",  # enable reference-counting
        ],
        "//conditions:default": [],
    }),
    deps = [
        ":image_to_tensor_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:packet",
        "//cv-cuda-pipeline/framework/api2:packet",
        "//cv-cuda-pipeline/framework/api2:port",
        "//cv-cuda-pipeline/framework/formats:image",
        "//cv-cuda-pipeline/framework/formats:image_frame",
        "//cv-cuda-pipeline/framework/formats:rect_cc_proto",
        "//cv-cuda-pipeline/framework/formats:tensor",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/framework/port:statusor",
        "//cv-cuda-pipeline/gpu:gpu_origin_cc_proto",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/types:optional",
    ] + select({
        "//cv-cuda-pipeline/gpu:disable_gpu": [],
        "//conditions:default": ["//cv-cuda-pipeline/gpu:gpu_buffer"],
    }),
)

cc_test(
    name = "image_to_tensor_utils_test",
    srcs = ["image_to_tensor_utils_test.cc"],
    deps = [
        ":image_to_tensor_utils",
        "//cv-cuda-pipeline/framework/formats:rect_cc_proto",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
        "//cv-cuda-pipeline/framework/port:status",
        "@com_google_absl//absl/status",
    ],
)

# Copied from /cv-cuda-pipeline/calculators/tflite/BUILD
selects.config_setting_group(
    name = "gpu_inference_disabled",
    match_any = [
        "//cv-cuda-pipeline/gpu:disable_gpu",
    ],
)

mediapipe_proto_library(
    name = "tensors_to_segmentation_calculator_proto",
    srcs = ["tensors_to_segmentation_calculator.proto"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_options_proto",
        "//cv-cuda-pipeline/framework:calculator_proto",
        "//cv-cuda-pipeline/gpu:gpu_origin_proto",
    ],
)

cc_library(
    name = "tensors_to_segmentation_calculator",
    srcs = ["tensors_to_segmentation_calculator.cc"],
    hdrs = ["tensors_to_segmentation_calculator.h"],
    copts = select({
        "//cv-cuda-pipeline:apple": [
            "-x objective-c++",
            "-fobjc-arc",  # enable reference-counting
        ],
        "//conditions:default": [],
    }),
    features = ["-layering_check"],  # allow depending on tensors_to_segmentation_calculator_gpu_deps
    deps = [
        ":tensors_to_segmentation_calculator_cc_proto",
        ":tensors_to_segmentation_converter",
        ":tensors_to_segmentation_utils",
        "//cv-cuda-pipeline/framework:calculator_context",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:port",
        "//cv-cuda-pipeline/framework/api3:calculator",
        "//cv-cuda-pipeline/framework/api3:contract",
        "//cv-cuda-pipeline/framework/api3:node",
        "//cv-cuda-pipeline/framework/formats:image",
        "//cv-cuda-pipeline/framework/formats:tensor",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/util:resource_util",
        "@com_google_absl//absl/status",
        "@litert//tflite:framework",
    ] + select({
        "//cv-cuda-pipeline/gpu:disable_gpu": [],
        "//conditions:default": [":tensors_to_segmentation_calculator_gpu_deps"],
    }) + select({
        "//cv-cuda-pipeline/framework/port:disable_opencv": [],
        "//conditions:default": [
            ":tensors_to_segmentation_converter_opencv",
        ],
    }),
    alwayslink = 1,
)

cc_library(
    name = "tensors_to_segmentation_calculator_gpu_deps",
    visibility = ["//visibility:private"],
    deps = selects.with_or({
        "//cv-cuda-pipeline:emscripten": [
            ":tensors_to_segmentation_converter_gl_texture",
            "//cv-cuda-pipeline/gpu:gl_calculator_helper",
        ],
        ":platform_apple_with_gpu": [
            ":tensors_to_segmentation_converter_metal",
            "//cv-cuda-pipeline/gpu:MPPMetalHelper",
            "//cv-cuda-pipeline/gpu:gl_calculator_helper",
        ],
        # This setting is needed to allow bazel to build all targets on Mac with GPU disabled
        ":platform_apple_without_gpu": [],
        "//conditions:default": [
            ":tensors_to_segmentation_converter_gl_buffer",
            "//cv-cuda-pipeline/gpu:gl_calculator_helper",
        ],
    }),
)

cc_library(
    name = "tensors_to_segmentation_utils",
    srcs = ["tensors_to_segmentation_utils.cc"],
    hdrs = ["tensors_to_segmentation_utils.h"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:port",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/gpu:gpu_origin_cc_proto",
        "@com_google_absl//absl/status:statusor",
    ] + selects.with_or({
        "//cv-cuda-pipeline/gpu:disable_gpu": [],
        "//conditions:default": [
            "//cv-cuda-pipeline/gpu:gl_base_hdr",
        ],
    }),
)

cc_test(
    name = "tensors_to_segmentation_utils_test",
    srcs = ["tensors_to_segmentation_utils_test.cc"],
    deps = [
        ":tensors_to_segmentation_utils",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:status_matchers",
        "@com_google_absl//absl/status:statusor",
    ],
)

cc_library(
    name = "tensors_to_segmentation_converter",
    hdrs = ["tensors_to_segmentation_converter.h"],
    visibility = ["//visibility:private"],
    deps = [
        "//cv-cuda-pipeline/framework/formats:image",
        "//cv-cuda-pipeline/framework/formats:tensor",
        "@com_google_absl//absl/status:statusor",
    ],
)

cc_library(
    name = "tensors_to_segmentation_converter_opencv",
    srcs = ["tensors_to_segmentation_converter_opencv.cc"],
    hdrs = ["tensors_to_segmentation_converter_opencv.h"],
    visibility = ["//visibility:private"],
    deps = [
        ":tensors_to_segmentation_calculator_cc_proto",
        ":tensors_to_segmentation_converter",
        ":tensors_to_segmentation_utils",
        "//cv-cuda-pipeline/framework/formats:image",
        "//cv-cuda-pipeline/framework/formats:image_frame",
        "//cv-cuda-pipeline/framework/formats:image_opencv",
        "//cv-cuda-pipeline/framework/formats:tensor",
        "//cv-cuda-pipeline/framework/port:opencv_core",
        "//cv-cuda-pipeline/framework/port:opencv_imgproc",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
    ],
)

cc_library(
    name = "tensors_to_segmentation_converter_gl_texture",
    srcs = ["tensors_to_segmentation_converter_gl_texture.cc"],
    hdrs = ["tensors_to_segmentation_converter_gl_texture.h"],
    visibility = ["//visibility:private"],
    deps = [
        "//cv-cuda-pipeline/framework:port",
    ] + select({
        "//cv-cuda-pipeline/gpu:disable_gpu": [],
        "//conditions:default": [
            ":tensors_to_segmentation_calculator_cc_proto",
            ":tensors_to_segmentation_converter",
            ":tensors_to_segmentation_utils",
            "//cv-cuda-pipeline/framework:calculator_context",
            "//cv-cuda-pipeline/framework:calculator_framework",
            "//cv-cuda-pipeline/framework/formats:image",
            "//cv-cuda-pipeline/framework/formats:tensor",
            "//cv-cuda-pipeline/framework/port:ret_check",
            "//cv-cuda-pipeline/framework/port:status",
            "//cv-cuda-pipeline/gpu:gl_base_hdr",
            "//cv-cuda-pipeline/gpu:gl_calculator_helper",
            "//cv-cuda-pipeline/gpu:gl_simple_shaders",
            "//cv-cuda-pipeline/gpu:gpu_buffer",
            "//cv-cuda-pipeline/gpu:gpu_buffer_format",
            "//cv-cuda-pipeline/gpu:gpu_origin_cc_proto",
            "//cv-cuda-pipeline/gpu:gpu_origin_utils",
            "//cv-cuda-pipeline/gpu:shader_util",
            "@com_google_absl//absl/status",
            "@com_google_absl//absl/status:statusor",
            "@com_google_absl//absl/strings",
            "@com_google_absl//absl/strings:str_format",
        ],
    }),
)

cc_library(
    name = "tensors_to_segmentation_converter_gl_buffer",
    srcs = ["tensors_to_segmentation_converter_gl_buffer.cc"],
    hdrs = ["tensors_to_segmentation_converter_gl_buffer.h"],
    visibility = ["//visibility:private"],
    deps = ["//cv-cuda-pipeline/framework:port"] + selects.with_or({
        "//cv-cuda-pipeline:emscripten": [],
        "//cv-cuda-pipeline:apple": [],
        "//conditions:default": [
            ":tensors_to_segmentation_calculator_cc_proto",
            ":tensors_to_segmentation_converter",
            ":tensors_to_segmentation_utils",
            "@com_google_absl//absl/memory",
            "@com_google_absl//absl/status",
            "@com_google_absl//absl/status:statusor",
            "@com_google_absl//absl/strings",
            "@com_google_absl//absl/strings:str_format",
            "//cv-cuda-pipeline/framework:calculator_context",
            "//cv-cuda-pipeline/framework:calculator_framework",
            "//cv-cuda-pipeline/framework/formats:image",
            "//cv-cuda-pipeline/framework/formats:tensor",
            "//cv-cuda-pipeline/framework/port:ret_check",
            "//cv-cuda-pipeline/framework/port:status",
            "//cv-cuda-pipeline/gpu:gpu_origin_cc_proto",
            "//cv-cuda-pipeline/gpu:gpu_origin_utils",
            "//cv-cuda-pipeline/gpu:gl_base_hdr",
            "//cv-cuda-pipeline/gpu:gl_calculator_helper",
            "//cv-cuda-pipeline/gpu:gl_simple_shaders",
            "//cv-cuda-pipeline/gpu:gpu_buffer",
            "//cv-cuda-pipeline/gpu:gpu_buffer_format",
            "//cv-cuda-pipeline/gpu:shader_util",
            "@litert//tflite/delegates/gpu/gl:gl_program",
            "@litert//tflite/delegates/gpu/gl:gl_shader",
            "@litert//tflite/delegates/gpu/gl:gl_texture",
            "@litert//tflite/delegates/gpu/gl/converters:util",
            "@litert//tflite/delegates/gpu/common:data_type",
            "@litert//tflite/delegates/gpu/common:types",
        ],
    }),
)

cc_library(
    name = "tensors_to_segmentation_converter_metal",
    srcs = ["tensors_to_segmentation_converter_metal.cc"],
    hdrs = ["tensors_to_segmentation_converter_metal.h"],
    copts = select({
        "//cv-cuda-pipeline:apple": [
            "-x objective-c++",
            "-fobjc-arc",  # enable reference-counting
        ],
        "//conditions:default": [],
    }),
    visibility = ["//visibility:private"],
    deps = [
        "//cv-cuda-pipeline/framework:port",
    ] + selects.with_or({
        ":gpu_inference_disabled": [],
        ":platform_apple_with_gpu": [
            ":tensors_to_segmentation_calculator_cc_proto",
            ":tensors_to_segmentation_converter",
            ":tensors_to_segmentation_utils",
            "//cv-cuda-pipeline/gpu:MPPMetalUtil",
            "//cv-cuda-pipeline/gpu:MPPMetalHelper",
            "//third_party/apple_frameworks:MetalKit",
            "@com_google_absl//absl/status",
            "@com_google_absl//absl/status:statusor",
            "//cv-cuda-pipeline/framework:calculator_context",
            "//cv-cuda-pipeline/framework:calculator_framework",
            "//cv-cuda-pipeline/framework/formats:image",
            "//cv-cuda-pipeline/framework/formats:tensor",
            "//cv-cuda-pipeline/framework/port:ret_check",
            "//cv-cuda-pipeline/framework/port:status",
            "//cv-cuda-pipeline/gpu:gl_calculator_helper",
            "//cv-cuda-pipeline/gpu:gl_simple_shaders",
            "//cv-cuda-pipeline/gpu:gpu_origin_cc_proto",
            "//cv-cuda-pipeline/gpu:gpu_origin_utils",
            "//cv-cuda-pipeline/gpu:shader_util",
            "//cv-cuda-pipeline/util:resource_util",
            "@litert//tflite:framework",
        ],
        "//conditions:default": [],
    }),
)

cc_test(
    name = "tensors_to_segmentation_calculator_test",
    srcs = ["tensors_to_segmentation_calculator_test.cc"],
    deps = [
        ":tensors_to_segmentation_calculator",
        ":tensors_to_segmentation_calculator_cc_proto",
        ":tensors_to_segmentation_calculator_test_utils",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:calculator_runner",
        "//cv-cuda-pipeline/framework:packet",
        "//cv-cuda-pipeline/framework:timestamp",
        "//cv-cuda-pipeline/framework/api3:function_runner",
        "//cv-cuda-pipeline/framework/api3:graph",
        "//cv-cuda-pipeline/framework/api3:packet",
        "//cv-cuda-pipeline/framework/api3:stream",
        "//cv-cuda-pipeline/framework/formats:image",
        "//cv-cuda-pipeline/framework/formats:image_format_cc_proto",
        "//cv-cuda-pipeline/framework/formats:image_opencv",
        "//cv-cuda-pipeline/framework/formats:rect_cc_proto",
        "//cv-cuda-pipeline/framework/formats:tensor",
        "//cv-cuda-pipeline/framework/port:gtest_main",
    ],
)

cc_library(
    name = "tensors_to_segmentation_calculator_test_utils",
    testonly = True,
    srcs = ["tensors_to_segmentation_calculator_test_utils.cc"],
    hdrs = ["tensors_to_segmentation_calculator_test_utils.h"],
    deps = [
        ":tensors_to_segmentation_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:packet",
        "//cv-cuda-pipeline/framework/formats:tensor",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
        "@com_google_absl//absl/log:absl_log",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings",
    ],
)

cc_test(
    name = "tensors_to_segmentation_calculator_test_utils_test",
    srcs = ["tensors_to_segmentation_calculator_test_utils_test.cc"],
    deps = [
        ":tensors_to_segmentation_calculator_cc_proto",
        ":tensors_to_segmentation_calculator_test_utils",
        "//cv-cuda-pipeline/framework/port:gtest_main",
    ],
)

cc_library(
    name = "tensors_dequantization_calculator",
    srcs = ["tensors_dequantization_calculator.cc"],
    copts = select({
        "//cv-cuda-pipeline:apple": [
            "-x objective-c++",
            "-fobjc-arc",  # enable reference-counting
        ],
        "//conditions:default": [],
    }),
    deps = [
        "//cv-cuda-pipeline/framework:calculator_context",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:memory_manager",
        "//cv-cuda-pipeline/framework:memory_manager_service",
        "//cv-cuda-pipeline/framework/api2:node",
        "//cv-cuda-pipeline/framework/api2:port",
        "//cv-cuda-pipeline/framework/formats:tensor",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings",
    ],
    alwayslink = 1,
)

# For a more maintainable build this target should not exist and the headers
# should  be split into the existing cc_library targets, but this change was
# automatically  done so that we can remove long standing issues and complexity
# in the build system. It's up to the OWNERS of this package to get rid of it or
# not. The use of the textual_hdrs attribute is discouraged, use hdrs instead.
# Here it is used to avoid header parsing errors in packages where the feature
# parse_headers was enabled since loose headers were not being parsed.
cc_library(
    name = "loose_headers",
    tags = ["avoid_dep"],
    textual_hdrs = [
        "image_to_tensor_converter_gl_buffer.h",
        "image_to_tensor_converter_gl_texture.h",
    ],
    visibility = ["//visibility:private"],
)

cc_test(
    name = "tensors_dequantization_calculator_test",
    srcs = ["tensors_dequantization_calculator_test.cc"],
    deps = [
        ":tensors_dequantization_calculator",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:calculator_runner",
        "//cv-cuda-pipeline/framework/formats:tensor",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
        "@com_google_absl//absl/status",
    ],
)

cc_library(
    name = "vector_to_tensor_calculator",
    srcs = ["vector_to_tensor_calculator.cc"],
    deps = [
        ":vector_to_tensor_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:memory_manager",
        "//cv-cuda-pipeline/framework:memory_manager_service",
        "//cv-cuda-pipeline/framework/api2:node",
        "//cv-cuda-pipeline/framework/api2:packet",
        "//cv-cuda-pipeline/framework/api2:port",
        "//cv-cuda-pipeline/framework/formats:tensor",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/log:check",
        "@com_google_absl//absl/status",
    ],
    alwayslink = 1,
)

mediapipe_proto_library(
    name = "vector_to_tensor_calculator_proto",
    srcs = ["vector_to_tensor_calculator.proto"],
)

cc_test(
    name = "vector_to_tensor_calculator_test",
    srcs = ["vector_to_tensor_calculator_test.cc"],
    deps = [
        ":vector_to_tensor_calculator",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:calculator_runner",
        "//cv-cuda-pipeline/framework/formats:tensor",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
        "//cv-cuda-pipeline/framework/port:status",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
    ],
)
