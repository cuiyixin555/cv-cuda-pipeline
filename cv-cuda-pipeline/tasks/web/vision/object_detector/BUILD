# This contains the MediaPipe Object Detector Task.
#
# This task takes video frames and outputs synchronized frames along with
# the detection results for one or more object categories, using Object Detector.

load("@rules_cc//cc:cc_library.bzl", "cc_library")
load("//cv-cuda-pipeline/framework/port:build_config.bzl", "ts_declaration", "ts_library")
load("//cv-cuda-pipeline/tasks/web:build_defs.bzl", "jasmine_node_test")

package(default_visibility = ["//cv-cuda-pipeline/tasks:internal"])

licenses(["notice"])

ts_library(
    name = "object_detector",
    srcs = ["object_detector.ts"],
    visibility = ["//visibility:public"],
    deps = [
        ":object_detector_types",
        "//cv-cuda-pipeline/framework:calculator_jspb_proto",
        "//cv-cuda-pipeline/framework:calculator_options_jspb_proto",
        "//cv-cuda-pipeline/framework/formats:detection_jspb_proto",
        "//cv-cuda-pipeline/tasks/cc/core/proto:base_options_jspb_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/object_detector/proto:object_detector_options_jspb_proto",
        "//cv-cuda-pipeline/tasks/web/components/containers:category",
        "//cv-cuda-pipeline/tasks/web/components/processors:detection_result",
        "//cv-cuda-pipeline/tasks/web/core",
        "//cv-cuda-pipeline/tasks/web/vision/core:image_processing_options",
        "//cv-cuda-pipeline/tasks/web/vision/core:vision_task_runner",
        "//cv-cuda-pipeline/web/graph_runner:graph_runner_ts",
    ],
)

ts_declaration(
    name = "object_detector_types",
    srcs = [
        "object_detector_options.d.ts",
        "object_detector_result.d.ts",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cv-cuda-pipeline/tasks/web/components/containers:bounding_box",
        "//cv-cuda-pipeline/tasks/web/components/containers:category",
        "//cv-cuda-pipeline/tasks/web/components/containers:detection_result",
        "//cv-cuda-pipeline/tasks/web/core",
        "//cv-cuda-pipeline/tasks/web/core:classifier_options",
        "//cv-cuda-pipeline/tasks/web/vision/core:vision_task_options",
    ],
)

ts_library(
    name = "object_detector_test_lib",
    testonly = True,
    srcs = [
        "object_detector_test.ts",
    ],
    deps = [
        ":object_detector",
        ":object_detector_types",
        "//cv-cuda-pipeline/framework:calculator_jspb_proto",
        "//cv-cuda-pipeline/framework/formats:detection_jspb_proto",
        "//cv-cuda-pipeline/framework/formats:location_data_jspb_proto",
        "//cv-cuda-pipeline/tasks/web/core",
        "//cv-cuda-pipeline/tasks/web/core:task_runner_test_utils",
    ],
)

jasmine_node_test(
    name = "object_detector_test",
    srcs = [":object_detector_test_lib"],
    tags = ["nomsan"],
)
