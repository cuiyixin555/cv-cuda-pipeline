load("@rules_cc//cc:cc_test.bzl", "cc_test")

package(
    default_visibility = ["//visibility:private"],
    licenses = ["notice"],  # Apache 2.0
)

cc_test(
    name = "metadata_extractor_test",
    srcs = ["metadata_extractor_test.cc"],
    data = [
        "//cv-cuda-pipeline/tasks/testdata/metadata:data_files",
        "//cv-cuda-pipeline/tasks/testdata/metadata:model_files",
    ],
    deps = [
        "//cv-cuda-pipeline/framework/port:file_helpers",
        "//cv-cuda-pipeline/framework/port:gtest",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/tasks/cc:common",
        "//cv-cuda-pipeline/tasks/cc/core:utils",
        "//cv-cuda-pipeline/tasks/cc/metadata:metadata_extractor",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:cord",
        "@com_google_absl//absl/strings:str_format",
    ],
)

cc_test(
    name = "metadata_version_test",
    srcs = ["metadata_version_test.cc"],
    deps = [
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/tasks/cc/metadata:metadata_version",
        "//cv-cuda-pipeline/tasks/metadata:metadata_schema_cc",
        "@flatbuffers//:runtime_cc",
    ],
)

cc_test(
    name = "metadata_parser_test",
    srcs = ["metadata_parser_test.cc"],
    deps = [
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/tasks/cc/metadata:metadata_version",
    ],
)

cc_test(
    name = "metadata_version_utils_test",
    srcs = ["metadata_version_utils_test.cc"],
    deps = [
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/tasks/cc/metadata:metadata_version_utils",
    ],
)
