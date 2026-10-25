load("@bazel_skylib//lib:selects.bzl", "selects")
load("@rules_cc//cc:cc_library.bzl", "cc_library")
load("@rules_cc//cc:cc_test.bzl", "cc_test")
load("//cv-cuda-pipeline/framework:mediapipe_cc_test.bzl", "mediapipe_cc_test")
load("//cv-cuda-pipeline/framework/port:build_config.bzl", "mediapipe_proto_library")

licenses(["notice"])

package(default_visibility = ["//visibility:public"])

# This can be used to force WebGPU on in desktop builds.
config_setting(
    name = "use_webgpu_define",
    define_values = {"MEDIAPIPE_USE_WEBGPU": "1"},
    visibility = ["//visibility:public"],
)

selects.config_setting_group(
    name = "use_webgpu",
    match_any = [
        ":use_webgpu_define",
    ],
)

selects.config_setting_group(
    name = "use_webgpu_emscripten",
    match_all = [
        ":use_webgpu_define",
        "//cv-cuda-pipeline:emscripten",
    ],
)

exports_files([
    "tensor_webgpu_texture_view.cc",
])

cc_library(
    name = "webgpu_device_registration",
    srcs = select({
        "//cv-cuda-pipeline:emscripten": [],
        "//conditions:default": ["webgpu_device_registration.cc"],
    }),
    hdrs = select({
        "//cv-cuda-pipeline:emscripten": [],
        "//conditions:default": ["webgpu_device_registration.h"],
    }),
    deps = select({
        "//cv-cuda-pipeline:emscripten": [],
        "//conditions:default": [
            "//cv-cuda-pipeline/framework/deps:no_destructor",
            "//third_party/dawn:dawncpp_headers",
        ],
    }),
)

cc_library(
    name = "webgpu_headers",
    hdrs = ["webgpu_headers.h"],
    deps = select({
        "//cv-cuda-pipeline:emscripten": [
            "//third_party/emdawnwebgpu:webgpu",
        ],
        "//conditions:default": [
            "//third_party/dawn:dawn_headers",
            "//third_party/dawn:webgpu_headers",
        ],
    }),
)

cc_library(
    name = "webgpu_check",
    srcs = ["webgpu_check.cc"],
    hdrs = ["webgpu_check.h"],
    deps = select({
        # `:webgpu_device_registration` should only be included if we are on a non-web platform, and
        # `MEDIAPIPE_USE_WEBGPU` is set to true. That is why we added both `:use_webgpu` and
        # `:use_webgpu_define`. Because in `select`, if multiple conditions match and one is a
        # specialization of the others, the specialization takes precedence. So, `:use_webgpu` will
        # filter out `emscripten`.
        ":use_webgpu_define": [":webgpu_device_registration"],
        ":use_webgpu": [],
        "//conditions:default": [],
    }),
)

cc_library(
    name = "webgpu_service",
    srcs = ["webgpu_service.cc"],
    hdrs = ["webgpu_service.h"],
    deps = [
        ":webgpu_check",
        ":webgpu_headers",
        "//cv-cuda-pipeline/framework:graph_service",
        "//cv-cuda-pipeline/framework/deps:no_destructor",
        "//cv-cuda-pipeline/gpu:attachments",
        "//cv-cuda-pipeline/web:jspi_check",
        "//third_party/dawn:webgpu_headers",
        "@com_google_absl//absl/base:core_headers",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/log:absl_check",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/synchronization",
    ] + select({
        "//cv-cuda-pipeline:emscripten": [
        ],
        "//conditions:default": [
            ":webgpu_device_registration",
        ],
    }),
)

cc_library(
    name = "webgpu_texture_view",
    hdrs = ["webgpu_texture_view.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":webgpu_headers",
        "//cv-cuda-pipeline/gpu:gpu_buffer_storage",
    ],
)

cc_library(
    name = "webgpu_texture_buffer",
    srcs = ["webgpu_texture_buffer.cc"],
    hdrs = ["webgpu_texture_buffer.h"],
    tags = ["keep_dep"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":webgpu_headers",
        ":webgpu_service",
        ":webgpu_texture_view",
        "//cv-cuda-pipeline/framework:legacy_calculator_support",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/gpu:gpu_buffer_format",
        "//cv-cuda-pipeline/gpu:gpu_buffer_storage",
        "//third_party/dawn:webgpu_headers",
        "@com_google_absl//absl/base:nullability",
        "@com_google_absl//absl/functional:any_invocable",
        "@com_google_absl//absl/log:absl_check",
        "@com_google_absl//absl/log:absl_log",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings:str_format",
    ] + select({
        "//cv-cuda-pipeline:emscripten": [
        ],
        "//conditions:default": [],
    }),
    alwayslink = 1,
)


cc_library(
    name = "webgpu_external_texture_buffer",
    hdrs = ["webgpu_external_texture_buffer.h"],
    tags = ["keep_dep"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":webgpu_headers",
        "//cv-cuda-pipeline/gpu:gpu_buffer_format",
        "//cv-cuda-pipeline/gpu:gpu_buffer_storage",
    ],
    alwayslink = 1,
)


cc_library(
    name = "webgpu_utils",
    srcs = ["webgpu_utils.cc"],
    hdrs = ["webgpu_utils.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":webgpu_headers",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/web:jspi_check",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/time",
    ],
)

cc_library(
    name = "image_to_tensor_converter_webgpu_texture",
    srcs = ["image_to_tensor_converter_webgpu_texture.cc"],
    hdrs = ["image_to_tensor_converter_webgpu_texture.h"],
    deps = [
        ":webgpu_headers",
        ":webgpu_service",
        ":webgpu_texture_view",
        ":webgpu_utils",
        "//cv-cuda-pipeline/calculators/tensor:image_to_tensor_converter",
        "//cv-cuda-pipeline/calculators/tensor:image_to_tensor_utils",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/formats:tensor",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/framework/port:statusor",
        "//cv-cuda-pipeline/gpu:gpu_buffer",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings:str_format",
    ],
)

cc_library(
    name = "webgpu_shader_calculator",
    srcs = ["webgpu_shader_calculator.cc"],
    hdrs = ["webgpu_shader_calculator.h"],
    deps = [
        ":webgpu_headers",
        ":webgpu_service",
        ":webgpu_shader_calculator_cc_proto",
        ":webgpu_texture_buffer",
        ":webgpu_texture_buffer_3d",
        ":webgpu_texture_view",
        ":webgpu_utils",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:calculator_options_cc_proto",
        "//cv-cuda-pipeline/framework:resources",
        "//cv-cuda-pipeline/framework/api3:any",
        "//cv-cuda-pipeline/framework/api3:calculator",
        "//cv-cuda-pipeline/framework/api3:calculator_context",
        "//cv-cuda-pipeline/framework/api3:calculator_contract",
        "//cv-cuda-pipeline/framework/api3:contract",
        "//cv-cuda-pipeline/framework/api3:node",
        "//cv-cuda-pipeline/framework/deps:re2",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/gpu:gpu_buffer",
        "//cv-cuda-pipeline/gpu:gpu_buffer_format",
        "@com_google_absl//absl/log:absl_log",
        "@com_google_absl//absl/log:check",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_absl//absl/strings:string_view",
    ],
    alwayslink = 1,
)

mediapipe_proto_library(
    name = "webgpu_shader_calculator_proto",
    srcs = ["webgpu_shader_calculator.proto"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_proto",
    ],
)
