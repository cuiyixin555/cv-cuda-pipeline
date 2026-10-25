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

load("@bazel_skylib//:bzl_library.bzl", "bzl_library")
load("@rules_cc//cc:cc_library.bzl", "cc_library")
load("@rules_cc//cc:cc_test.bzl", "cc_test")
load("//cv-cuda-pipeline/framework/port:build_config.bzl", "mediapipe_proto_library")
# Placeholder: load proto_library

licenses(["notice"])

package(default_visibility = ["//visibility:private"])

# For direct MediaPipe internal proto usage. Please only add your package
# here if necessary and using
# "//cv-cuda-pipeline/framework:calculator_framework" is not sufficient
# (e.g. auto generated code for serialization etc.)
package_group(
    name = "mediapipe_internal_proto_users",
    packages = [
        "//cv-cuda-pipeline/...",
    ],
)

# The MediaPipe internal package group. No mediapipe users should be added to this group.
package_group(
    name = "mediapipe_internal",
    packages = [
        "//cv-cuda-pipeline/...",
    ],
)

bzl_library(
    name = "transitive_protos_bzl",
    srcs = [
        "transitive_protos.bzl",
    ],
    visibility = [
        "//cv-cuda-pipeline/framework:__subpackages__",
    ],
)

bzl_library(
    name = "encode_binary_proto_bzl",
    srcs = [
        "encode_binary_proto.bzl",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "@bazel_skylib//lib:paths",
    ],
)

alias(
    name = "encode_binary_proto",
    actual = ":encode_binary_proto_bzl",
    deprecation = "Use encode_binary_proto_bzl",
    visibility = ["//visibility:public"],
)

mediapipe_proto_library(
    name = "calculator_proto",
    srcs = ["calculator.proto"],
    visibility = ["//visibility:public"],
    deps = [
        ":calculator_options_proto",
        ":mediapipe_options_proto",
        ":packet_factory_proto",
        ":packet_generator_proto",
        ":status_handler_proto",
        ":stream_handler_proto",
        "@com_google_protobuf//:any_proto",
    ],
)

mediapipe_proto_library(
    name = "calculator_options_proto",
    srcs = ["calculator_options.proto"],
    visibility = ["//visibility:public"],
)

mediapipe_proto_library(
    name = "calculator_contract_test_proto",
    testonly = True,
    srcs = ["calculator_contract_test.proto"],
    visibility = ["//cv-cuda-pipeline/framework:__subpackages__"],
    deps = [
        ":calculator_options_proto",
        ":calculator_proto",
    ],
)

mediapipe_proto_library(
    name = "calculator_profile_proto",
    srcs = ["calculator_profile.proto"],
    visibility = ["//visibility:public"],
    deps = [
        ":calculator_options_proto",
        ":calculator_proto",
    ],
)

mediapipe_proto_library(
    name = "mediapipe_options_proto",
    srcs = ["mediapipe_options.proto"],
    visibility = [
        "//visibility:public",
    ],
)

mediapipe_proto_library(
    name = "packet_factory_proto",
    srcs = ["packet_factory.proto"],
    visibility = ["//visibility:public"],
)

mediapipe_proto_library(
    name = "packet_generator_proto",
    srcs = ["packet_generator.proto"],
    visibility = ["//visibility:public"],
)

mediapipe_proto_library(
    name = "packet_test_proto",
    testonly = True,
    srcs = ["packet_test.proto"],
    visibility = [
        ":mediapipe_internal_proto_users",
        "//cv-cuda-pipeline/framework:__subpackages__",
    ],
)

mediapipe_proto_library(
    name = "status_handler_proto",
    srcs = ["status_handler.proto"],
    visibility = [":mediapipe_internal_proto_users"],
    deps = [":mediapipe_options_proto"],
)

mediapipe_proto_library(
    name = "stream_handler_proto",
    srcs = ["stream_handler.proto"],
    visibility = [":mediapipe_internal_proto_users"],
    deps = [":mediapipe_options_proto"],
)

mediapipe_proto_library(
    name = "test_calculators_proto",
    testonly = True,
    srcs = ["test_calculators.proto"],
    visibility = [":mediapipe_internal_proto_users"],
    deps = [
        ":calculator_options_proto",
        ":calculator_proto",
    ],
)

mediapipe_proto_library(
    name = "thread_pool_executor_proto",
    srcs = ["thread_pool_executor.proto"],
    visibility = ["//visibility:public"],
    deps = [":mediapipe_options_proto"],
)

cc_library(
    name = "calculator_base",
    srcs = ["calculator_base.cc"],
    hdrs = ["calculator_base.h"],
    visibility = [
        ":mediapipe_internal",
    ],
    deps = [
        ":calculator_context",
        ":calculator_contract",
        ":port",
        ":timestamp",
        "//cv-cuda-pipeline/framework/deps:registration",
        "//cv-cuda-pipeline/framework/port:logging",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/status",
    ],
)

cc_library(
    name = "calculator_context",
    srcs = ["calculator_context.cc"],
    hdrs = ["calculator_context.h"],
    visibility = [
        ":mediapipe_internal",
    ],
    deps = [
        ":calculator_state",
        ":collection_item_id",
        ":counter",
        ":counter_factory",
        ":graph_service",
        ":graph_service_manager",
        ":input_stream_shard",
        ":output_stream_shard",
        ":packet_set",
        ":port",
        ":resources",
        ":timestamp",
        "//cv-cuda-pipeline/framework/port:any_proto",
        "//cv-cuda-pipeline/framework/port:status",
        "@com_google_absl//absl/log:absl_check",
        "@com_google_absl//absl/status",
    ],
)

cc_library(
    name = "calculator_context_manager",
    srcs = ["calculator_context_manager.cc"],
    hdrs = ["calculator_context_manager.h"],
    visibility = [":mediapipe_internal"],
    deps = [
        ":calculator_context",
        ":calculator_state",
        ":timestamp",
        "//cv-cuda-pipeline/framework/port:logging",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/framework/tool:tag_map",
        "@com_google_absl//absl/base:core_headers",
        "@com_google_absl//absl/log:absl_check",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/synchronization",
    ],
)

cc_library(
    name = "calculator_contract",
    srcs = ["calculator_contract.cc"],
    hdrs = ["calculator_contract.h"],
    visibility = [
        ":mediapipe_internal",
    ],
    deps = [
        ":calculator_cc_proto",
        ":graph_service",
        ":mediapipe_options_cc_proto",
        ":packet_generator_cc_proto",
        ":packet_type",
        ":port",
        ":status_handler_cc_proto",
        "//cv-cuda-pipeline/framework/port:any_proto",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/framework/tool:options_map",
        "//cv-cuda-pipeline/framework/tool:packet_generator_wrapper_calculator_cc_proto",
        "//cv-cuda-pipeline/framework/tool:tag_map",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings",
    ],
)

cc_library(
    name = "legacy_calculator_support",
    srcs = ["legacy_calculator_support.cc"],
    hdrs = ["legacy_calculator_support.h"],
    visibility = [
        ":mediapipe_internal",
    ],
    deps = [
        ":calculator_context",
        ":calculator_contract",
    ],
)

cc_library(
    name = "calculator_framework",
    hdrs = ["calculator_framework.h"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":calculator_base",
        ":calculator_context",
        ":calculator_contract",
        ":calculator_graph",
        ":calculator_registry",
        ":counter_factory",
        ":input_stream",
        ":output_side_packet",
        ":output_stream",
        ":packet",
        ":packet_generator",
        ":packet_generator_graph",
        ":packet_set",
        ":packet_type",
        ":port",
        ":status_handler",
        ":subgraph",
        ":timestamp",
        ":validated_graph_config",
        "//cv-cuda-pipeline/framework/tool:sink",
        "//cv-cuda-pipeline/framework/tool:status_util",
        "//cv-cuda-pipeline/framework/tool:validate",
        "//cv-cuda-pipeline/framework/tool:validate_name",
    ],
)

cc_library(
    name = "calculator_graph",
    srcs = [
        "calculator_graph.cc",
        "scheduler.cc",
    ],
    hdrs = [
        "calculator_graph.h",
        "scheduler.h",
    ],
    visibility = [
        ":mediapipe_internal",
    ],
    deps = [
        ":calculator_base",
        ":calculator_cc_proto",
        ":calculator_context",
        ":calculator_node",
        ":counter_factory",
        ":delegating_executor",
        ":executor",
        ":graph_output_stream",
        ":graph_runtime_info_cc_proto",
        ":graph_service",
        ":graph_service_manager",
        ":input_stream_manager",
        ":mediapipe_profiling",
        ":output_side_packet_impl",
        ":output_stream_manager",
        ":output_stream_poller",
        ":output_stream_shard",
        ":packet",
        ":packet_generator_graph",
        ":packet_set",
        ":packet_type",
        ":resources_service",
        ":scheduler_queue",
        ":status_handler",
        ":status_handler_cc_proto",
        ":subgraph",
        ":thread_pool_executor",
        ":thread_pool_executor_cc_proto",
        ":timestamp",
        ":validated_graph_config",
        ":vlog_overrides",
        "//cv-cuda-pipeline/framework/deps:clock",
        "//cv-cuda-pipeline/framework/port:logging",
        "//cv-cuda-pipeline/framework/port:map_util",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:source_location",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/framework/tool:fill_packet_set",
        "//cv-cuda-pipeline/framework/tool:graph_runtime_info_logger",
        "//cv-cuda-pipeline/framework/tool:graph_runtime_info_utils",
        "//cv-cuda-pipeline/framework/tool:packet_generator_wrapper_calculator",
        "//cv-cuda-pipeline/framework/tool:status_util",
        "//cv-cuda-pipeline/framework/tool:tag_map",
        "//cv-cuda-pipeline/gpu:gpu_service",
        "//cv-cuda-pipeline/gpu:gpu_shared_data_internal",
        "//cv-cuda-pipeline/gpu:graph_support",
        "//cv-cuda-pipeline/util:cpu_util",
        "@com_google_absl//absl/base:core_headers",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/log:absl_check",
        "@com_google_absl//absl/log:absl_log",
        "@com_google_absl//absl/log:check",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/synchronization",
        "@com_google_absl//absl/time",
    ],
)

cc_library(
    name = "graph_service_manager",
    srcs = ["graph_service_manager.cc"],
    hdrs = ["graph_service_manager.h"],
    visibility = [":mediapipe_internal"],
    deps = [
        ":graph_service",
        ":packet",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "@com_google_absl//absl/log:absl_check",
        "@com_google_absl//absl/status",
    ],
)

cc_test(
    name = "graph_service_manager_test",
    srcs = ["graph_service_manager_test.cc"],
    deps = [
        ":graph_service_manager",
        ":packet",
        "//cv-cuda-pipeline/framework/port:gtest_main",
    ],
)

cc_library(
    name = "calculator_node",
    srcs = ["calculator_node.cc"],
    hdrs = ["calculator_node.h"],
    visibility = [":mediapipe_internal"],
    deps = [
        ":calculator_base",
        ":calculator_cc_proto",
        ":calculator_context",
        ":calculator_context_manager",
        ":calculator_state",
        ":counter_factory",
        ":graph_runtime_info_cc_proto",
        ":graph_service_manager",
        ":input_side_packet_handler",
        ":input_stream_handler",
        ":input_stream_manager",
        ":legacy_calculator_support",
        ":mediapipe_profiling",
        ":output_side_packet_impl",
        ":output_stream_handler",
        ":output_stream_manager",
        ":packet",
        ":packet_set",
        ":packet_type",
        ":port",
        ":stream_handler_cc_proto",
        ":timestamp",
        ":validated_graph_config",
        "//cv-cuda-pipeline/framework/deps:clock",
        "//cv-cuda-pipeline/framework/port:core_proto",
        "//cv-cuda-pipeline/framework/port:integral_types",
        "//cv-cuda-pipeline/framework/port:logging",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:source_location",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/framework/stream_handler:default_input_stream_handler",
        "//cv-cuda-pipeline/framework/stream_handler:in_order_output_stream_handler",
        "//cv-cuda-pipeline/framework/tool:name_util",
        "//cv-cuda-pipeline/framework/tool:status_util",
        "//cv-cuda-pipeline/framework/tool:tag_map",
        "//cv-cuda-pipeline/framework/tool:validate_name",
        "@com_google_absl//absl/base:core_headers",
        "@com_google_absl//absl/cleanup",
        "@com_google_absl//absl/log:absl_check",
        "@com_google_absl//absl/log:absl_log",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/synchronization",
        "@com_google_absl//absl/time",
    ],
)

cc_library(
    name = "calculator_registry",
    hdrs = ["calculator_registry.h"],
    visibility = [
        ":mediapipe_internal",
    ],
    deps = [
        ":calculator_base",
    ],
)

cc_library(
    name = "calculator_runner",
    testonly = True,
    srcs = ["calculator_runner.cc"],
    hdrs = ["calculator_runner.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":calculator_framework",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/framework/tool:sink",
        "@com_google_absl//absl/base:core_headers",
        "@com_google_absl//absl/log:absl_check",
        "@com_google_absl//absl/log:absl_log",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/strings",
    ],
)

cc_library(
    name = "calculator_state",
    srcs = ["calculator_state.cc"],
    hdrs = ["calculator_state.h"],
    visibility = [":mediapipe_internal"],
    deps = [
        ":calculator_cc_proto",
        ":counter",
        ":counter_factory",
        ":graph_service",
        ":graph_service_manager",
        ":packet",
        ":packet_set",
        ":port",
        ":resources",
        ":resources_service",
        "//cv-cuda-pipeline/framework/port:any_proto",
        "//cv-cuda-pipeline/framework/port:logging",
        "//cv-cuda-pipeline/framework/tool:options_map",
        "@com_google_absl//absl/base:core_headers",
        "@com_google_absl//absl/log:absl_check",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings",
    ],
)

cc_library(
    name = "camera_intrinsics",
    hdrs = ["camera_intrinsics.h"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "collection",
    hdrs = ["collection.h"],
    visibility = [":mediapipe_internal"],
    deps = [
        ":collection_item_id",
        ":type_map",
        "//cv-cuda-pipeline/framework/tool:tag_map",
        "//cv-cuda-pipeline/framework/tool:tag_map_helper",
        "//cv-cuda-pipeline/framework/tool:validate_name",
        "@com_google_absl//absl/base:core_headers",
        "@com_google_absl//absl/log:absl_check",
        "@com_google_absl//absl/log:absl_log",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/strings",
    ],
)

cc_library(
    name = "collection_item_id",
    srcs = ["collection_item_id.cc"],
    hdrs = ["collection_item_id.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//cv-cuda-pipeline/framework/deps:intops",
    ],
)

cc_library(
    name = "counter",
    hdrs = ["counter.h"],
    visibility = ["//visibility:public"],
    deps = ["//cv-cuda-pipeline/framework/port:integral_types"],
)

cc_library(
    name = "counter_factory",
    srcs = ["counter_factory.cc"],
    hdrs = ["counter_factory.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":counter",
        ":port",
        "//cv-cuda-pipeline/framework/port:integral_types",
        "//cv-cuda-pipeline/framework/port:map_util",
        "@com_google_absl//absl/base:core_headers",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/log:absl_check",
        "@com_google_absl//absl/log:absl_log",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/synchronization",
        "@com_google_absl//absl/time",
    ],
)

cc_library(
    name = "delegating_executor",
    srcs = ["delegating_executor.cc"],
    hdrs = ["delegating_executor.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":executor",
    ],
)

cc_library(
    name = "demangle",
    hdrs = ["demangle.h"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "mediapipe_profiling",
    hdrs = [
        "mediapipe_profiling.h",
        "platform_specific_profiling.h",
    ],
    visibility = [
        ":mediapipe_internal",
    ],
    deps = [
        "//cv-cuda-pipeline/framework/profiler:graph_profiler",
    ] + select({
        "//conditions:default": [],
    }),
)

cc_library(
    name = "executor",
    srcs = ["executor.cc"],
    hdrs = ["executor.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":mediapipe_options_cc_proto",
        "//cv-cuda-pipeline/framework/deps:registration",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/framework/port:statusor",
    ],
)

cc_library(
    name = "graph_output_stream",
    srcs = ["graph_output_stream.cc"],
    hdrs = ["graph_output_stream.h"],
    visibility = [
        ":mediapipe_internal",
    ],
    deps = [
        ":collection_item_id",
        ":input_stream_handler",
        ":input_stream_manager",
        ":output_stream_manager",
        ":packet",
        ":packet_set",
        ":packet_type",
        ":timestamp",
        "//cv-cuda-pipeline/framework/port:core_proto",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/framework/tool:tag_map",
        "@com_google_absl//absl/base:core_headers",
        "@com_google_absl//absl/log:absl_check",
        "@com_google_absl//absl/log:absl_log",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/synchronization",
        "@com_google_protobuf//:protobuf",
    ],
)

mediapipe_proto_library(
    name = "graph_runtime_info_proto",
    srcs = ["graph_runtime_info.proto"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "graph_service",
    hdrs = ["graph_service.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":packet",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/framework/port:statusor",
        "@com_google_absl//absl/base:core_headers",
        "@com_google_absl//absl/log:absl_check",
        "@com_google_absl//absl/strings",
    ],
)

cc_library(
    name = "resources",
    srcs = ["resources.cc"],
    hdrs = ["resources.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//cv-cuda-pipeline/framework/deps:mlock_helpers",
        "//cv-cuda-pipeline/framework/deps:mmapped_file",
        "//cv-cuda-pipeline/framework/port:file_helpers",
        "//cv-cuda-pipeline/framework/port:logging",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/framework/tool:status_util",
        "//cv-cuda-pipeline/util:resource_util",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/log:absl_log",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:string_view",
    ],
)

cc_test(
    name = "resources_test",
    srcs = ["resources_test.cc"],
    data = [
        "//cv-cuda-pipeline/framework/testdata:resource_calculator.data",
    ],
    deps = [
        ":resources",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:status_matchers",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings:string_view",
    ],
)

cc_library(
    name = "resources_service",
    hdrs = ["resources_service.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":graph_service",
        ":resources",
    ],
)

cc_test(
    name = "calculator_graph_resources_test",
    srcs = ["calculator_graph_resources_test.cc"],
    data = [
        "//cv-cuda-pipeline/framework/testdata:resource_calculator.data",
        "//cv-cuda-pipeline/framework/testdata:resource_subgraph.data",
    ],
    deps = [
        ":calculator_cc_proto",
        ":calculator_framework",
        ":resources",
        ":resources_service",
        "//cv-cuda-pipeline/calculators/core:constant_side_packet_calculator",
        "//cv-cuda-pipeline/calculators/core:constant_side_packet_calculator_cc_proto",
        "//cv-cuda-pipeline/framework/api2:builder",
        "//cv-cuda-pipeline/framework/api2:node",
        "//cv-cuda-pipeline/framework/api2:packet",
        "//cv-cuda-pipeline/framework/api2:port",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/framework/port:status_matchers",
        "//cv-cuda-pipeline/framework/testdata:resource_path_cc_proto",
        "@com_google_absl//absl/log:absl_check",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
    ],
)

cc_library(
    name = "input_side_packet_handler",
    srcs = ["input_side_packet_handler.cc"],
    hdrs = ["input_side_packet_handler.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":collection_item_id",
        ":packet",
        ":packet_set",
        ":packet_type",
        "//cv-cuda-pipeline/framework/port:logging",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/framework/tool:fill_packet_set",
        "@com_google_absl//absl/log:absl_check",
    ],
)

cc_library(
    name = "input_stream",
    hdrs = ["input_stream.h"],
    visibility = [":mediapipe_internal"],
    deps = [
        ":packet",
        ":port",
        "@com_google_absl//absl/base:core_headers",
    ],
)

cc_library(
    name = "input_stream_handler",
    srcs = ["input_stream_handler.cc"],
    hdrs = ["input_stream_handler.h"],
    visibility = [
        ":mediapipe_internal",
    ],
    deps = [
        ":calculator_context",
        ":calculator_context_manager",
        ":collection",
        ":collection_item_id",
        ":input_stream_manager",
        ":input_stream_shard",
        ":mediapipe_options_cc_proto",
        ":mediapipe_profiling",
        ":packet",
        ":packet_set",
        ":packet_type",
        "//cv-cuda-pipeline/framework/deps:registration",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/framework/tool:tag_map",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/log:absl_check",
        "@com_google_absl//absl/log:absl_log",
        "@com_google_absl//absl/strings",
    ],
)

cc_library(
    name = "input_stream_manager",
    srcs = ["input_stream_manager.cc"],
    hdrs = ["input_stream_manager.h"],
    visibility = [":mediapipe_internal"],
    deps = [
        ":packet",
        ":packet_type",
        ":port",
        ":timestamp",
        "//cv-cuda-pipeline/framework/port:logging",
        "//cv-cuda-pipeline/framework/port:source_location",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/framework/tool:status_util",
        "@com_google_absl//absl/base:core_headers",
        "@com_google_absl//absl/log:absl_check",
        "@com_google_absl//absl/log:absl_log",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_absl//absl/synchronization",
    ],
)

cc_library(
    name = "input_stream_shard",
    srcs = ["input_stream_shard.cc"],
    hdrs = ["input_stream_shard.h"],
    visibility = [":mediapipe_internal"],
    deps = [
        ":input_stream",
        ":packet",
        ":packet_type",
        ":port",
        ":timestamp",
        "//cv-cuda-pipeline/framework/port:source_location",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/framework/tool:status_util",
        "@com_google_absl//absl/log:absl_check",
        "@com_google_absl//absl/strings",
    ],
)

cc_library(
    name = "lifetime_tracker",
    testonly = True,
    hdrs = ["lifetime_tracker.h"],
    visibility = ["//visibility:public"],
    deps = [
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/synchronization",
    ],
)


cc_library(
    name = "output_side_packet",
    hdrs = ["output_side_packet.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":packet",
    ],
)

cc_library(
    name = "output_side_packet_impl",
    srcs = ["output_side_packet_impl.cc"],
    hdrs = ["output_side_packet_impl.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":collection_item_id",
        ":input_side_packet_handler",
        ":output_side_packet",
        ":packet",
        ":packet_type",
        ":port",
        "//cv-cuda-pipeline/framework/port:logging",
        "//cv-cuda-pipeline/framework/port:source_location",
        "//cv-cuda-pipeline/framework/port:status",
        "@com_google_absl//absl/log:absl_check",
    ],
)

cc_library(
    name = "output_stream",
    hdrs = ["output_stream.h"],
    visibility = [":mediapipe_internal"],
    deps = [
        ":packet",
        ":port",
        ":timestamp",
        "//cv-cuda-pipeline/framework/port:logging",
        "@com_google_absl//absl/base:core_headers",
    ],
)

cc_library(
    name = "output_stream_handler",
    srcs = ["output_stream_handler.cc"],
    hdrs = ["output_stream_handler.h"],
    visibility = [
        ":mediapipe_internal",
    ],
    deps = [
        ":calculator_context_manager",
        ":collection",
        ":collection_item_id",
        ":mediapipe_options_cc_proto",
        ":output_stream_manager",
        ":output_stream_shard",
        ":packet_set",
        ":timestamp",
        "//cv-cuda-pipeline/framework/deps:registration",
        "//cv-cuda-pipeline/framework/port:logging",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/framework/tool:tag_map",
        "@com_google_absl//absl/base:core_headers",
        "@com_google_absl//absl/log:absl_check",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/synchronization",
    ],
)

cc_library(
    name = "output_stream_manager",
    srcs = ["output_stream_manager.cc"],
    hdrs = ["output_stream_manager.h"],
    visibility = [":mediapipe_internal"],
    deps = [
        ":input_stream_handler",
        ":output_stream_shard",
        ":packet",
        ":packet_type",
        ":port",
        ":timestamp",
        "//cv-cuda-pipeline/framework/port:status",
        "@com_google_absl//absl/base:core_headers",
        "@com_google_absl//absl/log:absl_check",
        "@com_google_absl//absl/log:absl_log",
        "@com_google_absl//absl/synchronization",
    ],
)

cc_library(
    name = "output_stream_poller",
    hdrs = ["output_stream_poller.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":graph_output_stream",
        "@com_google_absl//absl/log:absl_check",
    ],
)

cc_library(
    name = "output_stream_shard",
    srcs = ["output_stream_shard.cc"],
    hdrs = ["output_stream_shard.h"],
    visibility = [":mediapipe_internal"],
    deps = [
        ":output_stream",
        ":packet",
        ":packet_type",
        ":port",
        ":timestamp",
        "//cv-cuda-pipeline/framework/port:source_location",
        "//cv-cuda-pipeline/framework/port:status",
        "@com_google_absl//absl/log:absl_check",
        "@com_google_absl//absl/strings",
    ],
)

# Defines Packet, a data carrier used throughout the framework.
cc_library(
    name = "packet",
    srcs = ["packet.cc"],
    hdrs = ["packet.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":port",
        ":timestamp",
        ":type_map",
        "//cv-cuda-pipeline/framework/deps:no_destructor",
        "//cv-cuda-pipeline/framework/deps:registration",
        "//cv-cuda-pipeline/framework/port:core_proto",
        "//cv-cuda-pipeline/framework/port:logging",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/framework/port:statusor",
        "//cv-cuda-pipeline/framework/tool:type_util",
        "@com_google_absl//absl/base:core_headers",
        "@com_google_absl//absl/functional:any_invocable",
        "@com_google_absl//absl/log:absl_check",
        "@com_google_absl//absl/log:absl_log",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/synchronization",
        "@com_google_protobuf//:protobuf",
    ],
)

cc_library(
    name = "packet_generator",
    hdrs = ["packet_generator.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":packet_generator_cc_proto",
        ":packet_set",
        ":packet_type",
        ":port",
        "//cv-cuda-pipeline/framework/deps:registration",
        "//cv-cuda-pipeline/framework/port:status",
        "@com_google_absl//absl/base:core_headers",
    ],
)

cc_library(
    name = "packet_generator_graph",
    srcs = ["packet_generator_graph.cc"],
    hdrs = ["packet_generator_graph.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":delegating_executor",
        ":executor",
        ":packet",
        ":packet_factory_cc_proto",
        ":packet_generator",
        ":packet_generator_cc_proto",
        ":packet_type",
        ":port",
        ":thread_pool_executor",
        ":validated_graph_config",
        "//cv-cuda-pipeline/framework/port:core_proto",
        "//cv-cuda-pipeline/framework/port:logging",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/framework/tool:status_util",
        "@com_google_absl//absl/base:core_headers",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/synchronization",
    ],
)

cc_library(
    name = "packet_set",
    hdrs = ["packet_set.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":collection",
        ":packet",
    ],
)

cc_library(
    name = "packet_type",
    srcs = ["packet_type.cc"],
    hdrs = ["packet_type.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":collection",
        ":packet",
        ":packet_set",
        ":type_map",
        "//cv-cuda-pipeline/framework/deps:no_destructor",
        "//cv-cuda-pipeline/framework/port:logging",
        "//cv-cuda-pipeline/framework/port:map_util",
        "//cv-cuda-pipeline/framework/port:source_location",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/framework/tool:status_util",
        "//cv-cuda-pipeline/framework/tool:type_util",
        "//cv-cuda-pipeline/framework/tool:validate_name",
        "@com_google_absl//absl/base:core_headers",
        "@com_google_absl//absl/log:absl_check",
        "@com_google_absl//absl/log:absl_log",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/types:span",
        "@com_google_absl//absl/types:variant",
    ],
)

# When --copt=-fno-rtti is set, MEDIAPIPE_HAS_RTTI is cleared in port.h.
# To explicitly clear MEDIAPIPE_HAS_RTTI, compile with:
#   bazel build --define=disable_rtti_and_exceptions=true
config_setting(
    name = "disable_rtti_and_exceptions",
    define_values = {"disable_rtti_and_exceptions": "true"},
    visibility = ["//visibility:public"],
)

cc_library(
    name = "port",
    hdrs = ["port.h"],
    defines = select({
                  "//conditions:default": [],
              }) + select({
                  "//conditions:default": [],
                  "//cv-cuda-pipeline/gpu:disable_gpu": ["MEDIAPIPE_DISABLE_GPU=1"],
              }) +
              select({
                  "//conditions:default": [],
                  "//cv-cuda-pipeline/framework/port:disable_opencv": ["MEDIAPIPE_DISABLE_OPENCV=1"],
              }) + select({
        "//conditions:default": [],
        # TODO: Improve this. This only sets MEDIAPIPE_DISABLE_OPENCV as a "defines" Make
        # value, not as a bazel "--define" variable, which has effects in C++ code but not in
        # select() statements.
        "//cv-cuda-pipeline/framework/port:enable_halide": [
            "MEDIAPIPE_ENABLE_HALIDE=1",
            "MEDIAPIPE_DISABLE_OPENCV=1",
        ],
    }) + select({
        "//conditions:default": [],
        "//cv-cuda-pipeline/framework:disable_rtti_and_exceptions": [
            "MEDIAPIPE_HAS_RTTI=0",
        ],
    }),
    visibility = [
        "//cv-cuda-pipeline/calculators:__subpackages__",
        "//cv-cuda-pipeline/framework:__subpackages__",
        "//cv-cuda-pipeline/framework/port:__pkg__",
        "//cv-cuda-pipeline/gpu:__pkg__",
        "//cv-cuda-pipeline/tasks:__subpackages__",
        "//cv-cuda-pipeline/util:__subpackages__",
    ],
)

cc_library(
    name = "scheduler_queue",
    srcs = ["scheduler_queue.cc"],
    hdrs = [
        "scheduler_queue.h",
        "scheduler_shared.h",
    ],
    copts = [],
    visibility = [":mediapipe_internal"],
    deps = [
        ":calculator_context",
        ":calculator_node",
        ":executor",
        "//cv-cuda-pipeline/framework/deps:clock",
        "//cv-cuda-pipeline/framework/port:logging",
        "//cv-cuda-pipeline/framework/port:status",
        "@com_google_absl//absl/base:core_headers",
        "@com_google_absl//absl/log:absl_check",
        "@com_google_absl//absl/log:absl_log",
        "@com_google_absl//absl/strings:string_view",
        "@com_google_absl//absl/synchronization",
    ],
)

cc_library(
    name = "status_handler",
    hdrs = ["status_handler.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":mediapipe_options_cc_proto",
        ":packet_set",
        ":packet_type",
        ":port",
        "//cv-cuda-pipeline/framework/deps:registration",
        "//cv-cuda-pipeline/framework/port:status",
    ],
)

cc_library(
    name = "subgraph",
    srcs = ["subgraph.cc"],
    hdrs = ["subgraph.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":calculator_cc_proto",
        ":graph_service",
        ":graph_service_manager",
        ":resources",
        ":resources_service",
        "//cv-cuda-pipeline/framework/deps:registration",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/tool:calculator_graph_template_cc_proto",
        "//cv-cuda-pipeline/framework/tool:options_util",
        "//cv-cuda-pipeline/framework/tool:template_expander",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
    ],
)

cc_library(
    name = "test_calculators",
    testonly = True,
    srcs = ["test_calculators.cc"],
    visibility = ["//visibility:public"],
    deps = [
        ":calculator_framework",
        ":test_calculators_cc_proto",
        "//cv-cuda-pipeline/framework/deps:mathutil",
        "//cv-cuda-pipeline/framework/formats:matrix",
        "//cv-cuda-pipeline/framework/port:integral_types",
        "//cv-cuda-pipeline/framework/port:logging",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "@com_google_absl//absl/base:core_headers",
        "@com_google_absl//absl/log:absl_check",
        "@com_google_absl//absl/strings",
        "@eigen//:eigen3",
    ],
    alwayslink = 1,
)

cc_library(
    name = "test_service",
    testonly = True,
    srcs = ["test_service.cc"],
    hdrs = ["test_service.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":calculator_contract",
        ":calculator_framework",
        ":graph_service",
        "//cv-cuda-pipeline/framework/port:logging",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
    ],
    alwayslink = True,  # Defines TestServiceCalculator
)

cc_library(
    name = "thread_pool_executor",
    srcs = ["thread_pool_executor.cc"],
    hdrs = ["thread_pool_executor.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":executor",
        ":thread_pool_executor_cc_proto",
        "//cv-cuda-pipeline/framework/deps:thread_options",
        "//cv-cuda-pipeline/framework/port:logging",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/framework/port:statusor",
        "//cv-cuda-pipeline/framework/port:threadpool",
        "//cv-cuda-pipeline/util:cpu_util",
    ],
)

cc_library(
    name = "timestamp",
    srcs = ["timestamp.cc"],
    hdrs = ["timestamp.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//cv-cuda-pipeline/framework/deps:intops",
        "//cv-cuda-pipeline/framework/port:integral_types",
        "//cv-cuda-pipeline/framework/port:logging",
        "@com_google_absl//absl/base:core_headers",
        "@com_google_absl//absl/log:absl_check",
        "@com_google_absl//absl/log:absl_log",
        "@com_google_absl//absl/strings",
    ],
)

cc_library(
    name = "throttler",
    hdrs = ["throttler.h"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "type_map",
    hdrs = ["type_map.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":demangle",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/framework/tool:status_util",
        "//cv-cuda-pipeline/framework/tool:type_util",
        "@com_google_absl//absl/base:core_headers",
        "@com_google_absl//absl/log:absl_check",
        "@com_google_absl//absl/log:absl_log",
        "@com_google_absl//absl/synchronization",
    ],
    alwayslink = 1,
)

cc_library(
    name = "basic_types_registration",
    srcs = ["basic_types_registration.cc"],
    visibility = ["//visibility:public"],
    deps = [
        ":type_map",
        "//cv-cuda-pipeline/framework/port:integral_types",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/strings",
    ],
    alwayslink = 1,
)

cc_library(
    name = "validated_graph_config",
    srcs = ["validated_graph_config.cc"],
    hdrs = ["validated_graph_config.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":calculator_base",
        ":calculator_cc_proto",
        ":calculator_contract",
        ":graph_service_manager",
        ":legacy_calculator_support",
        ":packet_generator",
        ":packet_generator_cc_proto",
        ":packet_type",
        ":port",
        ":status_handler",
        ":status_handler_cc_proto",
        ":stream_handler_cc_proto",
        ":subgraph",
        ":thread_pool_executor_cc_proto",
        ":vlog_utils",
        "//cv-cuda-pipeline/framework/port:core_proto",
        "//cv-cuda-pipeline/framework/port:file_helpers",
        "//cv-cuda-pipeline/framework/port:logging",
        "//cv-cuda-pipeline/framework/port:map_util",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:source_location",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/framework/port:topologicalsorter",
        "//cv-cuda-pipeline/framework/tool:name_util",
        "//cv-cuda-pipeline/framework/tool:status_util",
        "//cv-cuda-pipeline/framework/tool:subgraph_expansion",
        "//cv-cuda-pipeline/framework/tool:validate_name",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/log:absl_check",
        "@com_google_absl//absl/log:absl_log",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings",
        "@com_google_protobuf//:protobuf",
    ],
)

cc_test(
    name = "validated_graph_config_test",
    srcs = ["validated_graph_config_test.cc"],
    deps = [
        ":calculator_cc_proto",
        ":calculator_framework",
        ":graph_service",
        ":graph_service_manager",
        ":validated_graph_config",
        "//cv-cuda-pipeline/framework/api2:node",
        "//cv-cuda-pipeline/framework/api2:port",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings",
    ],
)

cc_library(
    name = "graph_validation",
    hdrs = ["graph_validation.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":calculator_framework",
        "//cv-cuda-pipeline/framework/port:status",
    ],
)

# cc tests
cc_test(
    name = "calculator_base_test",
    size = "medium",
    srcs = ["calculator_base_test.cc"],
    linkstatic = 1,
    deps = [
        ":calculator_base",
        ":calculator_cc_proto",
        ":calculator_context",
        ":calculator_context_manager",
        ":calculator_registry",
        ":calculator_state",
        ":output_stream",
        ":output_stream_manager",
        ":output_stream_shard",
        ":packet_set",
        ":packet_type",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/framework/tool:status_util",
        "//cv-cuda-pipeline/framework/tool:tag_map_helper",
        "@com_google_absl//absl/container:flat_hash_set",
    ],
)

cc_test(
    name = "calculator_contract_test",
    srcs = ["calculator_contract_test.cc"],
    linkstatic = 1,
    deps = [
        ":calculator_cc_proto",
        ":calculator_contract",
        ":calculator_contract_test_cc_proto",
        ":packet_generator_cc_proto",
        ":status_handler_cc_proto",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
    ],
)

cc_test(
    name = "calculator_node_test",
    size = "small",
    srcs = ["calculator_node_test.cc"],
    linkstatic = 1,
    deps = [
        ":calculator_framework",
        ":calculator_node",
        ":output_side_packet_impl",
        "//cv-cuda-pipeline/calculators/core:pass_through_calculator",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/framework/tool:source",
        "@com_google_absl//absl/log:absl_check",
        "@com_google_absl//absl/log:absl_log",
    ],
)

cc_test(
    name = "calculator_graph_event_loop_test",
    size = "small",
    srcs = ["calculator_graph_event_loop_test.cc"],
    deps = [
        ":calculator_framework",
        ":calculator_graph",
        "//cv-cuda-pipeline/calculators/core:pass_through_calculator",
        "//cv-cuda-pipeline/framework/port:core_proto",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:integral_types",
        "//cv-cuda-pipeline/framework/port:logging",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/framework/tool:sink",
        "//cv-cuda-pipeline/framework/tool:status_util",
        "@com_google_absl//absl/base:core_headers",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/synchronization",
        "@com_google_absl//absl/time",
    ],
)

cc_test(
    name = "calculator_graph_stopping_test",
    size = "small",
    srcs = ["calculator_graph_stopping_test.cc"],
    deps = [
        ":calculator_framework",
        ":calculator_graph",
        "//cv-cuda-pipeline/calculators/core:pass_through_calculator",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:logging",
        "//cv-cuda-pipeline/framework/tool:sink",
        "//cv-cuda-pipeline/framework/tool:status_util",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/synchronization",
    ],
)

cc_test(
    name = "calculator_parallel_execution_test",
    srcs = ["calculator_parallel_execution_test.cc"],
    deps = [
        ":calculator_framework",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:integral_types",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/framework/tool:sink",
        "@com_google_absl//absl/base:core_headers",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/synchronization",
        "@com_google_absl//absl/time",
    ],
)

cc_test(
    name = "calculator_graph_summary_packet_test",
    srcs = ["calculator_graph_summary_packet_test.cc"],
    deps = [
        ":calculator_framework",
        ":packet",
        "//cv-cuda-pipeline/framework/api2:node",
        "//cv-cuda-pipeline/framework/api2:packet",
        "//cv-cuda-pipeline/framework/api2:port",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
        "//cv-cuda-pipeline/framework/stream_handler:immediate_input_stream_handler",
        "//cv-cuda-pipeline/framework/tool:sink",
        "@com_google_absl//absl/status",
    ],
)

cc_test(
    name = "calculator_graph_error_callback_test",
    srcs = ["calculator_graph_error_callback_test.cc"],
    deps = [
        ":calculator_framework",
        ":packet",
        "//cv-cuda-pipeline/framework/api2:node",
        "//cv-cuda-pipeline/framework/api2:port",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
        "@com_google_absl//absl/log:absl_check",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/synchronization",
        "@com_google_absl//absl/time",
    ],
)

cc_test(
    name = "calculator_runner_test",
    size = "medium",
    srcs = ["calculator_runner_test.cc"],
    deps = [
        ":calculator_base",
        ":calculator_registry",
        ":calculator_runner",
        ":input_stream",
        ":output_stream",
        ":packet_type",
        ":timestamp",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:status",
        "@com_google_absl//absl/log:absl_log",
        "@com_google_absl//absl/strings",
    ],
)

cc_test(
    name = "calculator_context_test",
    size = "medium",
    srcs = ["calculator_context_test.cc"],
    data = ["//cv-cuda-pipeline/framework/testdata:resource_calculator.data"],
    linkstatic = 1,
    deps = [
        ":calculator_cc_proto",
        ":calculator_context",
        ":calculator_state",
        ":resources",
        "//cv-cuda-pipeline/framework/deps:no_destructor",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
        "//cv-cuda-pipeline/framework/testdata:night_light_calculator_cc_proto",
        "//cv-cuda-pipeline/framework/testdata:sky_light_calculator_cc_proto",
        "//cv-cuda-pipeline/framework/tool:tag_map_helper",
        "@com_google_absl//absl/strings:string_view",
    ],
)


cc_test(
    name = "calculator_graph_bounds_test",
    size = "small",
    srcs = [
        "calculator_graph_bounds_test.cc",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":calculator_context",
        ":calculator_framework",
        ":test_calculators",
        ":thread_pool_executor",
        ":timestamp",
        ":type_map",
        "//cv-cuda-pipeline/calculators/core:counting_source_calculator",
        "//cv-cuda-pipeline/calculators/core:mux_calculator",
        "//cv-cuda-pipeline/calculators/core:pass_through_calculator",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:logging",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/framework/stream_handler:barrier_input_stream_handler",
        "//cv-cuda-pipeline/framework/stream_handler:early_close_input_stream_handler",
        "//cv-cuda-pipeline/framework/stream_handler:fixed_size_input_stream_handler",
        "//cv-cuda-pipeline/framework/stream_handler:immediate_input_stream_handler",
        "//cv-cuda-pipeline/framework/stream_handler:mux_input_stream_handler",
        "//cv-cuda-pipeline/framework/stream_handler:sync_set_input_stream_handler",
        "//cv-cuda-pipeline/framework/tool:sink",
        "//cv-cuda-pipeline/util:packet_test_util",
        "@com_google_absl//absl/strings",
    ],
)

cc_test(
    name = "calculator_graph_side_packet_test",
    size = "small",
    srcs = [
        "calculator_graph_side_packet_test.cc",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":calculator_cc_proto",
        ":calculator_framework",
        ":test_calculators",
        "//cv-cuda-pipeline/calculators/core:counting_source_calculator",
        "//cv-cuda-pipeline/calculators/core:mux_calculator",
        "//cv-cuda-pipeline/calculators/core:pass_through_calculator",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/framework/tool:sink",
        "@com_google_absl//absl/log:absl_log",
        "@com_google_absl//absl/time",
    ],
)

cc_test(
    name = "collection_test",
    size = "small",
    srcs = ["collection_test.cc"],
    linkstatic = 1,
    deps = [
        ":collection",
        ":packet_set",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/framework/tool:tag_map_helper",
        "@com_google_absl//absl/strings",
    ],
)

cc_test(
    name = "graph_service_test",
    size = "small",
    srcs = [
        "graph_service_test.cc",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":calculator_contract",
        ":calculator_framework",
        ":graph_service",
        ":test_service",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:logging",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/framework/port:status_matchers",
        "//cv-cuda-pipeline/framework/tool:sink",
    ],
)

cc_test(
    name = "input_stream_manager_test",
    size = "small",
    srcs = ["input_stream_manager_test.cc"],
    linkstatic = 1,
    deps = [
        ":input_stream_manager",
        ":input_stream_shard",
        ":lifetime_tracker",
        ":packet",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "@com_google_absl//absl/memory",
    ],
)

cc_test(
    name = "output_stream_manager_test",
    size = "small",
    srcs = ["output_stream_manager_test.cc"],
    linkstatic = 1,
    deps = [
        ":input_stream_handler",
        ":input_stream_manager",
        ":output_stream_manager",
        ":output_stream_shard",
        ":packet",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/stream_handler:default_input_stream_handler",
        "//cv-cuda-pipeline/framework/tool:tag_map_helper",
        "@com_google_absl//absl/memory",
    ],
)

cc_test(
    name = "packet_delete_test",
    size = "small",
    srcs = ["packet_delete_test.cc"],
    copts = [
        "-Werror",
    ],
    linkstatic = 1,
    deps = [
        ":packet",
        "//cv-cuda-pipeline/framework/port:gtest_main",
    ],
)

cc_test(
    name = "executor_external_build_test",
    size = "small",
    srcs = ["executor_external_build_test.cc"],
    linkstatic = 1,
    deps = [
        ":executor",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:threadpool",
        "@com_google_absl//absl/memory",
    ],
)

cc_test(
    name = "packet_test",
    size = "medium",
    srcs = ["packet_test.cc"],
    linkstatic = 1,
    deps = [
        ":calculator_framework",
        ":packet",
        ":packet_test_cc_proto",
        ":type_map",
        "//cv-cuda-pipeline/framework/port:core_proto",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:statusor",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_protobuf//:protobuf",
    ],
)

cc_test(
    name = "packet_registration_test",
    size = "small",
    srcs = ["packet_registration_test.cc"],
    deps = [
        ":calculator_framework",
        ":packet",
        ":packet_test_cc_proto",
        ":type_map",
        "//cv-cuda-pipeline/framework/api2:builder",
        "//cv-cuda-pipeline/framework/port:core_proto",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "@com_google_absl//absl/strings",
    ],
)

cc_test(
    name = "packet_generator_test",
    size = "small",
    srcs = ["packet_generator_test.cc"],
    deps = [
        ":packet_generator",
        ":packet_generator_cc_proto",
        ":packet_type",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/tool:validate_type",
        "@com_google_absl//absl/strings",
    ],
)

cc_test(
    name = "timestamp_test",
    size = "small",
    srcs = ["timestamp_test.cc"],
    linkstatic = 1,
    deps = [
        ":timestamp",
        "//cv-cuda-pipeline/framework/port:gtest_main",
    ],
)

cc_test(
    name = "graph_validation_test",
    srcs = ["graph_validation_test.cc"],
    deps = [
        ":calculator_contract_test_cc_proto",
        ":calculator_framework",
        ":graph_validation",
        "//cv-cuda-pipeline/calculators/core:constant_side_packet_calculator",
        "//cv-cuda-pipeline/calculators/core:default_side_packet_calculator",
        "//cv-cuda-pipeline/calculators/core:pass_through_calculator",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
        "//cv-cuda-pipeline/framework/tool:template_parser",
        "@com_google_absl//absl/log:absl_check",
    ],
)

cc_test(
    name = "subgraph_test",
    srcs = ["subgraph_test.cc"],
    data = ["//cv-cuda-pipeline/framework/testdata:resource_subgraph.data"],
    deps = [
        ":calculator_framework",
        ":graph_service",
        ":resources",
        ":subgraph",
        ":test_calculators",
        "//cv-cuda-pipeline/calculators/core:constant_side_packet_calculator",
        "//cv-cuda-pipeline/calculators/core:constant_side_packet_calculator_cc_proto",
        "//cv-cuda-pipeline/calculators/core:pass_through_calculator",
        "//cv-cuda-pipeline/framework/api2:builder",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/framework/tool:sink",
        "//cv-cuda-pipeline/framework/tool/testdata:dub_quad_test_subgraph",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
    ],
)

cc_library(
    name = "memory_manager",
    hdrs = ["memory_manager.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":port",
        "//cv-cuda-pipeline/gpu:multi_pool",
    ] + select({
        "//conditions:default": [],
    }),
)

cc_library(
    name = "memory_manager_service",
    hdrs = ["memory_manager_service.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":graph_service",
        ":memory_manager",
    ],
)

cc_library(
    name = "vlog_overrides",
    srcs = ["vlog_overrides.cc"],
    hdrs = ["vlog_overrides.h"],
    visibility = ["//visibility:private"],
    deps = [
        "//cv-cuda-pipeline/framework/deps:no_destructor",
        "@com_google_absl//absl/log:absl_check",
        "@com_google_absl//absl/log:absl_log",
        "@com_google_absl//absl/log:globals",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_absl//absl/strings:string_view",
    ],
)

cc_library(
    name = "vlog_utils",
    srcs = ["vlog_utils.cc"],
    hdrs = ["vlog_utils.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//cv-cuda-pipeline/framework/port:logging",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/log:absl_log",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:string_view",
    ],
)
