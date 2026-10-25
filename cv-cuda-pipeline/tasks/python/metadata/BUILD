# Placeholder for internal Python strict library and test compatibility macro.
load("//cv-cuda-pipeline/tasks/metadata:build_defs.bzl", "stamp_metadata_parser_version")
# Placeholder: load py_binary

package(
    licenses = ["notice"],  # Apache 2.0
)

stamp_metadata_parser_version(
    name = "metadata_parser_py",
    srcs = ["metadata_parser.py.template"],
    outs = ["metadata_parser.py"],
)

py_library(
    name = "metadata",
    srcs = [
        "metadata.py",
        ":metadata_parser_py",
    ],
    data = ["//cv-cuda-pipeline/tasks/metadata:metadata_schema.fbs"],
    visibility = ["//visibility:public"],
    deps = [
        "//cv-cuda-pipeline/tasks/cc/metadata/python:_pywrap_metadata_version",
        "//cv-cuda-pipeline/tasks/metadata:metadata_schema_py",
        "//cv-cuda-pipeline/tasks/metadata:schema_py",
        "//cv-cuda-pipeline/tasks/python/core:mediapipe_c_bindings",
        "//cv-cuda-pipeline/tasks/python/core:mediapipe_c_utils",
        "//third_party/py/tensorflow:tensorflow_no_contrib",
        "@flatbuffers//:runtime_py",
        "@mediapipe_pip_deps_numpy//:pkg",
    ],
)

py_binary(
    name = "metadata_displayer_cli",
    srcs = ["metadata_displayer_cli.py"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":metadata",
        "@mediapipe_pip_deps_absl_py//:pkg",
    ],
)
