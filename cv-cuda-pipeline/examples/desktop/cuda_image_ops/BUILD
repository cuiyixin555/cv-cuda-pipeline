load("@rules_cc//cc:cc_binary.bzl", "cc_binary")
load("@rules_cc//cc:cc_test.bzl", "cc_test")
load("@rules_cuda//cuda:defs.bzl", "cuda_library")

package(default_visibility = ["//visibility:public"])

cuda_library(
    name = "image_kernels",
    srcs = ["image_kernels.cu"],
    hdrs = ["image_kernels.h"],
)

cc_binary(
    name = "cuda_image_ops",
    srcs = ["cuda_image_ops.cc"],
    deps = [
        ":image_kernels",
        "//cv-cuda-pipeline/framework/port:opencv_core",
        "//cv-cuda-pipeline/framework/port:opencv_imgcodecs",
        "@local_cuda//:cuda_runtime",
    ],
)

cc_test(
    name = "image_kernels_test",
    srcs = ["image_kernels_test.cc"],
    size = "small",
    deps = [
        ":image_kernels",
        "@local_cuda//:cuda_runtime",
    ],
)
