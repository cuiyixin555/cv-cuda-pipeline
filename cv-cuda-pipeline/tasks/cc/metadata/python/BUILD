load("@org_tensorflow//tensorflow:tensorflow.bzl", "pybind_extension")

package(
    default_visibility = ["//cv-cuda-pipeline/tasks:internal"],
    licenses = ["notice"],  # Apache 2.0
)

pybind_extension(
    name = "_pywrap_metadata_version",
    srcs = [
        "metadata_version.cc",
    ],
    features = ["-use_header_modules"],
    module_name = "_pywrap_metadata_version",
    deps = [
        "//cv-cuda-pipeline/tasks/cc/metadata:metadata_version",
        "@litert//tflite/c:common",
        "@pybind11",
    ],
)
