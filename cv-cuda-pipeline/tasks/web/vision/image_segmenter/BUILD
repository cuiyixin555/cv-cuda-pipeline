# This contains the MediaPipe Image Segmenter Task.

load("@rules_cc//cc:cc_library.bzl", "cc_library")
load("//cv-cuda-pipeline/framework/port:build_config.bzl", "ts_declaration", "ts_library")
load("//cv-cuda-pipeline/tasks/web:build_defs.bzl", "jasmine_node_test")

package(default_visibility = ["//cv-cuda-pipeline/tasks:internal"])

licenses(["notice"])

ts_library(
    name = "image_segmenter",
    srcs = [
        "image_segmenter.ts",
        "image_segmenter_result.ts",
    ],
    deps = [
        ":image_segmenter_types",
        "//cv-cuda-pipeline/framework:calculator_jspb_proto",
        "//cv-cuda-pipeline/framework:calculator_options_jspb_proto",
        "//cv-cuda-pipeline/tasks/cc/core/proto:base_options_jspb_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/image_segmenter/calculators:tensors_to_segmentation_calculator_jspb_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/image_segmenter/proto:image_segmenter_graph_options_jspb_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/image_segmenter/proto:segmenter_options_jspb_proto",
        "//cv-cuda-pipeline/tasks/web/core",
        "//cv-cuda-pipeline/tasks/web/core:classifier_options",
        "//cv-cuda-pipeline/tasks/web/vision/core:image_processing_options",
        "//cv-cuda-pipeline/tasks/web/vision/core:mask",
        "//cv-cuda-pipeline/tasks/web/vision/core:vision_task_runner",
        "//cv-cuda-pipeline/util:label_map_jspb_proto",
        "//cv-cuda-pipeline/web/graph_runner:graph_runner_ts",
    ],
)

ts_declaration(
    name = "image_segmenter_types",
    srcs = ["image_segmenter_options.d.ts"],
    deps = ["//cv-cuda-pipeline/tasks/web/vision/core:vision_task_options"],
)

ts_library(
    name = "image_segmenter_test_lib",
    testonly = True,
    srcs = [
        "image_segmenter_test.ts",
    ],
    deps = [
        ":image_segmenter",
        ":image_segmenter_types",
        "//cv-cuda-pipeline/framework:calculator_jspb_proto",
        "//cv-cuda-pipeline/tasks/web/core",
        "//cv-cuda-pipeline/tasks/web/core:task_runner_test_utils",
        "//cv-cuda-pipeline/tasks/web/vision/core:mask",
        "//cv-cuda-pipeline/web/graph_runner:graph_runner_image_lib_ts",
    ],
)

jasmine_node_test(
    name = "image_segmenter_test",
    srcs = [":image_segmenter_test_lib"],
    tags = ["nomsan"],
)
