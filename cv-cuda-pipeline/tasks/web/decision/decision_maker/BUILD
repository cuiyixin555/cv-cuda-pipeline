load("//cv-cuda-pipeline/framework/port:build_config.bzl", "ts_declaration", "ts_library")
load("//cv-cuda-pipeline/tasks/web:build_defs.bzl", "jasmine_node_test")

package(default_visibility = ["//cv-cuda-pipeline/tasks:internal"])

licenses(["notice"])

ts_library(
    name = "decision_maker",
    srcs = ["decision_maker.ts"],
    visibility = ["//visibility:public"],
    deps = [
        ":decision_maker_model_types",
        "//cv-cuda-pipeline/tasks/cc/core/proto:base_options_jspb_proto",
        "//cv-cuda-pipeline/tasks/web/core",
        "//cv-cuda-pipeline/tasks/web/core:task_runner",
        "//cv-cuda-pipeline/web/graph_runner:graph_runner_ts",
    ],
)

ts_declaration(
    name = "decision_maker_model_types",
    srcs = [
        "decision_maker_options.d.ts",
        "decision_maker_result.d.ts",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cv-cuda-pipeline/tasks/web/core",
    ],
)

ts_library(
    name = "decision_maker_test_lib",
    testonly = True,
    srcs = ["decision_maker_test.ts"],
    deps = [
        ":decision_maker",
        "//cv-cuda-pipeline/tasks/web/core:task_logger",
        "//cv-cuda-pipeline/web/graph_runner:graph_runner_ts",
    ],
)

jasmine_node_test(
    name = "decision_maker_test",
    srcs = [":decision_maker_test_lib"],
    tags = ["nobuilder"],
)
