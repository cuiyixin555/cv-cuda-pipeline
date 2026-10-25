load("@rules_cc//cc:cc_binary.bzl", "cc_binary")
load("@rules_cc//cc:cc_test.bzl", "cc_test")
load("@rules_cuda//cuda:defs.bzl", "cuda_library")

package(default_visibility = ["//visibility:public"])

cuda_library(
    name = "transform_kernels",
    srcs = ["transform_kernels.cu"],
    hdrs = ["transform_kernels.h"],
)

cc_binary(
    name = "operator_demo",
    srcs = ["operator_demo.cc"],
    deps = [
        ":transform_kernels",
        "//cv-cuda-pipeline/framework/port:opencv_core",
        "//cv-cuda-pipeline/framework/port:opencv_imgcodecs",
        "@local_cuda//:cuda_runtime",
    ],
)

cc_test(
    name = "operator_test",
    srcs = ["operator_test.cc"],
    size = "small",
    deps = [
        ":transform_kernels",
        "@local_cuda//:cuda_runtime",
    ],
)
