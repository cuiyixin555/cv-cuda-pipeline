# This contains the MediaPipe Text Embedder Task.
#
# This task takes text input and performs embedding
#

load("@rules_cc//cc:cc_library.bzl", "cc_library")
load("//cv-cuda-pipeline/framework/port:build_config.bzl", "ts_declaration", "ts_library")
load("//cv-cuda-pipeline/tasks/web:build_defs.bzl", "jasmine_node_test")

package(default_visibility = ["//cv-cuda-pipeline/tasks:internal"])

licenses(["notice"])

ts_library(
    name = "text_embedder",
    srcs = ["text_embedder.ts"],
    visibility = ["//visibility:public"],
    deps = [
        ":text_embedder_types",
        "//cv-cuda-pipeline/framework:calculator_jspb_proto",
        "//cv-cuda-pipeline/framework:calculator_options_jspb_proto",
        "//cv-cuda-pipeline/tasks/cc/components/containers/proto:embeddings_jspb_proto",
        "//cv-cuda-pipeline/tasks/cc/core/proto:base_options_jspb_proto",
        "//cv-cuda-pipeline/tasks/cc/text/text_embedder/proto:text_embedder_graph_options_jspb_proto",
        "//cv-cuda-pipeline/tasks/web/components/containers:embedding_result",
        "//cv-cuda-pipeline/tasks/web/components/processors:embedder_options",
        "//cv-cuda-pipeline/tasks/web/components/processors:embedder_result",
        "//cv-cuda-pipeline/tasks/web/components/utils:cosine_similarity",
        "//cv-cuda-pipeline/tasks/web/core",
        "//cv-cuda-pipeline/tasks/web/core:embedder_options",
        "//cv-cuda-pipeline/tasks/web/core:task_runner",
        "//cv-cuda-pipeline/web/graph_runner:graph_runner_ts",
    ],
)

ts_declaration(
    name = "text_embedder_types",
    srcs = [
        "text_embedder_options.d.ts",
        "text_embedder_result.d.ts",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cv-cuda-pipeline/tasks/web/components/containers:embedding_result",
        "//cv-cuda-pipeline/tasks/web/core",
        "//cv-cuda-pipeline/tasks/web/core:embedder_options",
    ],
)

ts_library(
    name = "text_embedder_test_lib",
    testonly = True,
    srcs = [
        "text_embedder_test.ts",
    ],
    deps = [
        ":text_embedder",
        "//cv-cuda-pipeline/framework:calculator_jspb_proto",
        "//cv-cuda-pipeline/tasks/cc/components/containers/proto:embeddings_jspb_proto",
        "//cv-cuda-pipeline/tasks/web/core",
        "//cv-cuda-pipeline/tasks/web/core:task_runner_test_utils",
    ],
)

jasmine_node_test(
    name = "text_embedder_test",
    srcs = [":text_embedder_test_lib"],
)
