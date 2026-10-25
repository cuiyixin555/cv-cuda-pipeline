# This contains the MediaPipe Interactive Segmenter Task.

load("@rules_cc//cc:cc_library.bzl", "cc_library")
load("//cv-cuda-pipeline/framework/port:build_config.bzl", "ts_declaration", "ts_library")
load("//cv-cuda-pipeline/tasks/web:build_defs.bzl", "jasmine_node_test")

package(default_visibility = ["//cv-cuda-pipeline/tasks:internal"])

licenses(["notice"])

ts_library(
    name = "interactive_segmenter_legacy",
    srcs = [
        "interactive_segmenter_legacy.ts",
        "interactive_segmenter_legacy_result.ts",
    ],
    deps = [
        ":interactive_segmenter_legacy_types",
        "//cv-cuda-pipeline/framework:calculator_jspb_proto",
        "//cv-cuda-pipeline/framework:calculator_options_jspb_proto",
        "//cv-cuda-pipeline/tasks/cc/core/proto:base_options_jspb_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/image_segmenter/proto:image_segmenter_graph_options_jspb_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/image_segmenter/proto:segmenter_options_jspb_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/interactive_segmenter_legacy/proto:region_of_interest_jspb_proto",
        "//cv-cuda-pipeline/tasks/web/components/containers:keypoint",
        "//cv-cuda-pipeline/tasks/web/core",
        "//cv-cuda-pipeline/tasks/web/core:classifier_options",
        "//cv-cuda-pipeline/tasks/web/vision/core:image_processing_options",
        "//cv-cuda-pipeline/tasks/web/vision/core:mask",
        "//cv-cuda-pipeline/tasks/web/vision/core:types",
        "//cv-cuda-pipeline/tasks/web/vision/core:vision_task_runner",
        "//cv-cuda-pipeline/web/graph_runner:graph_runner_ts",
    ],
)

ts_declaration(
    name = "interactive_segmenter_legacy_types",
    srcs = ["interactive_segmenter_legacy_options.d.ts"],
    deps = [
        "//cv-cuda-pipeline/tasks/web/core",
        "//cv-cuda-pipeline/tasks/web/vision/core:vision_task_options",
    ],
)

ts_library(
    name = "interactive_segmenter_legacy_test_lib",
    testonly = True,
    srcs = [
        "interactive_segmenter_legacy_test.ts",
    ],
    deps = [
        ":interactive_segmenter_legacy",
        "//cv-cuda-pipeline/framework:calculator_jspb_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/interactive_segmenter_legacy/proto:region_of_interest_jspb_proto",
        "//cv-cuda-pipeline/tasks/web/core",
        "//cv-cuda-pipeline/tasks/web/core:task_runner_test_utils",
        "//cv-cuda-pipeline/tasks/web/vision/core:mask",
        "//cv-cuda-pipeline/web/graph_runner:graph_runner_image_lib_ts",
    ],
)

jasmine_node_test(
    name = "interactive_segmenter_legacy_test",
    srcs = [":interactive_segmenter_legacy_test_lib"],
    tags = ["nomsan"],
)
