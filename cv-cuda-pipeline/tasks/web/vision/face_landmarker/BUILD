# This contains the MediaPipe Face Landmarker Task.
#
# This task takes video frames and outputs synchronized frames along with
# the detection results for one or more face categories, using Face Landmarker.

load("@rules_cc//cc:cc_library.bzl", "cc_library")
load("//cv-cuda-pipeline/framework/port:build_config.bzl", "ts_declaration", "ts_library")
load("//cv-cuda-pipeline/tasks/web:build_defs.bzl", "jasmine_node_test")

package(default_visibility = ["//cv-cuda-pipeline/tasks:internal"])

licenses(["notice"])

ts_library(
    name = "face_landmarker",
    srcs = ["face_landmarker.ts"],
    visibility = ["//visibility:public"],
    deps = [
        ":face_landmarker_types",
        ":face_landmarks_connections",
        "//cv-cuda-pipeline/framework:calculator_jspb_proto",
        "//cv-cuda-pipeline/framework:calculator_options_jspb_proto",
        "//cv-cuda-pipeline/framework/formats:classification_jspb_proto",
        "//cv-cuda-pipeline/framework/formats:landmark_jspb_proto",
        "//cv-cuda-pipeline/tasks/cc/core/proto:base_options_jspb_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/face_detector/proto:face_detector_graph_options_jspb_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/face_geometry/proto:face_geometry_jspb_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/face_landmarker/proto:face_landmarker_graph_options_jspb_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/face_landmarker/proto:face_landmarks_detector_graph_options_jspb_proto",
        "//cv-cuda-pipeline/tasks/web/components/containers:category",
        "//cv-cuda-pipeline/tasks/web/components/containers:classification_result",
        "//cv-cuda-pipeline/tasks/web/components/containers:landmark",
        "//cv-cuda-pipeline/tasks/web/components/containers:matrix",
        "//cv-cuda-pipeline/tasks/web/components/processors:classifier_result",
        "//cv-cuda-pipeline/tasks/web/components/processors:landmark_result",
        "//cv-cuda-pipeline/tasks/web/core",
        "//cv-cuda-pipeline/tasks/web/vision/core:image_processing_options",
        "//cv-cuda-pipeline/tasks/web/vision/core:vision_task_runner",
        "//cv-cuda-pipeline/web/graph_runner:graph_runner_ts",
    ],
)

ts_library(
    name = "face_landmarks_connections",
    srcs = ["face_landmarks_connections.ts"],
    visibility = ["//visibility:public"],
    deps = ["//cv-cuda-pipeline/tasks/web/vision/core:types"],
)

ts_declaration(
    name = "face_landmarker_types",
    srcs = [
        "face_landmarker_options.d.ts",
        "face_landmarker_result.d.ts",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cv-cuda-pipeline/tasks/web/components/containers:category",
        "//cv-cuda-pipeline/tasks/web/components/containers:classification_result",
        "//cv-cuda-pipeline/tasks/web/components/containers:landmark",
        "//cv-cuda-pipeline/tasks/web/components/containers:matrix",
        "//cv-cuda-pipeline/tasks/web/core",
        "//cv-cuda-pipeline/tasks/web/vision/core:vision_task_options",
    ],
)

ts_library(
    name = "face_landmarker_test_lib",
    testonly = True,
    srcs = [
        "face_landmarker_test.ts",
    ],
    deps = [
        ":face_landmarker",
        ":face_landmarker_types",
        "//cv-cuda-pipeline/framework:calculator_jspb_proto",
        "//cv-cuda-pipeline/framework/formats:classification_jspb_proto",
        "//cv-cuda-pipeline/framework/formats:matrix_data_jspb_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/face_geometry/proto:face_geometry_jspb_proto",
        "//cv-cuda-pipeline/tasks/web/components/processors:landmark_result",
        "//cv-cuda-pipeline/tasks/web/core",
        "//cv-cuda-pipeline/tasks/web/core:task_runner_test_utils",
        "//cv-cuda-pipeline/tasks/web/vision/core:vision_task_runner",
    ],
)

jasmine_node_test(
    name = "face_landmarker_test",
    srcs = [":face_landmarker_test_lib"],
    tags = ["nomsan"],
)
