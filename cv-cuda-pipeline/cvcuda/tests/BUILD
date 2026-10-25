load("@rules_cc//cc:cc_library.bzl", "cc_library")
load("@rules_cc//cc:cc_test.bzl", "cc_test")
load("@rules_cuda//cuda:defs.bzl", "cuda_library")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "nvcv_test_headers",
    hdrs = glob(["**/*.h", "**/*.hpp"]),
    includes = [
        ".",
        "cvcuda/system",
        "cvcuda/unit",
        "nvcv_types/cudatools_system",
        "nvcv_types/system",
        "nvcv_types/unit",
    ],
)

cc_library(
    name = "nvcv_test_main",
    srcs = ["Main.cpp"],
    deps = [
        "//cv-cuda-pipeline/cvcuda/src:nvcv_types",
        "@com_google_googletest//:gtest",
        "@local_cuda//:cuda_runtime",
    ],
    linkopts = ["-lrt"],
)

cc_library(
    name = "nvcv_test_common",
    srcs = [
        "common/HashMD5.cpp",
        "common/Printers.cpp",
        "common/TensorDataUtils.cpp",
    ],
    hdrs = glob(["common/*.hpp"]),
    includes = ["."],
    deps = [
        "//cv-cuda-pipeline/cvcuda/src:cvcuda_headers",
        "//cv-cuda-pipeline/cvcuda/src:nvcv_types",
        "//cv-cuda-pipeline/cvcuda/src:nvcv_util",
        "@local_cuda//:cuda_runtime",
    ],
    linkopts = [
        "-lcrypto",
        "-lz",
    ],
)

cc_library(
    name = "nvcv_test_common_system",
    srcs = [
        "common/BorderUtils.cpp",
        "common/ObjectBag.cpp",
    ],
    deps = [
        ":nvcv_test_common",
        "//cv-cuda-pipeline/cvcuda/src:cvcuda_headers",
        "//cv-cuda-pipeline/cvcuda/src:nvcv_types",
    ],
)

cc_test(
    name = "nvcv_types_unit_test",
    srcs = glob([
        "nvcv_types/unit/*.cpp",
    ], exclude = [
        "nvcv_types/unit/TestCompat.cpp",
    ]) + ["Main.cpp"],
    local_defines = ["NVCV_EXPORTING=1"],
    defines = [
        "NVCV_UNIT_TESTS=0",
        "NVCV_SANITIZED=0",
    ],
    copts = ["-std=c++20"],
    data = glob(["nvcv_types/unit/testdata/**"]),
    deps = [
        ":nvcv_test_common",
        ":nvcv_test_headers",
        ":nvcv_test_main",
        "//cv-cuda-pipeline/cvcuda/src:nvcv_types_priv",
        "//cv-cuda-pipeline/cvcuda/src:nvcv_util",
        "@com_google_googletest//:gtest",
        "@local_cuda//:cuda_runtime",
    ],
    linkopts = ["-lrt"],
    linkstatic = True,
    size = "large",
)

cc_test(
    name = "nvcv_types_system_test",
    srcs = glob([
        "nvcv_types/system/*.cpp",
    ], exclude = [
        "nvcv_types/system/TestVersionApiOverride.cpp",
    ]) + ["Main.cpp"],
    copts = ["-std=c++20"],
    data = glob(["nvcv_types/system/testdata/**"]),
    deps = [
        ":nvcv_test_common",
        ":nvcv_test_common_system",
        ":nvcv_test_headers",
        ":nvcv_test_main",
        "//cv-cuda-pipeline/cvcuda/src:nvcv_types",
        "@com_google_googletest//:gtest",
        "@local_cuda//:cuda_runtime",
    ],
    linkopts = ["-lrt"],
    linkstatic = True,
    size = "large",
)

cc_test(
    name = "nvcv_types_standalone_test",
    srcs = ["nvcv_types/standalone/TestNVCVStandalone.cpp"],
    copts = ["-std=c++17"],
    deps = [
        "//cv-cuda-pipeline/cvcuda/src:nvcv_types",
        "@com_google_googletest//:gtest_main",
        "@local_cuda//:cuda_runtime",
    ],
    linkopts = ["-lrt"],
    linkstatic = True,
)

cc_test(
    name = "nvcv_version_api_override_test",
    srcs = [
        "Main.cpp",
        "nvcv_types/system/TestVersionApiOverride.cpp",
    ],
    copts = ["-std=c++20"],
    defines = ["NVCV_VERSION_API=160000"],
    deps = [
        ":nvcv_test_headers",
        ":nvcv_test_main",
        "//cv-cuda-pipeline/cvcuda/src:nvcv_types",
        "@com_google_googletest//:gtest",
    ],
    linkstatic = True,
)

cuda_library(
    name = "nvcv_cudatools_system_test_cuda",
    srcs = glob(["nvcv_types/cudatools_system/*.cu"]),
    hdrs = glob([
        "nvcv_types/cudatools_system/**/*.cuh",
        "nvcv_types/cudatools_system/**/*.hpp",
    ]),
    includes = [
        ".",
        "nvcv_types/cudatools_system",
    ],
    deps = [
        "//cv-cuda-pipeline/cvcuda/src:cvcuda_headers",
        "//cv-cuda-pipeline/cvcuda/src:nvcv_types_headers",
        "@local_cuda//:cuda_runtime",
    ],
)

cc_test(
    name = "nvcv_cudatools_system_test",
    srcs = glob(["nvcv_types/cudatools_system/*.cpp"]) + ["Main.cpp"],
    copts = ["-std=c++20"],
    deps = [
        ":nvcv_cudatools_system_test_cuda",
        ":nvcv_test_common_system",
        ":nvcv_test_headers",
        ":nvcv_test_main",
        "//cv-cuda-pipeline/cvcuda/src:cvcuda_headers",
        "//cv-cuda-pipeline/cvcuda/src:nvcv_types",
        "//cv-cuda-pipeline/cvcuda/src:nvcv_util",
        "@com_google_googletest//:gtest",
        "@local_cuda//:cuda",
        "@local_cuda//:cuda_runtime",
    ],
    linkopts = ["-lrt"],
    linkstatic = True,
    size = "large",
)

cc_test(
    name = "cvcuda_unit_test",
    srcs = glob(["cvcuda/unit/*.cpp"]) + ["Main.cpp"],
    copts = ["-std=c++20"],
    defines = ["NVCV_UNIT_TEST=1"],
    deps = [
        ":nvcv_test_common",
        ":nvcv_test_headers",
        ":nvcv_test_main",
        "//cv-cuda-pipeline/cvcuda/src:cvcuda_headers",
        "//cv-cuda-pipeline/cvcuda/src:cvcuda_priv",
        "//cv-cuda-pipeline/cvcuda/src:cvcuda_util",
        "//cv-cuda-pipeline/cvcuda/src:nvcv_types",
        "//cv-cuda-pipeline/cvcuda/src:nvcv_util",
        "@com_google_googletest//:gtest",
        "@local_cuda//:cuda",
        "@local_cuda//:cuda_runtime",
    ],
    linkopts = ["-lrt"],
    linkstatic = True,
    size = "large",
)

cuda_library(
    name = "cvcuda_system_test_cuda",
    srcs = ["cvcuda/system/GaussianNoiseUtils.cu"],
    hdrs = glob([
        "cvcuda/system/**/*.cuh",
        "cvcuda/system/**/*.hpp",
    ]),
    includes = [
        ".",
        "nvcv_types/system",
        "cvcuda/system",
    ],
    deps = [
        "//cv-cuda-pipeline/cvcuda/src:cvcuda_headers",
        "//cv-cuda-pipeline/cvcuda/src:nvcv_types",
        "@local_cuda//:cuda_runtime",
    ],
)

cc_test(
    name = "cvcuda_system_test",
    srcs = glob([
        "cvcuda/system/*.cpp",
        "cvcuda/system/*.c",
    ], exclude = [
        "cvcuda/system/*_Smoke.cpp",
    ]) + ["Main.cpp"],
    copts = ["-std=c++20"],
    data = glob(["cvcuda/system/testdata/**"]),
    deps = [
        ":cvcuda_system_test_cuda",
        ":nvcv_test_common_system",
        ":nvcv_test_headers",
        ":nvcv_test_main",
        "//cv-cuda-pipeline/cvcuda/src:cvcuda",
        "//cv-cuda-pipeline/cvcuda/src:cvcuda_headers",
        "//cv-cuda-pipeline/cvcuda/src:nvcv_types",
        "@com_google_googletest//:gtest",
        "@local_cuda//:cuda",
        "@local_cuda//:cuda_runtime",
    ],
    linkopts = ["-lrt"],
    linkstatic = True,
    shard_count = 4,
    size = "large",
    timeout = "long",
)

cc_test(
    name = "cvcuda_system_smoke_test",
    srcs = [
        "cvcuda/system/TestOpBndBox_Smoke.cpp",
        "cvcuda/system/TestOpBoxBlur_Smoke.cpp",
        "Main.cpp",
    ],
    copts = ["-std=c++20"],
    deps = [
        ":nvcv_test_common_system",
        ":nvcv_test_headers",
        ":nvcv_test_main",
        "//cv-cuda-pipeline/cvcuda/src:cvcuda",
        "@com_google_googletest//:gtest",
        "@local_cuda//:cuda_runtime",
    ],
    linkopts = ["-lrt"],
    linkstatic = True,
    size = "small",
)
