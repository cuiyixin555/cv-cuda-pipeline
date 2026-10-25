# This contains the MediaPipe Language Detector Task.

load("@rules_cc//cc:cc_library.bzl", "cc_library")
load("//cv-cuda-pipeline/framework/port:build_config.bzl", "ts_declaration", "ts_library")
load("//cv-cuda-pipeline/tasks/web:build_defs.bzl", "jasmine_node_test")

package(default_visibility = ["//cv-cuda-pipeline/tasks:internal"])

licenses(["notice"])

ts_library(
    name = "language_detector",
    srcs = ["language_detector.ts"],
    visibility = ["//visibility:public"],
    deps = [
        ":language_detector_types",
        "//cv-cuda-pipeline/framework:calculator_jspb_proto",
        "//cv-cuda-pipeline/framework:calculator_options_jspb_proto",
        "//cv-cuda-pipeline/tasks/cc/components/containers/proto:classifications_jspb_proto",
        "//cv-cuda-pipeline/tasks/cc/core/proto:base_options_jspb_proto",
        "//cv-cuda-pipeline/tasks/cc/text/text_classifier/proto:text_classifier_graph_options_jspb_proto",
        "//cv-cuda-pipeline/tasks/web/components/containers:category",
        "//cv-cuda-pipeline/tasks/web/components/containers:classification_result",
        "//cv-cuda-pipeline/tasks/web/components/processors:classifier_options",
        "//cv-cuda-pipeline/tasks/web/components/processors:classifier_result",
        "//cv-cuda-pipeline/tasks/web/core",
        "//cv-cuda-pipeline/tasks/web/core:classifier_options",
        "//cv-cuda-pipeline/tasks/web/core:task_runner",
        "//cv-cuda-pipeline/web/graph_runner:graph_runner_ts",
    ],
)

ts_declaration(
    name = "language_detector_types",
    srcs = [
        "language_detector_options.d.ts",
        "language_detector_result.d.ts",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cv-cuda-pipeline/tasks/web/components/containers:category",
        "//cv-cuda-pipeline/tasks/web/components/containers:classification_result",
        "//cv-cuda-pipeline/tasks/web/core",
        "//cv-cuda-pipeline/tasks/web/core:classifier_options",
    ],
)

ts_library(
    name = "language_detector_test_lib",
    testonly = True,
    srcs = [
        "language_detector_test.ts",
    ],
    deps = [
        ":language_detector",
        "//cv-cuda-pipeline/framework:calculator_jspb_proto",
        "//cv-cuda-pipeline/framework/formats:classification_jspb_proto",
        "//cv-cuda-pipeline/tasks/cc/components/containers/proto:classifications_jspb_proto",
        "//cv-cuda-pipeline/tasks/web/core",
        "//cv-cuda-pipeline/tasks/web/core:task_runner_test_utils",
    ],
)

jasmine_node_test(
    name = "language_detector_test",
    srcs = [":language_detector_test_lib"],
)
