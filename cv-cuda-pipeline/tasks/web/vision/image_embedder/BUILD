# This contains the MediaPipe Image Embedder Task.
#
# This task performs embedding extraction on images.

load("@rules_cc//cc:cc_library.bzl", "cc_library")
load("//cv-cuda-pipeline/framework/port:build_config.bzl", "ts_declaration", "ts_library")
load("//cv-cuda-pipeline/tasks/web:build_defs.bzl", "jasmine_node_test")

package(default_visibility = ["//cv-cuda-pipeline/tasks:internal"])

licenses(["notice"])

ts_library(
    name = "image_embedder",
    srcs = ["image_embedder.ts"],
    visibility = ["//visibility:public"],
    deps = [
        ":image_embedder_types",
        "//cv-cuda-pipeline/framework:calculator_jspb_proto",
        "//cv-cuda-pipeline/framework:calculator_options_jspb_proto",
        "//cv-cuda-pipeline/tasks/cc/components/containers/proto:embeddings_jspb_proto",
        "//cv-cuda-pipeline/tasks/cc/core/proto:base_options_jspb_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/image_embedder/proto:image_embedder_graph_options_jspb_proto",
        "//cv-cuda-pipeline/tasks/web/components/containers:embedding_result",
        "//cv-cuda-pipeline/tasks/web/components/processors:embedder_options",
        "//cv-cuda-pipeline/tasks/web/components/processors:embedder_result",
        "//cv-cuda-pipeline/tasks/web/components/utils:cosine_similarity",
        "//cv-cuda-pipeline/tasks/web/core",
        "//cv-cuda-pipeline/tasks/web/core:embedder_options",
        "//cv-cuda-pipeline/tasks/web/vision/core:image_processing_options",
        "//cv-cuda-pipeline/tasks/web/vision/core:vision_task_options",
        "//cv-cuda-pipeline/tasks/web/vision/core:vision_task_runner",
        "//cv-cuda-pipeline/web/graph_runner:graph_runner_ts",
    ],
)

ts_declaration(
    name = "image_embedder_types",
    srcs = [
        "image_embedder_options.d.ts",
        "image_embedder_result.d.ts",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cv-cuda-pipeline/tasks/web/components/containers:embedding_result",
        "//cv-cuda-pipeline/tasks/web/core",
        "//cv-cuda-pipeline/tasks/web/core:embedder_options",
        "//cv-cuda-pipeline/tasks/web/vision/core:vision_task_options",
    ],
)

ts_library(
    name = "image_embedder_test_lib",
    testonly = True,
    srcs = [
        "image_embedder_test.ts",
    ],
    deps = [
        ":image_embedder",
        "//cv-cuda-pipeline/framework:calculator_jspb_proto",
        "//cv-cuda-pipeline/tasks/cc/components/containers/proto:embeddings_jspb_proto",
        "//cv-cuda-pipeline/tasks/web/core",
        "//cv-cuda-pipeline/tasks/web/core:task_runner_test_utils",
    ],
)

jasmine_node_test(
    name = "image_embedder_test",
    srcs = [":image_embedder_test_lib"],
)
