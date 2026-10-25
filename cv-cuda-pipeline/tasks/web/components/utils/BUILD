load("//cv-cuda-pipeline/framework/port:build_config.bzl", "ts_library")
load("//cv-cuda-pipeline/tasks/web:build_defs.bzl", "jasmine_node_test")

package(default_visibility = ["//cv-cuda-pipeline/tasks:internal"])

ts_library(
    name = "cosine_similarity",
    srcs = ["cosine_similarity.ts"],
    deps = [
        "//cv-cuda-pipeline/tasks/web/components/containers:embedding_result",
    ],
)

ts_library(
    name = "cosine_similarity_test_lib",
    testonly = True,
    srcs = ["cosine_similarity_test.ts"],
    deps = [
        ":cosine_similarity",
        "//cv-cuda-pipeline/tasks/web/components/containers:embedding_result",
    ],
)

jasmine_node_test(
    name = "cosine_similarity_test",
    srcs = [":cosine_similarity_test_lib"],
)

ts_library(
    name = "numeric_conversion",
    srcs = ["numeric_conversion.ts"],
    visibility = ["//cv-cuda-pipeline:__subpackages__"],
)

ts_library(
    name = "numeric_conversion_test_lib",
    testonly = True,
    srcs = ["numeric_conversion_test.ts"],
    deps = [":numeric_conversion"],
)

jasmine_node_test(
    name = "numeric_conversion_test",
    srcs = [":numeric_conversion_test_lib"],
)
