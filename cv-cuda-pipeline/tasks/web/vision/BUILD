# This contains the MediaPipe Vision Tasks.

load("//cv-cuda-pipeline/framework/port:build_config.bzl", "ts_library")
load(
    "//cv-cuda-pipeline/framework/tool:mediapipe_files.bzl",
    "mediapipe_files",
)
load("//cv-cuda-pipeline/tasks/web:build_defs.bzl", "pkg_npm", "rollup_bundle")

package(default_visibility = ["//cv-cuda-pipeline/tasks:internal"])

mediapipe_files(srcs = [
    "wasm/vision_wasm_internal.js",
    "wasm/vision_wasm_internal.wasm",
    "wasm/vision_wasm_module_internal.js",
    "wasm/vision_wasm_module_internal.wasm",
    "wasm/vision_wasm_nosimd_internal.js",
    "wasm/vision_wasm_nosimd_internal.wasm",
])

VISION_LIBS = [
    "//cv-cuda-pipeline/tasks/web/core:fileset_resolver",
    "//cv-cuda-pipeline/tasks/web/vision/core:drawing_utils",
    "//cv-cuda-pipeline/tasks/web/vision/core:image",
    "//cv-cuda-pipeline/tasks/web/vision/core:mask",
    "//cv-cuda-pipeline/tasks/web/vision/face_detector",
    "//cv-cuda-pipeline/tasks/web/vision/face_landmarker",
    "//cv-cuda-pipeline/tasks/web/vision/gesture_recognizer",
    "//cv-cuda-pipeline/tasks/web/vision/hand_landmarker",
    "//cv-cuda-pipeline/tasks/web/vision/holistic_landmarker",
    "//cv-cuda-pipeline/tasks/web/vision/image_classifier",
    "//cv-cuda-pipeline/tasks/web/vision/image_embedder",
    "//cv-cuda-pipeline/tasks/web/vision/image_segmenter",
    "//cv-cuda-pipeline/tasks/web/vision/interactive_segmenter_legacy",
    "//cv-cuda-pipeline/tasks/web/vision/interactive_segmenter",
    "//cv-cuda-pipeline/tasks/web/vision/object_detector",
    "//cv-cuda-pipeline/tasks/web/vision/pose_landmarker",
]

ts_library(
    name = "vision_lib",
    srcs = ["index.ts"],
    visibility = ["//visibility:public"],
    deps = VISION_LIBS,
)

ts_library(
    name = "vision_types",
    srcs = ["types.ts"],
    visibility = ["//visibility:public"],
    deps = VISION_LIBS,
)

rollup_bundle(
    name = "vision_bundle_mjs",
    config_file = "//cv-cuda-pipeline/tasks/web:rollup.config.mjs",
    entry_point = "index.ts",
    format = "esm",
    output_dir = False,
    sourcemap = "true",
    deps = [
        ":vision_lib",
        "//:node_modules/@rollup/plugin-commonjs",
        "//:node_modules/@rollup/plugin-node-resolve",
        "//:node_modules/@rollup/plugin-terser",
        "//:node_modules/google-protobuf",
    ],
)

rollup_bundle(
    name = "vision_bundle_cjs",
    config_file = "//cv-cuda-pipeline/tasks/web:rollup.config.mjs",
    entry_point = "index.ts",
    format = "cjs",
    output_dir = False,
    sourcemap = "true",
    deps = [
        ":vision_lib",
        "//:node_modules/@rollup/plugin-commonjs",
        "//:node_modules/@rollup/plugin-node-resolve",
        "//:node_modules/@rollup/plugin-terser",
        "//:node_modules/google-protobuf",
    ],
)

rollup_bundle(
    name = "vision_bundle_iife",
    args = [
        "--name",
        "Vision",
    ],
    config_file = "//cv-cuda-pipeline/tasks/web:rollup.config.mjs",
    entry_point = "index.ts",
    format = "iife",
    output_dir = False,
    sourcemap = "true",
    deps = [
        ":vision_lib",
        "//:node_modules/@rollup/plugin-commonjs",
        "//:node_modules/@rollup/plugin-node-resolve",
        "//:node_modules/@rollup/plugin-terser",
        "//:node_modules/google-protobuf",
    ],
)

genrule(
    name = "vision_sources",
    srcs = [
        ":vision_bundle_cjs",
        ":vision_bundle_mjs",
        ":vision_bundle_iife",
    ],
    outs = [
        "vision_bundle.cjs",
        "vision_bundle.cjs.map",
        "vision_bundle.mjs",
        "vision_bundle.mjs.map",
        "vision_bundle.js",
        "vision_bundle.js.map",
    ],
    cmd = (
        "for FILE in $(SRCS); do " +
        "  OUT_FILE=$(GENDIR)/cv-cuda-pipeline/tasks/web/vision/$$(" +
        "      basename $$FILE | sed -E 's/_([cm])js\\.js/.\\1js/; s/_iife\\.js/.js/'" +
        "  ); " +
        "  echo $$FILE ; echo $$OUT_FILE ; " +
        "  cp $$FILE $$OUT_FILE ; " +
        "done;"
    ),
)

pkg_npm(
    name = "vision_pkg",
    srcs = ["README.md"],
    package_json = "//cv-cuda-pipeline/tasks/web:package.json",
    substitutions = {
        "__NAME__": "vision",
        "__DESCRIPTION__": "MediaPipe Vision Tasks",
        "__TYPES__": "vision.d.ts",
    },
    deps = [
        "wasm/vision_wasm_internal.js",
        "wasm/vision_wasm_internal.wasm",
        "wasm/vision_wasm_module_internal.js",
        "wasm/vision_wasm_module_internal.wasm",
        "wasm/vision_wasm_nosimd_internal.js",
        "wasm/vision_wasm_nosimd_internal.wasm",
        ":vision_sources",
    ],
)
