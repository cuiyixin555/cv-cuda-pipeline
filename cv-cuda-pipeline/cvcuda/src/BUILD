load("@rules_cc//cc:cc_library.bzl", "cc_library")
load("@rules_cuda//cuda:defs.bzl", "cuda_library")

package(default_visibility = ["//visibility:public"])

genrule(
    name = "version_headers",
    srcs = [
        "nvcv/cmake/VersionDef.h.in",
        "nvcv/cmake/VersionUtils.h.in",
    ],
    outs = [
        "generated/nvcv/VersionDef.h",
        "generated/nvcv/detail/VersionUtils.h",
        "generated/cvcuda/VersionDef.h",
        "generated/cvcuda/detail/VersionUtils.h",
    ],
    cmd = """
      mkdir -p $(RULEDIR)/generated/nvcv/detail $(RULEDIR)/generated/cvcuda/detail
      sed -e 's/@LIBPREFIX@/NVCV/g' -e 's/@VERSION_MAJOR@/0/g' \\
          -e 's/@VERSION_MINOR@/19/g' -e 's/@VERSION_PATCH@/0/g' \\
          -e 's/@VERSION_TWEAK@/0/g' -e 's/@VERSION_SUFFIX@//g' \\
          -e 's/@VERSION_FULL@/0.19.0/g' $(location nvcv/cmake/VersionDef.h.in) \\
          > $(RULEDIR)/generated/nvcv/VersionDef.h
      sed -e 's/@LIBPREFIX@/NVCV/g' -e 's/@REPO_COMMIT@/3c06df7feb75510c3e4a80dfd6da30b8fbed0dd6/g' \\
          $(location nvcv/cmake/VersionUtils.h.in) \\
          > $(RULEDIR)/generated/nvcv/detail/VersionUtils.h
      sed -e 's/@LIBPREFIX@/CVCUDA/g' -e 's/@VERSION_MAJOR@/0/g' \\
          -e 's/@VERSION_MINOR@/19/g' -e 's/@VERSION_PATCH@/0/g' \\
          -e 's/@VERSION_TWEAK@/0/g' -e 's/@VERSION_SUFFIX@//g' \\
          -e 's/@VERSION_FULL@/0.19.0/g' $(location nvcv/cmake/VersionDef.h.in) \\
          > $(RULEDIR)/generated/cvcuda/VersionDef.h
      sed -e 's/@LIBPREFIX@/CVCUDA/g' -e 's/@REPO_COMMIT@/3c06df7feb75510c3e4a80dfd6da30b8fbed0dd6/g' \\
          $(location nvcv/cmake/VersionUtils.h.in) \\
          > $(RULEDIR)/generated/cvcuda/detail/VersionUtils.h
    """,
)

genrule(
    name = "nvcv_exports",
    srcs = glob(["nvcv/src/*.cpp"]),
    tools = ["nvcv/cmake/CreateExportsFile.cmake"],
    outs = ["generated/nvcv_exports.ldscript"],
    cmd = "cmake -DSOURCES='$(SRCS)' -DVERPREFIX=NVCV -DOUTPUT=$@ " +
          "-P $(location nvcv/cmake/CreateExportsFile.cmake)",
)

cc_library(
    name = "nvcv_util_sanitizer",
    srcs = ["nvcv/util/SanitizerOptions.c"],
    hdrs = ["nvcv/util/Compiler.hpp"],
    includes = ["nvcv/util"],
)

cc_library(
    name = "nvcv_util",
    srcs = [
        "nvcv/util/Assert.cpp",
        "nvcv/util/CheckError.cpp",
        "nvcv/util/String.cpp",
        "nvcv/util/Version.cpp",
    ],
    hdrs = glob([
        "nvcv/util/*.h",
        "nvcv/util/*.hpp",
    ]),
    defines = ["NVCV_DEBUG=1"],
    includes = [
        "generated",
        "nvcv/src/include",
        "nvcv/src",
        "nvcv/util",
    ],
    deps = [
        ":nvcv_types_headers",
        ":nvcv_util_sanitizer",
        "@local_cuda//:cuda_runtime",
    ],
    linkopts = ["-lrt"],
)

cc_library(
    name = "nvcv_types_headers",
    hdrs = glob([
        "nvcv/src/include/nvcv/**/*.h",
        "nvcv/src/include/nvcv/**/*.hpp",
        "nvcv/src/include/nvcv/**/*.inc",
    ]) + [
        ":generated/nvcv/VersionDef.h",
        ":generated/nvcv/detail/VersionUtils.h",
    ],
    includes = [
        "generated",
        "nvcv/src/include",
    ],
)

cc_library(
    name = "nvcv_types_priv",
    srcs = glob(["nvcv/src/priv/*.cpp"]),
    hdrs = glob([
        "nvcv/src/priv/**/*.h",
        "nvcv/src/priv/**/*.hpp",
    ]),
    local_defines = ["NVCV_EXPORTING=1"],
    includes = [
        "generated",
        "nvcv/src/include",
        "nvcv/src",
        "nvcv/util",
        ".",
    ],
    deps = [
        ":nvcv_types_headers",
        ":nvcv_util",
        ":nvcv_util_sanitizer",
        "@local_cuda//:cuda_runtime",
    ],
    linkopts = ["-lrt"],
)

cc_library(
    name = "nvcv_types",
    srcs = glob(["nvcv/src/*.cpp"]),
    local_defines = ["NVCV_EXPORTING=1"],
    includes = [
        "generated",
        "nvcv/src/include",
        "nvcv/src",
        "nvcv/util",
    ],
    deps = [
        ":nvcv_types_headers",
        ":nvcv_types_priv",
        ":nvcv_util",
    ],
    linkstatic = True,
    linkopts = ["-lrt"],
)

cc_library(
    name = "cvcuda_headers",
    hdrs = glob([
        "cvcuda/include/cvcuda/**/*.h",
        "cvcuda/include/cvcuda/**/*.hpp",
    ]) + [
        ":generated/cvcuda/VersionDef.h",
        ":generated/cvcuda/detail/VersionUtils.h",
    ],
    includes = [
        "generated",
        "nvcv/src/include",
        "cvcuda/include",
    ],
    deps = [":nvcv_types_headers"],
)

cc_library(
    name = "cvcuda_util",
    srcs = glob(["cvcuda/util/*.cpp"]),
    hdrs = glob([
        "cvcuda/util/*.h",
        "cvcuda/util/*.hpp",
    ]),
    includes = [
        "generated",
        "nvcv/src/include",
        "nvcv/src",
        "nvcv/util",
        "cvcuda/include",
        "cvcuda/util",
    ],
    deps = [
        ":nvcv_types",
        ":nvcv_util",
        "@local_cuda//:cuda_runtime",
    ],
    linkopts = ["-lrt"],
)

cuda_library(
    name = "cvcuda_priv",
    srcs = glob([
        "cvcuda/priv/**/*.cpp",
        "cvcuda/priv/**/*.cu",
    ]),
    hdrs = glob([
        "cvcuda/priv/**/*.h",
        "cvcuda/priv/**/*.hpp",
        "cvcuda/priv/**/*.cuh",
    ]),
    local_defines = [
        "ENABLE_TEXT_BACKEND_STB",
        "NVCV_EXPORTING=1",
    ],
    includes = [
        ".",
        "generated",
        "nvcv/src/include",
        "nvcv/src",
        "nvcv/util",
        "cvcuda/include",
        "cvcuda/priv",
        "cvcuda/util",
    ],
    deps = [
        ":cvcuda_headers",
        ":cvcuda_util",
        ":nvcv_types",
        ":nvcv_util",
        ":nvcv_util_sanitizer",
        "@local_cuda//:cuda_runtime",
    ],
    host_linkopts = [
        "-L/usr/local/cuda/lib64",
        "-lcusolver",
        "-lcublas",
        "-lcublasLt",
        "-lrt",
    ],
)

cc_library(
    name = "cvcuda",
    srcs = glob(["cvcuda/*.cpp"]),
    local_defines = ["CVCUDA_EXPORTING=1"],
    includes = [
        ".",
        "generated",
        "nvcv/src/include",
        "nvcv/src",
        "nvcv/util",
        "cvcuda/include",
        "cvcuda/priv",
        "cvcuda/util",
    ],
    deps = [
        ":cvcuda_headers",
        ":cvcuda_priv",
        ":cvcuda_util",
        ":nvcv_types",
        ":nvcv_util",
        "@local_cuda//:cuda_runtime",
    ],
    linkstatic = True,
    linkopts = ["-lrt"],
)
