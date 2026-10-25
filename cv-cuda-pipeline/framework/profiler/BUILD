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

licenses(["notice"])

package(default_visibility = ["//cv-cuda-pipeline/framework:__subpackages__"])

# This is used to enable the profiler on platforms where it is not on by default.
# To enable, pass --define MEDIAPIPE_PROFILING=1 to bazel.
config_setting(
    name = "graph_profiler_enabled",
    values = {
        "define": "MEDIAPIPE_PROFILING=1",
    },
    visibility = ["//visibility:public"],
)

config_setting(
    name = "graph_profiler_disabled",
    values = {
        "define": "MEDIAPIPE_PROFILING=0",
    },
    visibility = ["//visibility:public"],
)

# Maps either to graph_profiler_real or to graph_profiler_stub
# depending on the platform, compilation mode, and MEDIAPIPE_PROFILING value.
# This requires three levels of aliases because of limitations with select.
#
alias(
    name = "graph_profiler",
    actual = select({
        "//conditions:default": ":graph_profiler_default_on",
    }),
    visibility = [
        "//cv-cuda-pipeline/framework:__subpackages__",
        "//cv-cuda-pipeline/java:__subpackages__",
    ],
)

alias(
    name = "graph_profiler_default_off",
    actual = select({
        ":graph_profiler_enabled": ":graph_profiler_real",
        "//conditions:default": ":graph_profiler_stub",
    }),
    visibility = ["//visibility:private"],
)

alias(
    name = "graph_profiler_default_on",
    actual = select({
        ":graph_profiler_disabled": ":graph_profiler_stub",
        "//conditions:default": ":graph_profiler_real",
    }),
    visibility = ["//visibility:private"],
)

cc_library(
    name = "graph_profiler_stub",
    hdrs = ["graph_profiler_stub.h"],
    visibility = ["//visibility:private"],
    deps = [
        "//cv-cuda-pipeline/framework:timestamp",
        "//cv-cuda-pipeline/framework/port:status",
    ],
)

cc_library(
    name = "graph_profiler_real",
    srcs = [
        "graph_profiler.cc",
    ] + select({
        "//conditions:default": ["gl_context_profiler.cc"],
        "//cv-cuda-pipeline/gpu:disable_gpu": [],
    }),
    hdrs = [
        "graph_profiler.h",
    ],
    defines = [
        "MEDIAPIPE_PROFILER_AVAILABLE",
    ],
    visibility = ["//visibility:private"],
    deps = [
        ":graph_tracer",
        ":profiler_resource_util",
        ":sharded_map",
        ":trace_buffer",
        ":web_performance_profiling",
        "//cv-cuda-pipeline/framework:calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_context",
        "//cv-cuda-pipeline/framework:calculator_profile_cc_proto",
        "//cv-cuda-pipeline/framework:executor",
        "//cv-cuda-pipeline/framework:validated_graph_config",
        "//cv-cuda-pipeline/framework/deps:clock",
        "//cv-cuda-pipeline/framework/port:advanced_proto_lite",
        "//cv-cuda-pipeline/framework/port:file_helpers",
        "//cv-cuda-pipeline/framework/port:integral_types",
        "//cv-cuda-pipeline/framework/port:re2",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/framework/tool:name_util",
        "//cv-cuda-pipeline/framework/tool:tag_map",
        "//cv-cuda-pipeline/framework/tool:validate_name",
        "@com_google_absl//absl/log:absl_check",
        "@com_google_absl//absl/log:absl_log",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/synchronization",
        "@com_google_absl//absl/time",
        "@com_google_absl//absl/types:optional",
    ] + select({
        "//conditions:default": [],
    }) + select({
        "//conditions:default": [
        ],
        "//cv-cuda-pipeline/gpu:disable_gpu": [],
    }),
)

cc_library(
    name = "circular_buffer",
    hdrs = ["circular_buffer.h"],
    visibility = [
        "//cv-cuda-pipeline:__subpackages__",
    ],
    deps = [
        "//cv-cuda-pipeline/framework/port:integral_types",
    ],
)

cc_test(
    name = "circular_buffer_test",
    size = "small",
    srcs = ["circular_buffer_test.cc"],
    deps = [
        ":circular_buffer",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:threadpool",
        "@com_google_absl//absl/time",
    ],
)

cc_library(
    name = "trace_buffer",
    srcs = ["trace_buffer.h"],
    hdrs = ["trace_buffer.h"],
    visibility = ["//cv-cuda-pipeline/framework/profiler:__subpackages__"],
    deps = [
        ":circular_buffer",
        "//cv-cuda-pipeline/framework:calculator_profile_cc_proto",
        "//cv-cuda-pipeline/framework:packet",
        "//cv-cuda-pipeline/framework:timestamp",
        "@com_google_absl//absl/time",
    ],
)

cc_library(
    name = "graph_tracer",
    srcs = [
        "graph_tracer.cc",
        "trace_builder.cc",
        "trace_builder.h",
    ],
    hdrs = [
        "graph_tracer.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":trace_buffer",
        "//cv-cuda-pipeline/framework:calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_context",
        "//cv-cuda-pipeline/framework:calculator_profile_cc_proto",
        "//cv-cuda-pipeline/framework:input_stream_shard",
        "//cv-cuda-pipeline/framework:output_stream_shard",
        "//cv-cuda-pipeline/framework:packet",
        "//cv-cuda-pipeline/framework:timestamp",
        "//cv-cuda-pipeline/framework/port:integral_types",
        "@com_google_absl//absl/container:node_hash_map",
        "@com_google_absl//absl/synchronization",
        "@com_google_absl//absl/time",
    ],
)

cc_library(
    name = "sharded_map",
    hdrs = ["sharded_map.h"],
    visibility = ["//visibility:private"],
    deps = [
        "@com_google_absl//absl/synchronization",
    ],
)

cc_library(
    name = "test_context_builder",
    testonly = True,
    hdrs = ["test_context_builder.h"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:calculator_options_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_state",
        "//cv-cuda-pipeline/framework:mediapipe_options_cc_proto",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/framework/port:statusor",
        "//cv-cuda-pipeline/framework/tool:tag_map",
        "//cv-cuda-pipeline/framework/tool:tag_map_helper",
        "@com_google_absl//absl/log:absl_log",
        "@com_google_absl//absl/memory",
    ],
)

cc_test(
    name = "graph_tracer_test",
    srcs = ["graph_tracer_test.cc"],
    visibility = ["//visibility:public"],
    deps = [
        ":graph_profiler",
        ":graph_tracer",
        ":test_context_builder",
        "//cv-cuda-pipeline/calculators/core:flow_limiter_calculator",
        "//cv-cuda-pipeline/calculators/core:immediate_mux_calculator",
        "//cv-cuda-pipeline/calculators/core:round_robin_demux_calculator",
        "//cv-cuda-pipeline/calculators/util:annotation_overlay_calculator",
        "//cv-cuda-pipeline/framework:calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:calculator_profile_cc_proto",
        "//cv-cuda-pipeline/framework:test_calculators",
        "//cv-cuda-pipeline/framework/deps:clock",
        "//cv-cuda-pipeline/framework/port:advanced_proto",
        "//cv-cuda-pipeline/framework/port:file_helpers",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:integral_types",
        "//cv-cuda-pipeline/framework/port:logging",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/framework/stream_handler:default_input_stream_handler",
        "//cv-cuda-pipeline/framework/stream_handler:immediate_input_stream_handler",
        "//cv-cuda-pipeline/framework/tool:simulation_clock",
        "//cv-cuda-pipeline/framework/tool:simulation_clock_executor",
        "//cv-cuda-pipeline/framework/tool:status_util",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/log:absl_check",
        "@com_google_absl//absl/time",
    ],
)

cc_test(
    name = "sharded_map_test",
    srcs = ["sharded_map_test.cc"],
    deps = [
        ":sharded_map",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:integral_types",
        "//cv-cuda-pipeline/framework/port:threadpool",
        "@com_google_absl//absl/container:node_hash_map",
        "@com_google_absl//absl/log:absl_log",
        "@com_google_absl//absl/synchronization",
        "@com_google_absl//absl/time",
    ],
)

config_setting(
    name = "mediapipe_web_profiling_enabled",
    values = {
        "define": "MEDIAPIPE_WEB_PROFILING=1",
    },
    visibility = ["//visibility:private"],
)

cc_library(
    name = "web_performance_profiling",
    hdrs = ["web_performance_profiling.h"],
    defines = select({
        ":mediapipe_web_profiling_enabled": ["MEDIAPIPE_WEB_PROFILING_ENABLED"],
        "//conditions:default": [],
    }),
    visibility = ["//cv-cuda-pipeline:__subpackages__"],
    deps = ["@com_google_absl//absl/strings"],
)

cc_library(
    name = "profiler_resource_util",
    srcs = ["profiler_resource_util_common.cc"] + select({
        "//conditions:default": ["profiler_resource_util.cc"],
    }),
    hdrs = ["profiler_resource_util.h"],

    visibility = ["//visibility:private"],
    deps = [
        "//cv-cuda-pipeline/framework/deps:file_path",
        "//cv-cuda-pipeline/framework/port:logging",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/framework/port:statusor",
        "@com_google_absl//absl/flags:flag",
    ] + [
            "//cv-cuda-pipeline/framework/port:file_helpers",
        ],
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
        "graph_profiler.h",
        "graph_profiler_stub.h",
    ],
    visibility = ["//cv-cuda-pipeline/framework:__pkg__"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_profile_cc_proto",
    ],
)

cc_test(
    name = "reporter_test",
    srcs = ["reporter_test.cc"],
    data = [
        "//cv-cuda-pipeline/framework/profiler/testdata:mediapipe_profile_graphs",
    ],
    visibility = ["//visibility:private"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_profile_cc_proto",
        "//cv-cuda-pipeline/framework/deps:file_path",
        "//cv-cuda-pipeline/framework/port:advanced_proto",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
        "//cv-cuda-pipeline/framework/profiler/reporter:reporter_lib",
        "//cv-cuda-pipeline/framework/tool:test_util",
        "@com_google_absl//absl/log:absl_log",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings",
    ],
)
