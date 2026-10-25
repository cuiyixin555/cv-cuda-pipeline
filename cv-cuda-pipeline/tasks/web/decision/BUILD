# This contains the MediaPipe Decision Tasks.

load("//cv-cuda-pipeline/framework/port:build_config.bzl", "ts_library")
load(
    "//cv-cuda-pipeline/framework/tool:mediapipe_files.bzl",
    "mediapipe_files",
)
load("//cv-cuda-pipeline/tasks/web:build_defs.bzl", "pkg_npm", "rollup_bundle")

package(default_visibility = ["//cv-cuda-pipeline/tasks:internal"])

DECISION_LIBS = [
    "//cv-cuda-pipeline/tasks/web/core",
    "//cv-cuda-pipeline/tasks/web/core:fileset_resolver",
    "//cv-cuda-pipeline/tasks/web/decision/decision_maker",
    "//cv-cuda-pipeline/tasks/web/decision/decision_maker:decision_maker_model_types",
]

ts_library(
    name = "decision_lib",
    srcs = ["index.ts"],
    visibility = ["//visibility:public"],
    deps = DECISION_LIBS,
)

ts_library(
    name = "decision_types",
    srcs = ["types.ts"],
    visibility = ["//visibility:public"],
    deps = DECISION_LIBS,
)

mediapipe_files(srcs = [
    "wasm/decision_wasm_internal.js",
    "wasm/decision_wasm_internal.wasm",
    "wasm/decision_wasm_module_internal.js",
    "wasm/decision_wasm_module_internal.wasm",
    "wasm/decision_wasm_nosimd_internal.js",
    "wasm/decision_wasm_nosimd_internal.wasm",
])

rollup_bundle(
    name = "decision_bundle_mjs",
    config_file = "//cv-cuda-pipeline/tasks/web:rollup.config.mjs",
    entry_point = "index.ts",
    format = "esm",
    output_dir = False,
    sourcemap = "true",
    deps = [
        ":decision_lib",
        "//:node_modules/@rollup/plugin-commonjs",
        "//:node_modules/@rollup/plugin-node-resolve",
        "//:node_modules/@rollup/plugin-terser",
        "//:node_modules/google-protobuf",
    ],
)

rollup_bundle(
    name = "decision_bundle_cjs",
    config_file = "//cv-cuda-pipeline/tasks/web:rollup.config.mjs",
    entry_point = "index.ts",
    format = "cjs",
    output_dir = False,
    sourcemap = "true",
    deps = [
        ":decision_lib",
        "//:node_modules/@rollup/plugin-commonjs",
        "//:node_modules/@rollup/plugin-node-resolve",
        "//:node_modules/@rollup/plugin-terser",
        "//:node_modules/google-protobuf",
    ],
)

rollup_bundle(
    name = "decision_bundle_iife",
    args = [
        "--name",
        "Decision",
    ],
    config_file = "//cv-cuda-pipeline/tasks/web:rollup.config.mjs",
    entry_point = "index.ts",
    format = "iife",
    output_dir = False,
    sourcemap = "true",
    deps = [
        ":decision_lib",
        "//:node_modules/@rollup/plugin-commonjs",
        "//:node_modules/@rollup/plugin-node-resolve",
        "//:node_modules/@rollup/plugin-terser",
        "//:node_modules/google-protobuf",
    ],
)

# Renames Bazel generated chunks to standerd 3P JS module names.
genrule(
    name = "decision_sources",
    srcs = [
        ":decision_bundle_cjs",
        ":decision_bundle_mjs",
        ":decision_bundle_iife",
    ],
    outs = [
        "decision_bundle.cjs",
        "decision_bundle.cjs.map",
        "decision_bundle.mjs",
        "decision_bundle.mjs.map",
        "decision_bundle.js",
        "decision_bundle.js.map",
    ],
    cmd = (
        "for FILE in $(SRCS); do " +
        "  OUT_FILE=$(GENDIR)/cv-cuda-pipeline/tasks/web/decision/$$(" +
        "      basename $$FILE | sed -E 's/_([cm])js\\.js/.\\1js/; s/_iife\\.js/.js/'" +
        "  ); " +
        "  echo $$FILE ; echo $$OUT_FILE ; " +
        "  cp $$FILE $$OUT_FILE ; " +
        "done;"
    ),
)

pkg_npm(
    name = "decision_pkg",
    srcs = ["README.md"],
    package_json = "//cv-cuda-pipeline/tasks/web:package.json",
    substitutions = {
        "__NAME__": "decision",
        "__DESCRIPTION__": "MediaPipe Decision Tasks",
        "__TYPES__": "decision.d.ts",
    },
    deps = [
        "wasm/decision_wasm_internal.js",
        "wasm/decision_wasm_internal.wasm",
        "wasm/decision_wasm_module_internal.js",
        "wasm/decision_wasm_module_internal.wasm",
        "wasm/decision_wasm_nosimd_internal.js",
        "wasm/decision_wasm_nosimd_internal.wasm",
        ":decision_sources",
    ],
)
