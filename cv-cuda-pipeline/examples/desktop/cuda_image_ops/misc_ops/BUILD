load("@rules_cc//cc:cc_binary.bzl", "cc_binary")
load("@rules_cc//cc:cc_test.bzl", "cc_test")
load("@rules_cuda//cuda:defs.bzl", "cuda_library")

package(default_visibility = ["//visibility:public"])

cuda_library(
    name = "operator_kernels",
    srcs = ["operator_kernels.cu"],
    hdrs = ["operator_kernels.h"],
)

cc_binary(
    name = "operator_demo",
    srcs = ["operator_demo.cc"],
    deps = [
        ":operator_kernels",
        "//cv-cuda-pipeline/framework/port:opencv_core",
        "//cv-cuda-pipeline/framework/port:opencv_imgcodecs",
        "@local_cuda//:cuda_runtime",
    ],
)

cc_test(
    name = "operator_test",
    srcs = ["operator_test.cc"],
    deps = [
        ":operator_kernels",
        "@local_cuda//:cuda_runtime",
    ],
)
