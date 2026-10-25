# This package contains options shared by all MediaPipe Vision Tasks for Web.

load("//cv-cuda-pipeline/framework/port:build_config.bzl", "ts_declaration", "ts_library")
load("//cv-cuda-pipeline/tasks/web:build_defs.bzl", "jasmine_node_test")

package(default_visibility = ["//cv-cuda-pipeline/tasks:internal"])

ts_declaration(
    name = "image_processing_options",
    srcs = ["image_processing_options.d.ts"],
    deps = [
        "//cv-cuda-pipeline/tasks/web/components/containers:rect",
    ],
)

ts_declaration(
    name = "vision_task_options",
    srcs = ["vision_task_options.d.ts"],
    deps = [
        "//cv-cuda-pipeline/tasks/web/core",
    ],
)

ts_library(
    name = "types",
    srcs = ["types.ts"],
    deps = [
        "//cv-cuda-pipeline/tasks/web/components/containers:keypoint",
    ],
)

ts_library(
    name = "drawing_utils",
    srcs = [
        "drawing_utils.ts",
        "drawing_utils_category_mask.ts",
        "drawing_utils_confidence_mask.ts",
    ],
    deps = [
        ":image_shader_context",
        ":mask",
        ":types",
        "//cv-cuda-pipeline/tasks/web/components/containers:bounding_box",
        "//cv-cuda-pipeline/tasks/web/components/containers:landmark",
        "//cv-cuda-pipeline/web/graph_runner:graph_runner_ts",
    ],
)

ts_library(
    name = "drawing_utils_test_lib",
    testonly = True,
    srcs = ["drawing_utils.test.ts"],
    deps = [
        ":drawing_utils",
        ":image_shader_context",
        ":mask",
    ],
)

jasmine_node_test(
    name = "drawing_utils_test",
    srcs = [":drawing_utils_test_lib"],
)

ts_library(
    name = "image",
    srcs = ["image.ts"],
    deps = ["image_shader_context"],
)

ts_library(
    name = "image_shader_context",
    srcs = ["image_shader_context.ts"],
)

ts_library(
    name = "image_test_lib",
    testonly = True,
    srcs = ["image.test.ts"],
    deps = [
        ":image",
        ":image_shader_context",
    ],
)

jasmine_node_test(
    name = "image_test",
    srcs = [":image_test_lib"],
)

ts_library(
    name = "mask",
    srcs = ["mask.ts"],
    deps = [
        ":image_shader_context",
        "//cv-cuda-pipeline/web/graph_runner:platform_utils",
    ],
)

ts_library(
    name = "mask_test_lib",
    testonly = True,
    srcs = ["mask.test.ts"],
    deps = [
        ":image_shader_context",
        ":mask",
    ],
)

jasmine_node_test(
    name = "mask_test",
    srcs = [":mask_test_lib"],
)

ts_library(
    name = "vision_task_runner",
    srcs = ["vision_task_runner.ts"],
    deps = [
        ":image",
        ":image_processing_options",
        ":image_shader_context",
        ":mask",
        ":vision_task_options",
        "//cv-cuda-pipeline/framework/formats:rect_jspb_proto",
        "//cv-cuda-pipeline/tasks/web/core",
        "//cv-cuda-pipeline/tasks/web/core:task_runner",
        "//cv-cuda-pipeline/web/graph_runner:graph_runner_image_lib_ts",
        "//cv-cuda-pipeline/web/graph_runner:graph_runner_logging_lib_ts",
        "//cv-cuda-pipeline/web/graph_runner:graph_runner_ts",
        "//cv-cuda-pipeline/web/graph_runner:platform_utils",
        "//cv-cuda-pipeline/web/graph_runner:register_model_resources_graph_service_ts",
    ],
)

ts_library(
    name = "vision_task_runner_test_lib",
    testonly = True,
    srcs = ["vision_task_runner_test.ts"],
    deps = [
        ":image_processing_options",
        ":vision_task_options",
        ":vision_task_runner",
        "//cv-cuda-pipeline/framework/formats:rect_jspb_proto",
        "//cv-cuda-pipeline/tasks/cc/core/proto:base_options_jspb_proto",
        "//cv-cuda-pipeline/tasks/web/core:task_runner_test_utils",
        "//cv-cuda-pipeline/web/graph_runner:graph_runner_ts",
    ],
)

jasmine_node_test(
    name = "vision_task_runner_test",
    srcs = [":vision_task_runner_test_lib"],
)
