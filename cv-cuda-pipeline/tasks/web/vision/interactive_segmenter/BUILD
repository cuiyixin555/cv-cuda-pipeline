# This contains the MediaPipe Interactive Segmenter Task.

load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")
load("//cv-cuda-pipeline/framework/port:build_config.bzl", "ts_library")
load("//cv-cuda-pipeline/tasks/web:build_defs.bzl", "jasmine_node_test")

package(default_visibility = ["//cv-cuda-pipeline/tasks:internal"])

licenses(["notice"])

ts_library(
    name = "interactive_segmenter",
    srcs = [
        "interactive_segmenter.ts",
    ],
    deps = [
        ":interactive_segmenter_types",
        "//cv-cuda-pipeline/calculators/tensor:inference_calculator_jspb_proto",
        "//cv-cuda-pipeline/tasks/cc/core/proto:acceleration_jspb_proto",
        "//cv-cuda-pipeline/tasks/cc/core/proto:base_options_jspb_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/interactive_segmenter/proto:stroke_jspb_proto",
        "//cv-cuda-pipeline/tasks/web/core",
        "//cv-cuda-pipeline/tasks/web/core:task_runner",
        "//cv-cuda-pipeline/tasks/web/vision/core:image_shader_context",
        "//cv-cuda-pipeline/tasks/web/vision/core:mask",
        "//cv-cuda-pipeline/web/graph_runner:graph_runner_image_lib_ts",
        "//cv-cuda-pipeline/web/graph_runner:graph_runner_ts",
        "//cv-cuda-pipeline/web/graph_runner:platform_utils",
    ],
)

ts_library(
    name = "interactive_segmenter_types",
    srcs = ["interactive_segmenter_options.ts"],
    deps = [
        "//cv-cuda-pipeline/tasks/web/core",
    ],
)

ts_library(
    name = "interactive_segmenter_test_lib",
    testonly = True,
    srcs = [
        "interactive_segmenter_test.ts",
    ],
    deps = [
        ":interactive_segmenter",
        "//cv-cuda-pipeline/tasks/cc/core/proto:base_options_jspb_proto",
        "//cv-cuda-pipeline/tasks/web/core",
        "//cv-cuda-pipeline/tasks/web/core:fileset_resolver",
        "//cv-cuda-pipeline/tasks/web/core:task_logger",
        "//cv-cuda-pipeline/tasks/web/core:task_runner",
        "//cv-cuda-pipeline/tasks/web/core:task_runner_test_utils",
        "//cv-cuda-pipeline/tasks/web/vision/core:drawing_utils",
        "//cv-cuda-pipeline/tasks/web/vision/core:image_shader_context",
        "//cv-cuda-pipeline/tasks/web/vision/core:mask",
        "//cv-cuda-pipeline/web/graph_runner:graph_runner_image_lib_ts",
        "//cv-cuda-pipeline/web/graph_runner:platform_utils",
    ],
)

# Headless unit tests verifying options contract, DOM mocking, and native Wasm memory bridge.
# Executes natively on standard Linux development clients in NodeJS using:
#
# bazel test //cv-cuda-pipeline/tasks/web/vision/interactive_segmenter:interactive_segmenter_test
jasmine_node_test(
    name = "interactive_segmenter_test",
    srcs = [":interactive_segmenter_test_lib"],
    tags = ["nomsan"],
)
