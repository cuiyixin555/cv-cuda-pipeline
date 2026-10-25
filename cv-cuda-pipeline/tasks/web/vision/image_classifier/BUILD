# This contains the MediaPipe Image Classifier Task.
#
# This task takes video or image frames and outputs the classification result.

load("@rules_cc//cc:cc_library.bzl", "cc_library")
load("//cv-cuda-pipeline/framework/port:build_config.bzl", "ts_declaration", "ts_library")
load("//cv-cuda-pipeline/tasks/web:build_defs.bzl", "jasmine_node_test")

package(default_visibility = ["//cv-cuda-pipeline/tasks:internal"])

licenses(["notice"])

ts_library(
    name = "image_classifier",
    srcs = ["image_classifier.ts"],
    visibility = ["//visibility:public"],
    deps = [
        ":image_classifier_types",
        "//cv-cuda-pipeline/framework:calculator_jspb_proto",
        "//cv-cuda-pipeline/framework:calculator_options_jspb_proto",
        "//cv-cuda-pipeline/tasks/cc/components/containers/proto:classifications_jspb_proto",
        "//cv-cuda-pipeline/tasks/cc/core/proto:base_options_jspb_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/image_classifier/proto:image_classifier_graph_options_jspb_proto",
        "//cv-cuda-pipeline/tasks/web/components/containers:category",
        "//cv-cuda-pipeline/tasks/web/components/containers:classification_result",
        "//cv-cuda-pipeline/tasks/web/components/processors:classifier_options",
        "//cv-cuda-pipeline/tasks/web/components/processors:classifier_result",
        "//cv-cuda-pipeline/tasks/web/core",
        "//cv-cuda-pipeline/tasks/web/core:classifier_options",
        "//cv-cuda-pipeline/tasks/web/vision/core:image_processing_options",
        "//cv-cuda-pipeline/tasks/web/vision/core:vision_task_runner",
        "//cv-cuda-pipeline/web/graph_runner:graph_runner_ts",
    ],
)

ts_declaration(
    name = "image_classifier_types",
    srcs = [
        "image_classifier_options.d.ts",
        "image_classifier_result.d.ts",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cv-cuda-pipeline/tasks/web/components/containers:category",
        "//cv-cuda-pipeline/tasks/web/components/containers:classification_result",
        "//cv-cuda-pipeline/tasks/web/core:classifier_options",
        "//cv-cuda-pipeline/tasks/web/vision/core:vision_task_options",
    ],
)

ts_library(
    name = "image_classifier_test_lib",
    testonly = True,
    srcs = [
        "image_classifier_test.ts",
    ],
    deps = [
        ":image_classifier",
        ":image_classifier_types",
        "//cv-cuda-pipeline/framework:calculator_jspb_proto",
        "//cv-cuda-pipeline/framework/formats:classification_jspb_proto",
        "//cv-cuda-pipeline/tasks/cc/components/containers/proto:classifications_jspb_proto",
        "//cv-cuda-pipeline/tasks/web/core",
        "//cv-cuda-pipeline/tasks/web/core:task_runner_test_utils",
    ],
)

jasmine_node_test(
    name = "image_classifier_test",
    srcs = [":image_classifier_test_lib"],
    tags = ["nomsan"],
)
