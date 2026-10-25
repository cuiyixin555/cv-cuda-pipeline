# This contains the MediaPipe Gesture Recognizer Task.
#
# This task takes video frames and outputs synchronized frames along with
# the detection results for one or more gesture categories, using Gesture Recognizer.

load("@rules_cc//cc:cc_library.bzl", "cc_library")
load("//cv-cuda-pipeline/framework/port:build_config.bzl", "ts_declaration", "ts_library")
load("//cv-cuda-pipeline/tasks/web:build_defs.bzl", "jasmine_node_test")

package(default_visibility = ["//cv-cuda-pipeline/tasks:internal"])

licenses(["notice"])

ts_library(
    name = "gesture_recognizer",
    srcs = ["gesture_recognizer.ts"],
    visibility = ["//visibility:public"],
    deps = [
        ":gesture_recognizer_types",
        "//cv-cuda-pipeline/framework:calculator_jspb_proto",
        "//cv-cuda-pipeline/framework:calculator_options_jspb_proto",
        "//cv-cuda-pipeline/framework/formats:classification_jspb_proto",
        "//cv-cuda-pipeline/framework/formats:landmark_jspb_proto",
        "//cv-cuda-pipeline/tasks/cc/core/proto:base_options_jspb_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/gesture_recognizer/proto:gesture_classifier_graph_options_jspb_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/gesture_recognizer/proto:gesture_recognizer_graph_options_jspb_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/gesture_recognizer/proto:hand_gesture_recognizer_graph_options_jspb_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/hand_detector/proto:hand_detector_graph_options_jspb_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/hand_landmarker/proto:hand_landmarker_graph_options_jspb_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/hand_landmarker/proto:hand_landmarks_detector_graph_options_jspb_proto",
        "//cv-cuda-pipeline/tasks/web/components/containers:category",
        "//cv-cuda-pipeline/tasks/web/components/containers:landmark",
        "//cv-cuda-pipeline/tasks/web/components/processors:classifier_options",
        "//cv-cuda-pipeline/tasks/web/components/processors:landmark_result",
        "//cv-cuda-pipeline/tasks/web/core",
        "//cv-cuda-pipeline/tasks/web/core:classifier_options",
        "//cv-cuda-pipeline/tasks/web/vision/core:image_processing_options",
        "//cv-cuda-pipeline/tasks/web/vision/core:vision_task_runner",
        "//cv-cuda-pipeline/tasks/web/vision/hand_landmarker:hand_landmarks_connections",
        "//cv-cuda-pipeline/web/graph_runner:graph_runner_ts",
    ],
)

ts_declaration(
    name = "gesture_recognizer_types",
    srcs = [
        "gesture_recognizer_options.d.ts",
        "gesture_recognizer_result.d.ts",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cv-cuda-pipeline/tasks/web/components/containers:category",
        "//cv-cuda-pipeline/tasks/web/components/containers:landmark",
        "//cv-cuda-pipeline/tasks/web/core",
        "//cv-cuda-pipeline/tasks/web/core:classifier_options",
        "//cv-cuda-pipeline/tasks/web/vision/core:vision_task_options",
    ],
)

ts_library(
    name = "gesture_recognizer_test_lib",
    testonly = True,
    srcs = [
        "gesture_recognizer_test.ts",
    ],
    deps = [
        ":gesture_recognizer",
        ":gesture_recognizer_types",
        "//cv-cuda-pipeline/framework:calculator_jspb_proto",
        "//cv-cuda-pipeline/framework/formats:classification_jspb_proto",
        "//cv-cuda-pipeline/framework/formats:landmark_jspb_proto",
        "//cv-cuda-pipeline/tasks/web/core",
        "//cv-cuda-pipeline/tasks/web/core:task_runner_test_utils",
        "//cv-cuda-pipeline/tasks/web/vision/core:vision_task_runner",
    ],
)

jasmine_node_test(
    name = "gesture_recognizer_test",
    srcs = [":gesture_recognizer_test_lib"],
    tags = ["nomsan"],
)
