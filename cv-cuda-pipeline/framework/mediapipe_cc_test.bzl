"""Macro for multi-platform C++ tests."""

# buildifier: disable=out-of-order-load

load("@rules_cc//cc:cc_test.bzl", "cc_test")
load("@rules_cc//cc:cc_library.bzl", "cc_library")

DEFAULT_ADDITIONAL_TEST_DEPS = []

def mediapipe_cc_test(
        name,
        srcs = [],
        data = [],
        deps = [],
        size = None,
        tags = [],
        linux_tags = [],
        wasm_tags = [],
        timeout = None,
        args = [],
        additional_deps = DEFAULT_ADDITIONAL_TEST_DEPS,
        **kwargs):
    cc_library(
        name = name + "_lib",
        testonly = True,
        srcs = srcs,
        data = data,
        deps = deps + additional_deps,
        alwayslink = 1,
    )

    native.cc_test(
        name = name,
        size = size,
        timeout = timeout,
        args = args,
        deps = [":{}_lib".format(name)],
        tags = tags + linux_tags + wasm_tags,
        **kwargs
    )
