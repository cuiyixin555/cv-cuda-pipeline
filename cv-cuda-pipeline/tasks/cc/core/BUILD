# Copyright 2022 The MediaPipe Authors.
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

load("@litert//tflite/core/shims:cc_library_with_tflite.bzl", "cc_library_with_tflite", "cc_test_with_tflite")
load("@rules_cc//cc:cc_library.bzl", "cc_library")
load("@rules_cc//cc:cc_test.bzl", "cc_test")

package(default_visibility = ["//visibility:public"])

licenses(["notice"])

cc_library(
    name = "tasks_logging_factory",
    visibility = ["//cv-cuda-pipeline/tasks:__subpackages__"],
    deps = ["//cv-cuda-pipeline/tasks/cc/core/logging/factory:logging_factory"],
)

cc_library(
    name = "base_options",
    srcs = ["base_options.cc"],
    hdrs = ["base_options.h"],
    deps = [
        ":host_environment",
        ":mediapipe_builtin_op_resolver",
        "//cv-cuda-pipeline/calculators/tensor:inference_calculator_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/core/proto:acceleration_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/core/proto:base_options_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/core/proto:external_file_cc_proto",
        "@com_google_absl//absl/log:absl_log",
        "@com_google_absl//absl/memory",
        "@litert//tflite/core/api:op_resolver",
        "@litert//tflite/kernels:builtin_ops",
    ],
)

cc_library(
    name = "host_environment",
    srcs = ["host_environment.cc"],
    hdrs = ["host_environment.h"],
    deps = [
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/log:absl_log",
    ],
)

cc_library(
    name = "running_mode",
    hdrs = ["running_mode.h"],
)

cc_test(
    name = "base_options_test",
    srcs = ["base_options_test.cc"],
    data = [
        "//cv-cuda-pipeline/tasks/testdata/core:test_models",
    ],
    deps = [
        ":base_options",
        ":utils",
        "//cv-cuda-pipeline/calculators/tensor:inference_calculator_cc_proto",
        "//cv-cuda-pipeline/framework/port:gtest",
        "//cv-cuda-pipeline/tasks/cc/core/proto:acceleration_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/core/proto:external_file_cc_proto",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "external_file_handler",
    srcs = ["external_file_handler.cc"],
    hdrs = ["external_file_handler.h"],
    deps = [
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/tasks/cc:common",
        "//cv-cuda-pipeline/tasks/cc/core/proto:external_file_cc_proto",
        "//cv-cuda-pipeline/util:resource_util",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
    ],
)

cc_library_with_tflite(
    name = "mediapipe_builtin_op_resolver",
    srcs = ["mediapipe_builtin_op_resolver.cc"],
    hdrs = ["mediapipe_builtin_op_resolver.h"],
    tflite_deps = [
        "@litert//tflite/kernels:builtin_ops",
    ],
    deps = [
        "//cv-cuda-pipeline/tasks/cc/text/custom_ops/ragged:ragged_tensor_to_tensor_tflite",
        "//cv-cuda-pipeline/tasks/cc/text/custom_ops/sentencepiece:sentencepiece_tokenizer_tflite",
        "//cv-cuda-pipeline/tasks/cc/text/language_detector/custom_ops:kmeans_embedding_lookup",
        "//cv-cuda-pipeline/tasks/cc/text/language_detector/custom_ops:ngram_hash",
        "//cv-cuda-pipeline/tasks/cc/vision/custom_ops:fused_batch_norm",
        "//cv-cuda-pipeline/util/tflite/operations:landmarks_to_transform_matrix",
        "//cv-cuda-pipeline/util/tflite/operations:max_pool_argmax",
        "//cv-cuda-pipeline/util/tflite/operations:max_unpooling",
        "//cv-cuda-pipeline/util/tflite/operations:transform_landmarks",
        "//cv-cuda-pipeline/util/tflite/operations:transform_tensor_bilinear",
        "//cv-cuda-pipeline/util/tflite/operations:transpose_conv_bias",
    ],
)

cc_library_with_tflite(
    name = "model_task_graph",
    srcs = ["model_task_graph.cc"],
    hdrs = ["model_task_graph.h"],
    tflite_deps = [
        ":model_resources",
        ":model_resources_cache",
    ],
    deps = [
        ":model_asset_bundle_resources",
        ":model_resources_calculator",
        "//cv-cuda-pipeline/calculators/tensor:inference_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:subgraph",
        "//cv-cuda-pipeline/framework/api2:builder",
        "//cv-cuda-pipeline/framework/api2:port",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/tasks/cc:common",
        "//cv-cuda-pipeline/tasks/cc/core/proto:acceleration_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/core/proto:base_options_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/core/proto:external_file_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/core/proto:inference_subgraph_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/core/proto:model_resources_calculator_cc_proto",
        "@com_google_absl//absl/log:absl_log",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
    ],
)

# TODO: Enable this test

cc_library_with_tflite(
    name = "model_resources",
    srcs = ["model_resources.cc"],
    hdrs = ["model_resources.h"],
    tflite_deps = [
        "@litert//tflite:framework_stable",
        "@litert//tflite/kernels:builtin_ops",
        "@litert//tflite/tools:verifier",
    ],
    deps = [
        ":external_file_handler",
        "//cv-cuda-pipeline/framework/api2:packet",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/tasks/cc:common",
        "//cv-cuda-pipeline/tasks/cc/core/proto:external_file_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/metadata:metadata_extractor",
        "//cv-cuda-pipeline/util:resource_util",
        "//cv-cuda-pipeline/util:resource_util_custom",
        "//cv-cuda-pipeline/util/tflite:error_reporter",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@litert//tflite/core/api:error_reporter",
        "@litert//tflite/core/api:op_resolver",
    ],
)

cc_test_with_tflite(
    name = "model_resources_test",
    srcs = ["model_resources_test.cc"],
    data = [
        "//cv-cuda-pipeline/tasks/testdata/core:test_models",
    ],
    tflite_deps = [
        ":model_resources",
        "@litert//tflite:framework_stable",
        "@litert//tflite:test_util",
        "@litert//tflite/kernels:builtin_ops",
    ],
    deps = [
        ":utils",
        "//cv-cuda-pipeline/framework/api2:packet",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/tasks/cc:common",
        "//cv-cuda-pipeline/tasks/cc/core/proto:external_file_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/metadata:metadata_extractor",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:cord",
        "@litert//tflite:mutable_op_resolver",
        "@litert//tflite/c:common",
        "@litert//tflite/core/api:op_resolver",
    ],
)

cc_library_with_tflite(
    name = "model_resources_cache",
    srcs = ["model_resources_cache.cc"],
    hdrs = ["model_resources_cache.h"],
    tflite_deps = [
        ":model_resources",
        "@litert//tflite/kernels:builtin_ops",
    ],
    deps = [
        ":model_asset_bundle_resources",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/api2:packet",
        "//cv-cuda-pipeline/tasks/cc:common",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@litert//tflite/core/api:op_resolver",
    ],
)

cc_library_with_tflite(
    name = "model_resources_calculator",
    srcs = ["model_resources_calculator.cc"],
    tflite_deps = [
        ":model_resources",
        ":model_resources_cache",
    ],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/api2:node",
        "//cv-cuda-pipeline/framework/api2:port",
        "//cv-cuda-pipeline/tasks/cc/core/proto:external_file_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/core/proto:model_resources_calculator_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/metadata:metadata_extractor",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@litert//tflite/core/api:op_resolver",
    ],
    alwayslink = 1,
)

cc_test(
    name = "model_resources_calculator_test",
    srcs = ["model_resources_calculator_test.cc"],
    data = [
        "//cv-cuda-pipeline/tasks/testdata/core:test_models",
    ],
    deps = [
        ":model_resources",
        ":model_resources_cache",
        ":model_resources_calculator",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
        "//cv-cuda-pipeline/tasks/cc/core/proto:external_file_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/core/proto:model_resources_calculator_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/metadata:metadata_extractor",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@litert//tflite:test_util",
        "@litert//tflite/core/api:op_resolver",
    ],
)

cc_library_with_tflite(
    name = "task_runner",
    srcs = ["task_runner.cc"],
    hdrs = ["task_runner.h"],
    tflite_deps = [
        ":model_resources",
        ":model_resources_cache",
        ":model_resources_calculator",
    ],
    deps = [
        ":host_environment",
        ":running_mode",
        ":tasks_logging_factory",
        "//cv-cuda-pipeline/framework:calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:executor",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/framework/tool:name_util",
        "//cv-cuda-pipeline/gpu:gpu_shared_data_internal",
        "//cv-cuda-pipeline/tasks/cc:common",
        "//cv-cuda-pipeline/tasks/cc/core/logging:tasks_dummy_logger",
        "//cv-cuda-pipeline/tasks/cc/core/logging:tasks_logger",
        "//cv-cuda-pipeline/tasks/cc/core/logging/factory:logging_factory_hdr",
        "@com_google_absl//absl/base:core_headers",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/log:absl_log",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/synchronization",
        "@litert//tflite/core/api:op_resolver",
    ],
)

cc_test_with_tflite(
    name = "task_runner_test",
    srcs = ["task_runner_test.cc"],
    data = [
        "//cv-cuda-pipeline/tasks/testdata/core:test_models",
    ],
    tflite_deps = [
        ":task_runner",
        ":model_resources",
        ":model_resources_cache",
        "@litert//tflite:test_util",
    ],
    deps = [
        ":running_mode",
        "//cv-cuda-pipeline/calculators/core:pass_through_calculator",
        "//cv-cuda-pipeline/calculators/core:side_packet_to_stream_calculator",
        "//cv-cuda-pipeline/framework:calculator_cc_proto",
        "//cv-cuda-pipeline/framework:packet",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
        "//cv-cuda-pipeline/tasks/cc/core/proto:external_file_cc_proto",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
    ],
)

cc_library_with_tflite(
    name = "base_task_api",
    hdrs = ["base_task_api.h"],
    tflite_deps = [
        ":task_runner",
    ],
    deps = [
        "//cv-cuda-pipeline/calculators/core:flow_limiter_calculator",
    ],
)

cc_library_with_tflite(
    name = "task_api_factory",
    hdrs = ["task_api_factory.h"],
    tflite_deps = [
        ":base_task_api",
        ":model_resources",
        ":task_runner",
    ],
    deps = [
        ":running_mode",
        ":utils",
        "//cv-cuda-pipeline/framework:calculator_cc_proto",
        "//cv-cuda-pipeline/framework:executor",
        "//cv-cuda-pipeline/framework/port:requires",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/tasks/cc:common",
        "//cv-cuda-pipeline/tasks/cc/core/proto:base_options_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/core/proto:external_file_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/core/proto:inference_subgraph_cc_proto",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@litert//tflite/core/api:op_resolver",
    ],
)

cc_library(
    name = "utils",
    srcs = ["utils.cc"],
    hdrs = ["utils.h"],
    visibility = [
        "//cv-cuda-pipeline/calculators:__subpackages__",
        "//cv-cuda-pipeline/tasks:internal",
    ],
    deps = [
        ":external_file_handler",
        "//cv-cuda-pipeline/calculators/core:flow_limiter_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_cc_proto",
        "//cv-cuda-pipeline/framework/api2:builder",
        "//cv-cuda-pipeline/tasks/cc/core/proto:external_file_cc_proto",
        "//cv-cuda-pipeline/tasks/metadata:metadata_schema_cc",
        "@com_google_absl//absl/strings",
        "@flatbuffers//:runtime_cc",
    ],
)

cc_library(
    name = "model_asset_bundle_resources",
    srcs = ["model_asset_bundle_resources.cc"],
    hdrs = ["model_asset_bundle_resources.h"],
    deps = [
        ":external_file_handler",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/tasks/cc:common",
        "//cv-cuda-pipeline/tasks/cc/core/proto:external_file_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/metadata/utils:zip_utils",
        "//cv-cuda-pipeline/tasks/metadata:bundle_manifest_cc_proto",
        "//cv-cuda-pipeline/util:resource_util",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
    ],
)

cc_test(
    name = "model_asset_bundle_resources_test",
    srcs = ["model_asset_bundle_resources_test.cc"],
    data = [
        "//cv-cuda-pipeline/tasks/testdata/core:test_models",
    ],
    deps = [
        ":model_asset_bundle_resources",
        ":model_resources",
        ":utils",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/tasks/cc:common",
        "//cv-cuda-pipeline/tasks/cc/core/proto:external_file_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/metadata/utils:zip_utils",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:cord",
    ],
)

cc_library(
    name = "embedding_provider",
    hdrs = ["embedding_provider.h"],
    deps = [
        "@com_google_absl//absl/status:statusor",
    ],
)

cc_test(
    name = "embedding_provider_test",
    srcs = ["embedding_provider_test.cc"],
    deps = [
        ":embedding_provider",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:status_matchers",
        "@com_google_absl//absl/status:statusor",
    ],
)
