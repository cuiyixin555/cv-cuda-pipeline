load("@rules_cc//cc:cc_library.bzl", "cc_library")
load("@rules_cc//cc:cc_test.bzl", "cc_test")
load("//cv-cuda-pipeline/framework:mediapipe_cc_test.bzl", "mediapipe_cc_test")

package(default_visibility = ["//cv-cuda-pipeline:__subpackages__"] + [
])

licenses(["notice"])

cc_library(
    name = "calculator_context",
    hdrs = ["calculator_context.h"],
    deps = [
        ":contract",
        ":one_of",
        ":packet",
        "//cv-cuda-pipeline/framework:calculator_context",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:graph_service",
        "//cv-cuda-pipeline/framework:resources",
        "//cv-cuda-pipeline/framework:timestamp",
        "//cv-cuda-pipeline/framework/api3/internal:contract_fields",
        "//cv-cuda-pipeline/framework/api3/internal:contract_to_tuple",
        "//cv-cuda-pipeline/framework/api3/internal:port_base",
        "//cv-cuda-pipeline/framework/api3/internal:specializers",
        "@com_google_absl//absl/log:absl_check",
        "@com_google_absl//absl/log:absl_log",
    ],
)

cc_test(
    name = "calculator_context_test",
    srcs = ["calculator_context_test.cc"],
    deps = [
        ":any",
        ":calculator",
        ":calculator_context",
        ":contract",
        ":function_runner",
        ":graph",
        ":node",
        ":packet",
        ":port_test_nodes",
        ":stream",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status_matchers",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings:string_view",
    ],
)

cc_library(
    name = "calculator_contract",
    hdrs = ["calculator_contract.h"],
    deps = [
        ":any",
        ":contract",
        "//cv-cuda-pipeline/framework:calculator_contract",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:graph_service",
        "//cv-cuda-pipeline/framework/api3/internal:contract_fields",
        "//cv-cuda-pipeline/framework/api3/internal:contract_to_tuple",
        "//cv-cuda-pipeline/framework/api3/internal:has_update_contract",
        "//cv-cuda-pipeline/framework/api3/internal:port_base",
        "//cv-cuda-pipeline/framework/api3/internal:specializers",
        "@com_google_absl//absl/functional:any_invocable",
        "@com_google_absl//absl/status",
    ],
)

cc_test(
    name = "calculator_contract_test",
    srcs = ["calculator_contract_test.cc"],
    deps = [
        ":calculator_contract",
        ":contract",
        ":node",
        ":port_test_nodes",
        "//cv-cuda-pipeline/framework:calculator_contract",
        "//cv-cuda-pipeline/framework:packet_type",
        "//cv-cuda-pipeline/framework/api3/internal:specializers",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
        "//cv-cuda-pipeline/framework/port:status_matchers",
        "@com_google_absl//absl/status",
    ],
)

cc_library(
    name = "packet",
    hdrs = ["packet.h"],
    deps = [
        ":any",
        "//cv-cuda-pipeline/framework:packet",
        "//cv-cuda-pipeline/framework:timestamp",
        "//cv-cuda-pipeline/framework/port:status",
        "@com_google_absl//absl/functional:any_invocable",
        "@com_google_absl//absl/status:statusor",
    ],
)

cc_library(
    name = "packet_matcher",
    testonly = True,
    hdrs = ["packet_matcher.h"],
    deps = [
        ":packet",
        "//cv-cuda-pipeline/framework:demangle",
        "//cv-cuda-pipeline/framework/port:gtest_main",
    ],
)

cc_test(
    name = "packet_test",
    srcs = ["packet_test.cc"],
    deps = [
        ":packet",
        ":packet_matcher",
        "//cv-cuda-pipeline/framework:timestamp",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:status_matchers",
        "@com_google_absl//absl/status:statusor",
    ],
)

cc_library(
    name = "contract",
    hdrs = ["contract.h"],
    deps = [
        "//cv-cuda-pipeline/framework/api3/internal:contract_fields",
        "//cv-cuda-pipeline/framework/api3/internal:port_base",
        "//cv-cuda-pipeline/framework/api3/internal:specializers",
        "@com_google_absl//absl/base:core_headers",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/log:absl_log",
    ],
)

cc_library(
    name = "node",
    hdrs = ["node.h"],
    deps = [
        "//cv-cuda-pipeline/framework/port:compile_time_string",
        "@com_google_absl//absl/strings:string_view",
    ],
)

cc_test(
    name = "node_test",
    srcs = [
        "node_test.cc",
        "node_test.h",
    ],
    deps = [
        ":contract",
        ":node",
        "//cv-cuda-pipeline/framework/api3/testing:bar_cc_proto",
        "//cv-cuda-pipeline/framework/api3/testing:foo_cc_proto",
        "//cv-cuda-pipeline/framework/port:gtest_main",
    ],
)

cc_library(
    name = "graph",
    hdrs = ["graph.h"],
    deps = [
        ":contract",
        ":one_of",
        ":side_packet",
        ":stream",
        "//cv-cuda-pipeline/framework:calculator_cc_proto",
        "//cv-cuda-pipeline/framework/api3/internal:contract_fields",
        "//cv-cuda-pipeline/framework/api3/internal:contract_to_tuple",
        "//cv-cuda-pipeline/framework/api3/internal:contract_validator",
        "//cv-cuda-pipeline/framework/api3/internal:graph_builder",
        "//cv-cuda-pipeline/framework/api3/internal:port_base",
        "//cv-cuda-pipeline/framework/api3/internal:specializers",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/log:absl_log",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings:string_view",
    ],
)

cc_test(
    name = "graph_test",
    srcs = [
        "graph_test.cc",
    ],
    deps = [
        ":contract",
        ":graph",
        ":node",
        ":side_packet",
        ":stream",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/api3/testing:generator_cc_proto",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
        "//cv-cuda-pipeline/framework/stream_handler:fixed_size_input_stream_handler_cc_proto",
        "//cv-cuda-pipeline/framework/testdata:night_light_calculator_cc_proto",
        "//cv-cuda-pipeline/framework/testdata:sky_light_calculator_cc_proto",
        "@com_google_absl//absl/strings:string_view",
    ],
)

cc_test(
    name = "graph_interop_test",
    srcs = [
        "graph_interop_test.cc",
    ],
    deps = [
        ":contract",
        ":graph",
        ":port_test_nodes",
        ":side_packet",
        ":stream",
        "//cv-cuda-pipeline/framework:calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/api2:builder",
        "//cv-cuda-pipeline/framework/api3/testing:generator_cc_proto",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
        "//cv-cuda-pipeline/framework/stream_handler:fixed_size_input_stream_handler_cc_proto",
        "//cv-cuda-pipeline/framework/testdata:night_light_calculator_cc_proto",
        "//cv-cuda-pipeline/framework/testdata:sky_light_calculator_cc_proto",
    ],
)

cc_library(
    name = "calculator",
    hdrs = ["calculator.h"],
    deps = [
        ":calculator_context",
        ":calculator_contract",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/api2:node",
        "//cv-cuda-pipeline/framework/api3/internal:contract_validator",
        "//cv-cuda-pipeline/framework/api3/internal:has_update_contract",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "@com_google_absl//absl/status",
    ],
)

cc_test(
    name = "calculator_test",
    srcs = [
        "calculator_test.cc",
        "calculator_test.h",
    ],
    deps = [
        ":calculator",
        ":calculator_context",
        ":calculator_contract",
        ":contract",
        ":node",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:graph_service",
        "//cv-cuda-pipeline/framework:timestamp",
        "//cv-cuda-pipeline/framework/api3/testing:bar_cc_proto",
        "//cv-cuda-pipeline/framework/api3/testing:foo_cc_proto",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/tool:options_map",
        "@com_google_absl//absl/status",
    ],
)

cc_library(
    name = "function_runner",
    srcs = [
        "function_runner_internal.cc",
        "function_runner_internal.h",
    ],
    hdrs = ["function_runner.h"],
    deps = [
        ":graph",
        ":packet",
        ":side_packet",
        ":stream",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:executor",
        "//cv-cuda-pipeline/framework:graph_service",
        "//cv-cuda-pipeline/framework:output_stream_poller",
        "//cv-cuda-pipeline/framework:packet",
        "//cv-cuda-pipeline/framework:thread_pool_executor",
        "//cv-cuda-pipeline/framework:timestamp",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/log:absl_log",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
    ],
)

cc_test(
    name = "function_runner_test",
    srcs = ["function_runner_test.cc"],
    deps = [
        ":any",
        ":calculator",
        ":calculator_context",
        ":calculator_contract",
        ":contract",
        ":function_runner",
        ":graph",
        ":node",
        ":packet",
        ":side_packet",
        ":stream",
        "//cv-cuda-pipeline/calculators/core:pass_through_calculator",
        "//cv-cuda-pipeline/framework:calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:graph_service",
        "//cv-cuda-pipeline/framework:thread_pool_executor",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "@com_google_absl//absl/functional:any_invocable",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings:string_view",
    ],
)

cc_library(
    name = "port_test_nodes",
    testonly = True,
    hdrs = ["port_test_nodes.h"],
    deps = [
        ":contract",
        ":node",
    ],
)

cc_library(
    name = "stream",
    hdrs = ["stream.h"],
    deps = [
        ":any",
        "//cv-cuda-pipeline/framework/api3/internal:graph_builder",
    ],
)

cc_library(
    name = "side_packet",
    hdrs = ["side_packet.h"],
    deps = [
        ":any",
        "//cv-cuda-pipeline/framework/api3/internal:graph_builder",
    ],
)

cc_library(
    name = "any",
    hdrs = ["any.h"],
)

cc_test(
    name = "any_test",
    srcs = [
        "any_test.cc",
    ],
    deps = [
        ":any",
        ":calculator",
        ":calculator_context",
        ":calculator_contract",
        ":contract",
        ":graph",
        ":node",
        ":side_packet",
        ":stream",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:timestamp",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/tool:options_map",
        "@com_google_absl//absl/status",
    ],
)

cc_library(
    name = "subgraph_context",
    hdrs = ["subgraph_context.h"],
    deps = [
        ":contract",
        ":graph",
        ":side_packet",
        ":stream",
        "//cv-cuda-pipeline/framework:calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:graph_service",
        "//cv-cuda-pipeline/framework:resources",
        "//cv-cuda-pipeline/framework/api3/internal:contract_fields",
        "//cv-cuda-pipeline/framework/api3/internal:graph_builder",
        "//cv-cuda-pipeline/framework/api3/internal:port_base",
        "//cv-cuda-pipeline/framework/api3/internal:specializers",
    ],
)

cc_library(
    name = "subgraph",
    hdrs = ["subgraph.h"],
    deps = [
        ":graph",
        ":subgraph_context",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/api2:node",
        "//cv-cuda-pipeline/framework/api3/internal:contract_validator",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/framework/tool:options_map",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
    ],
)

cc_test(
    name = "subgraph_test",
    srcs = ["subgraph_test.cc"],
    deps = [
        ":calculator",
        ":calculator_context",
        ":contract",
        ":graph",
        ":node",
        ":side_packet",
        ":stream",
        ":subgraph",
        ":subgraph_context",
        "//cv-cuda-pipeline/framework:calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/api3/testing:foo_cc_proto",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
        "@com_google_absl//absl/status",
    ],
)

cc_library(
    name = "one_of",
    hdrs = ["one_of.h"],
)

cc_test(
    name = "one_of_test",
    srcs = [
        "one_of_test.cc",
    ],
    deps = [
        ":calculator",
        ":calculator_context",
        ":contract",
        ":function_runner",
        ":graph",
        ":node",
        ":one_of",
        ":packet",
        ":stream",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "@com_google_absl//absl/functional:overload",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
    ],
)
