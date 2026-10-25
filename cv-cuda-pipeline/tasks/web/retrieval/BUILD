# This contains the MediaPipe Retrieval Tasks.

load("//cv-cuda-pipeline/framework/port:build_config.bzl", "ts_library")
load(
    "//cv-cuda-pipeline/framework/tool:mediapipe_files.bzl",
    "mediapipe_files",
)
load("//cv-cuda-pipeline/tasks/web:build_defs.bzl", "pkg_npm", "rollup_bundle")

package(default_visibility = ["//cv-cuda-pipeline/tasks:internal"])

RETRIEVAL_LIBS = [
    "//cv-cuda-pipeline/tasks/web/core",
    "//cv-cuda-pipeline/tasks/web/core:fileset_resolver",
    "//cv-cuda-pipeline/tasks/web/retrieval/semantic_retriever",
    "//cv-cuda-pipeline/tasks/web/retrieval/semantic_retriever:semantic_retriever_types",
    "//cv-cuda-pipeline/tasks/web/retrieval/universal_embedder",
    "//cv-cuda-pipeline/tasks/web/retrieval/universal_embedder:universal_embedder_types",
]

ts_library(
    name = "retrieval_lib",
    srcs = ["index.ts"],
    visibility = ["//visibility:public"],
    deps = RETRIEVAL_LIBS,
)

ts_library(
    name = "retrieval_types",
    srcs = ["types.ts"],
    visibility = ["//visibility:public"],
    deps = RETRIEVAL_LIBS,
)

ts_library(
    name = "logging_utils",
    srcs = ["logging_utils.ts"],
    visibility = ["//cv-cuda-pipeline/tasks/web/retrieval:__subpackages__"],
)

mediapipe_files(srcs = [
    "wasm/retrieval_wasm_internal.js",
    "wasm/retrieval_wasm_internal.wasm",
    "wasm/retrieval_wasm_module_internal.js",
    "wasm/retrieval_wasm_module_internal.wasm",
    "wasm/retrieval_wasm_nosimd_internal.js",
    "wasm/retrieval_wasm_nosimd_internal.wasm",
])

rollup_bundle(
    name = "retrieval_bundle_mjs",
    config_file = "//cv-cuda-pipeline/tasks/web:rollup.config.mjs",
    entry_point = "index.ts",
    format = "esm",
    output_dir = False,
    sourcemap = "true",
    deps = [
        ":retrieval_lib",
        "//:node_modules/@rollup/plugin-commonjs",
        "//:node_modules/@rollup/plugin-node-resolve",
        "//:node_modules/@rollup/plugin-terser",
        "//:node_modules/google-protobuf",
    ],
)

rollup_bundle(
    name = "retrieval_bundle_cjs",
    config_file = "//cv-cuda-pipeline/tasks/web:rollup.config.mjs",
    entry_point = "index.ts",
    format = "cjs",
    output_dir = False,
    sourcemap = "true",
    deps = [
        ":retrieval_lib",
        "//:node_modules/@rollup/plugin-commonjs",
        "//:node_modules/@rollup/plugin-node-resolve",
        "//:node_modules/@rollup/plugin-terser",
        "//:node_modules/google-protobuf",
    ],
)

rollup_bundle(
    name = "retrieval_bundle_iife",
    args = [
        "--name",
        "Retrieval",
    ],
    config_file = "//cv-cuda-pipeline/tasks/web:rollup.config.mjs",
    entry_point = "index.ts",
    format = "iife",
    output_dir = False,
    sourcemap = "true",
    deps = [
        ":retrieval_lib",
        "//:node_modules/@rollup/plugin-commonjs",
        "//:node_modules/@rollup/plugin-node-resolve",
        "//:node_modules/@rollup/plugin-terser",
        "//:node_modules/google-protobuf",
    ],
)

# Renames Bazel generated chunks to standerd 3P JS module names.
genrule(
    name = "retrieval_sources",
    srcs = [
        ":retrieval_bundle_cjs",
        ":retrieval_bundle_mjs",
        ":retrieval_bundle_iife",
    ],
    outs = [
        "retrieval_bundle.cjs",
        "retrieval_bundle.cjs.map",
        "retrieval_bundle.mjs",
        "retrieval_bundle.mjs.map",
        "retrieval_bundle.js",
        "retrieval_bundle.js.map",
    ],
    cmd = (
        "for FILE in $(SRCS); do " +
        "  OUT_FILE=$(GENDIR)/cv-cuda-pipeline/tasks/web/retrieval/$$(" +
        "      basename $$FILE | sed -E 's/_([cm])js\\.js/.\\1js/; s/_iife\\.js/.js/'" +
        "  ); " +
        "  echo $$FILE ; echo $$OUT_FILE ; " +
        "  cp $$FILE $$OUT_FILE ; " +
        "done;"
    ),
)

pkg_npm(
    name = "retrieval_pkg",
    srcs = ["README.md"],
    package_json = "//cv-cuda-pipeline/tasks/web:package.json",
    substitutions = {
        "__NAME__": "retrieval",
        "__DESCRIPTION__": "MediaPipe Retrieval Tasks",
        "__TYPES__": "retrieval.d.ts",
    },
    deps = [
        "wasm/retrieval_wasm_internal.js",
        "wasm/retrieval_wasm_internal.wasm",
        "wasm/retrieval_wasm_module_internal.js",
        "wasm/retrieval_wasm_module_internal.wasm",
        "wasm/retrieval_wasm_nosimd_internal.js",
        "wasm/retrieval_wasm_nosimd_internal.wasm",
        ":retrieval_sources",
    ],
)
