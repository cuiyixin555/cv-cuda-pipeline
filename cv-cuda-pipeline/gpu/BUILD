# Copyright 2019 The MediaPipe Authors.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

load("@rules_cc//cc:cc_test.bzl", "cc_test")
load("@rules_cc//cc:cc_library.bzl", "cc_library")
load("@bazel_skylib//lib:selects.bzl", "selects")  # buildifier: disable=out-of-order-load
load("//cv-cuda-pipeline/framework/port:build_config.bzl", "mediapipe_proto_library")  # buildifier: disable=out-of-order-load
load("//cv-cuda-pipeline/framework:mediapipe_cc_test.bzl", "mediapipe_cc_test")  # buildifier: disable=out-of-order-load

licenses(["notice"])

package(default_visibility = ["//visibility:public"])

selects.config_setting_group(
    name = "build_for_3p_on_linux-google",
    match_all = [
        "//cv-cuda-pipeline/tasks:build_for_3p",
        "//tools/cc_target_os:linux-google",
    ],
)

# Disabling GPU support is sometimes useful on desktop Linux because SwiftShader can
# interfere with desktop GL. b/73494271
# Also disable GPU on Windows since it's not supported (use pthread).
selects.config_setting_group(
    name = "disable_gpu",
    match_any = [
        ":disable_gpu_flag",
        # The GPU library here use pthread which is not available on Windows.
        "@platforms//os:windows",
    ],
    visibility = ["//visibility:public"],
)

config_setting(
    name = "disable_gpu_flag",
    define_values = {
        "MEDIAPIPE_DISABLE_GPU": "1",
    },
)

cc_library(
    name = "gpu_service",
    srcs = ["gpu_service.cc"],
    hdrs = ["gpu_service.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//cv-cuda-pipeline/framework:graph_service",
        "@com_google_absl//absl/base:core_headers",
    ] + select({
        "//conditions:default": [
            ":gpu_shared_data_internal",
        ],
        "//cv-cuda-pipeline/gpu:disable_gpu": [],
    }),
)

cc_library(
    name = "graph_support",
    hdrs = ["graph_support.h"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "attachments",
    hdrs = ["attachments.h"],
    visibility = ["//visibility:public"],
)

GL_BASE_LINK_OPTS = select({
    "//conditions:default": [],
    "//cv-cuda-pipeline:emscripten": [
        # Needed for symbols such as glFenceSync.
        "-s MAX_WEBGL_VERSION=2",
    ],
})

# This is @unused internally.
GL_BASE_LINK_OPTS_OSS = GL_BASE_LINK_OPTS + select({
    "//conditions:default": [
        # Use GLES/EGL on linux.
        # Requires support from graphics card driver (nvidia,mesa,etc..)
        # and libraries to be installed.
        # Ex: libegl1-mesa-dev libgles2-mesa-dev, or libegl1-nvidia libgles2-nvidia, etc...
    ],
    ":disable_gpu": [],
})

cc_library(
    name = "gl_base",
    defines = [],
    features = ["-layering_check"],
    linkopts = GL_BASE_LINK_OPTS_OSS,
    textual_hdrs = ["gl_base.h"],
    visibility = ["//visibility:public"],
    deps = [":gl_base_hdr"] + select({
        "//cv-cuda-pipeline:emscripten": [],
        "//conditions:default": [
        ],
    }),
)

cc_library(
    name = "egl_base",
    textual_hdrs = ["egl_base.h"],
    visibility = ["//visibility:public"],
    deps = [":gl_base"],
)

cc_library(
    name = "gl_base_hdr",
    hdrs = ["gl_base.h"],
    features = ["-layering_check"],
    linkopts = [],
    visibility = ["//visibility:public"],
    deps = select({
        "//cv-cuda-pipeline:emscripten": [],
        "//conditions:default": [
            "@linux_opencv//:egl_gles_headers",
        ],
    }),
)

cc_library(
    name = "gl_thread_collector",
    hdrs = ["gl_thread_collector.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":gl_base",
        "@com_google_absl//absl/base:core_headers",
        "@com_google_absl//absl/base:log_severity",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/log:absl_log",
        "@com_google_absl//absl/time",
    ],
)

cc_library(
    name = "gl_context",
    srcs = [
        "gl_context.cc",
        "gl_context_internal.h",
    ] + select({
        "//conditions:default": [
            "gl_context_egl.cc",
        ],
        "//cv-cuda-pipeline:emscripten": [
            "gl_context_webgl.cc",
        ],
    }),
    hdrs = ["gl_context.h"],
    copts = [],
    linkopts = [],
    target_compatible_with = select({
        # This cc_library uses pthread and is not compatible with Windows. Marking it as
        # incompatible instead of the confusing message "pthread.h" file not found.
        "@platforms//os:windows": ["//third_party/bazel_platforms:incompatible"],
        "//conditions:default": [],
    }),
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":attachments",
        ":gl_base",
        ":gl_thread_collector",
        ":gpu_buffer_format",
        "//cv-cuda-pipeline/framework:executor",
        "//cv-cuda-pipeline/framework:mediapipe_profiling",
        "//cv-cuda-pipeline/framework:port",
        "//cv-cuda-pipeline/framework:timestamp",
        "//cv-cuda-pipeline/framework/port:logging",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/framework/port:statusor",
        "//cv-cuda-pipeline/framework/port:threadpool",
        "@com_google_absl//absl/base:core_headers",
        "@com_google_absl//absl/base:dynamic_annotations",
        "@com_google_absl//absl/base:no_destructor",
        "@com_google_absl//absl/container:node_hash_map",
        "@com_google_absl//absl/debugging:leak_check",
        "@com_google_absl//absl/log:absl_check",
        "@com_google_absl//absl/log:absl_log",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_absl//absl/strings:string_view",
        "@com_google_absl//absl/synchronization",
        "@com_google_absl//absl/types:span",
    ] + [] + select({
        "//conditions:default": [
        ],
        "//cv-cuda-pipeline:emscripten": [
        ],
    }),
)

cc_library(
    name = "gl_texture_buffer",
    srcs = ["gl_texture_buffer.cc"],
    hdrs = ["gl_texture_buffer.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":gl_base",
        ":gl_context",
        ":gl_texture_view",
        ":gpu_buffer_format",
        ":gpu_buffer_storage",
        ":gpu_buffer_storage_image_frame",
        "@com_google_absl//absl/base:core_headers",
        "@com_google_absl//absl/log:absl_check",
        "@com_google_absl//absl/log:absl_log",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_absl//absl/synchronization",
        # TODO: remove this dependency. Some other teams' tests
        # depend on having an indirect image_frame dependency, need to be
        # fixed first.
        "//cv-cuda-pipeline/framework/formats:image_frame",
    ] + [],
)

cc_library(
    name = "gl_texture_view",
    srcs = ["gl_texture_view.cc"],
    hdrs = ["gl_texture_view.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":gl_base",
        ":gl_context",
        ":gpu_buffer_storage",
    ],
)

cc_library(
    name = "gl_external_texture_view",
    srcs = select({
        "//conditions:default": ["gl_external_texture_view_default.cc"],
    }),
    hdrs = ["gl_external_texture_view.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":gl_context",
        ":gl_texture_view",
        ":gpu_buffer",
        ":gpu_shared_data_internal",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
    ],
)




cc_library(
    name = "gpu_buffer",
    srcs = ["gpu_buffer.cc"],
    hdrs = ["gpu_buffer.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":gpu_buffer_format",
        ":gpu_buffer_storage",
        ":gpu_buffer_storage_image_frame",
        "//cv-cuda-pipeline/framework/formats:image_frame",
        "//cv-cuda-pipeline/framework/port:logging",
        "@com_google_absl//absl/functional:bind_front",
        "@com_google_absl//absl/log:absl_check",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/synchronization",
    ] + select({
        "//conditions:default": [
            ":gl_texture_buffer",
            ":gl_texture_view",
        ],
        ":disable_gpu": [],
    }) + [],
)

cc_library(
    name = "gpu_buffer_format",
    srcs = ["gpu_buffer_format.cc"],
    hdrs = ["gpu_buffer_format.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//cv-cuda-pipeline/framework/deps:no_destructor",
        "//cv-cuda-pipeline/framework/formats:image_format_cc_proto",
        "//cv-cuda-pipeline/framework/port:logging",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/log:absl_check",
    ] + select({
        "//conditions:default": [
            ":gl_base",
        ],
        "//cv-cuda-pipeline/gpu:disable_gpu": [],
    }),
)


cc_library(
    name = "gpu_buffer_storage",
    srcs = ["gpu_buffer_storage.cc"],
    hdrs = ["gpu_buffer_storage.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":gpu_buffer_format",
        "//cv-cuda-pipeline/framework/deps:no_destructor",
        "//cv-cuda-pipeline/framework/formats:image_frame",
        "//cv-cuda-pipeline/framework/tool:type_util",
        "@com_google_absl//absl/container:flat_hash_map",
    ],
)





cc_library(
    name = "gpu_buffer_storage_image_frame",
    srcs = ["gpu_buffer_storage_image_frame.cc"],
    hdrs = ["gpu_buffer_storage_image_frame.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":frame_buffer_view",
        ":gpu_buffer_format",
        ":gpu_buffer_storage",
        ":image_frame_view",
        "//cv-cuda-pipeline/framework/formats:frame_buffer",
        "//cv-cuda-pipeline/framework/formats:image_frame",
        "@com_google_absl//absl/log:absl_check",
    ],
)

cc_library(
    name = "image_frame_view",
    hdrs = ["image_frame_view.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":gpu_buffer_format",
        ":gpu_buffer_storage",
        "//cv-cuda-pipeline/framework/formats:image_frame",
    ],
)

cc_library(
    name = "frame_buffer_view",
    hdrs = ["frame_buffer_view.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":gpu_buffer_storage",
        "//cv-cuda-pipeline/framework/formats:frame_buffer",
    ],
)

cc_library(
    name = "gpu_buffer_storage_yuv_image",
    srcs = ["gpu_buffer_storage_yuv_image.cc"],
    hdrs = ["gpu_buffer_storage_yuv_image.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":frame_buffer_view",
        ":gpu_buffer_format",
        ":gpu_buffer_storage",
        ":image_frame_view",
        "//cv-cuda-pipeline/framework/formats:frame_buffer",
        "//cv-cuda-pipeline/framework/formats:image_frame",
        "//cv-cuda-pipeline/framework/formats:yuv_image",
        "//cv-cuda-pipeline/util/frame_buffer:frame_buffer_util",
        "@com_google_absl//absl/log:absl_check",
        "@com_google_absl//absl/log:absl_log",
        "@com_google_absl//absl/types:span",
        "@libyuv",
    ],
)

mediapipe_proto_library(
    name = "gpu_origin_proto",
    srcs = ["gpu_origin.proto"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "gpu_origin_utils",
    srcs = ["gpu_origin_utils.cc"],
    hdrs = ["gpu_origin_utils.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":gpu_origin_cc_proto",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings:str_format",
    ],
)




cc_library(
    name = "gl_app_texture_support",
    srcs = ["gl_app_texture_support.cc"],
    hdrs = ["gl_app_texture_support.h"],
    deps = [
        ":gl_base",
        ":gl_context",
        ":gl_texture_buffer",
        ":gpu_buffer",
        ":gpu_buffer_format",
        ":gpu_shared_data_header",
        ":multi_pool",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:calculator_graph",
        "//cv-cuda-pipeline/framework:executor",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
    ],
)

mediapipe_proto_library(
    name = "gl_context_options_proto",
    srcs = ["gl_context_options.proto"],
    visibility = ["//visibility:public"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_options_proto",
        "//cv-cuda-pipeline/framework:calculator_proto",
    ],
)

# This is a hack needed to work around some issues with strict hdrs_check.
# See e.g. b/67524270.
cc_library(
    name = "gpu_shared_data_header",
    textual_hdrs = [
        "gpu_shared_data_internal.h",
    ],
    visibility = ["//visibility:private"],
    deps = [
        ":gl_base",
        ":gl_context",
        "@com_google_absl//absl/base:core_headers",
        "@com_google_absl//absl/synchronization",
    ],
)

alias(
    name = "gpu_shared_data_internal",
    actual = select({
        "//conditions:default": ":gpu_shared_data_internal_actual",
        ":disable_gpu": ":gpu_shared_data_internal_stub",
    }),
    visibility = ["//visibility:public"],
)

cc_library(
    name = "gpu_shared_data_internal_stub",
    visibility = ["//visibility:private"],
    deps = [
        ":gl_context_options_cc_proto",
        ":graph_support",
        "//cv-cuda-pipeline/framework:calculator_context",
        "//cv-cuda-pipeline/framework:calculator_node",
        "//cv-cuda-pipeline/framework:executor",
        "//cv-cuda-pipeline/framework:port",
        "//cv-cuda-pipeline/framework/deps:no_destructor",
        "//cv-cuda-pipeline/framework/port:ret_check",
    ],
)

cc_library(
    name = "gpu_shared_data_internal_actual",
    srcs = [
        "gpu_shared_data_internal.cc",
    ],
    hdrs = [
        "gpu_shared_data_internal.h",
    ],
    copts = [],
    visibility = ["//visibility:private"],
    deps = [
        ":gl_base",
        ":gl_context",
        ":gl_context_options_cc_proto",
        ":gpu_buffer_format",
        ":gpu_buffer_multi_pool",
        ":gpu_shared_data_header",
        ":graph_support",
        ":multi_pool",
        "//cv-cuda-pipeline/framework:calculator_context",
        "//cv-cuda-pipeline/framework:calculator_node",
        "//cv-cuda-pipeline/framework:executor",
        "//cv-cuda-pipeline/framework:graph_service",
        "//cv-cuda-pipeline/framework/deps:no_destructor",
        "//cv-cuda-pipeline/framework/port:map_util",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "@com_google_absl//absl/base:core_headers",
        "@com_google_absl//absl/log:absl_check",
        "@com_google_absl//absl/log:absl_log",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:status_macros",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/synchronization",
        "@com_google_absl//absl/types:span",
    ] + [],
)

cc_library(
    name = "gl_texture_buffer_pool",
    srcs = ["gl_texture_buffer_pool.cc"],
    hdrs = ["gl_texture_buffer_pool.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":gl_texture_buffer",
        ":gpu_buffer_format",
        ":multi_pool",
        ":reusable_pool",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_absl//absl/synchronization",
    ],
)

cc_library(
    name = "reusable_pool",
    hdrs = ["reusable_pool.h"],
    deps = [
        ":multi_pool",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "@com_google_absl//absl/base:core_headers",
        "@com_google_absl//absl/functional:any_invocable",
        "@com_google_absl//absl/log:absl_log",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_absl//absl/synchronization",
    ],
)

cc_library(
    name = "multi_pool",
    hdrs = ["multi_pool.h"],
    deps = [
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/util:resource_cache",
        "@com_google_absl//absl/base:core_headers",
        "@com_google_absl//absl/log:absl_log",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/synchronization",
    ],
)

cc_library(
    name = "gpu_buffer_multi_pool",
    srcs = ["gpu_buffer_multi_pool.cc"],
    hdrs = ["gpu_buffer_multi_pool.h"] + [
        ],
    copts = [],
    visibility = ["//visibility:public"],
    deps = [
        ":gl_base",
        ":gpu_buffer",
        ":gpu_buffer_format",
        ":gpu_shared_data_header",
        ":multi_pool",
        "//cv-cuda-pipeline/framework:calculator_context",
        "//cv-cuda-pipeline/framework:calculator_node",
        "//cv-cuda-pipeline/framework/port:logging",
        "//cv-cuda-pipeline/util:resource_cache",
        "@com_google_absl//absl/hash",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/synchronization",
    ] + [
            ":gl_texture_buffer",
            ":gl_texture_buffer_pool",
        ],
)

cc_library(
    name = "egl_errors",
    srcs = ["egl_errors.cc"],
    hdrs = ["egl_errors.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":egl_base",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings",
    ],
)

cc_library(
    name = "egl_sync",
    srcs = ["egl_sync.cc"],
    hdrs = ["egl_sync.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":egl_base",
        ":egl_errors",
        "//cv-cuda-pipeline/framework/deps:no_destructor",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "@com_google_absl//absl/log:absl_log",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
    ],
)

cc_library(
    name = "egl_sync_point",
    srcs = ["egl_sync_point.cc"],
    hdrs = ["egl_sync_point.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":egl_base",
        ":egl_sync",
        ":gl_context",
        "@com_google_absl//absl/log:absl_log",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
    ],
)


cc_library(
    name = "shader_util",
    srcs = ["shader_util.cc"],
    hdrs = ["shader_util.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":gl_base",
        "//cv-cuda-pipeline/framework/port:logging",
        "@com_google_absl//absl/log:absl_check",
        "@com_google_absl//absl/log:absl_log",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
    ],
)

cc_library(
    name = "egl_surface_holder",
    hdrs = ["egl_surface_holder.h"],
    deps = [
        ":gl_base",
        "@com_google_absl//absl/base:core_headers",
        "@com_google_absl//absl/synchronization",
    ],
)

cc_library(
    name = "gl_calculator_helper",
    srcs = [
        "gl_calculator_helper.cc",
    ],
    hdrs = [
        "gl_calculator_helper.h",
    ],
    linkopts = [] + [],
    visibility = ["//visibility:public"],
    deps = [
        ":gl_base",
        ":gl_context",
        ":gl_texture_buffer_pool",
        ":gpu_buffer",
        ":gpu_buffer_format",
        ":gpu_buffer_multi_pool",
        ":gpu_service",
        ":gpu_shared_data_internal",
        ":graph_support",
        ":image_frame_view",
        ":shader_util",
        "//cv-cuda-pipeline/framework:calculator_context",
        "//cv-cuda-pipeline/framework:calculator_contract",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:calculator_node",
        "//cv-cuda-pipeline/framework:demangle",
        "//cv-cuda-pipeline/framework:legacy_calculator_support",
        "//cv-cuda-pipeline/framework:packet",
        "//cv-cuda-pipeline/framework:packet_set",
        "//cv-cuda-pipeline/framework:packet_type",
        "//cv-cuda-pipeline/framework:timestamp",
        "//cv-cuda-pipeline/framework/deps:registration",
        "//cv-cuda-pipeline/framework/formats:image",
        "//cv-cuda-pipeline/framework/formats:image_frame",
        "//cv-cuda-pipeline/framework/port:map_util",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "@com_google_absl//absl/base:core_headers",
        "@com_google_absl//absl/log:absl_check",
        "@com_google_absl//absl/log:absl_log",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/synchronization",
    ] + [
        ],
)


mediapipe_proto_library(
    name = "scale_mode_proto",
    srcs = ["scale_mode.proto"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "gl_quad_renderer",
    srcs = ["gl_quad_renderer.cc"],
    hdrs = ["gl_quad_renderer.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":gl_base",
        ":gl_simple_shaders",
        ":scale_mode_cc_proto",
        ":shader_util",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "@com_google_absl//absl/status",
    ],
)

cc_library(
    name = "gl_simple_shaders",
    srcs = ["gl_simple_shaders.cc"],
    hdrs = ["gl_simple_shaders.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":gl_base",
    ],
)

### General calculator superclasses

cc_library(
    name = "gl_simple_calculator",
    srcs = ["gl_simple_calculator.cc"],
    hdrs = ["gl_simple_calculator.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":gl_calculator_helper",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/port:status",
    ],
)

### Converters

cc_library(
    name = "gpu_buffer_to_image_frame_calculator",
    srcs = ["gpu_buffer_to_image_frame_calculator.cc"],
    hdrs = ["gpu_buffer_to_image_frame_calculator.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":gl_calculator_helper",
        ":gpu_buffer",
        ":gpu_buffer_format",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:timestamp",
        "//cv-cuda-pipeline/framework/api3:calculator",
        "//cv-cuda-pipeline/framework/api3:calculator_context",
        "//cv-cuda-pipeline/framework/api3:calculator_contract",
        "//cv-cuda-pipeline/framework/api3:contract",
        "//cv-cuda-pipeline/framework/api3:node",
        "//cv-cuda-pipeline/framework/api3:one_of",
        "//cv-cuda-pipeline/framework/formats:image_frame",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "@com_google_absl//absl/status",
    ] + [],
    alwayslink = 1,
)


cc_library(
    name = "image_frame_to_gpu_buffer_calculator",
    srcs = ["image_frame_to_gpu_buffer_calculator.cc"],
    hdrs = ["image_frame_to_gpu_buffer_calculator.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":gl_calculator_helper",
        ":gpu_buffer",
        ":gpu_buffer_format",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/api3:calculator",
        "//cv-cuda-pipeline/framework/api3:calculator_context",
        "//cv-cuda-pipeline/framework/api3:calculator_contract",
        "//cv-cuda-pipeline/framework/api3:contract",
        "//cv-cuda-pipeline/framework/api3:node",
        "//cv-cuda-pipeline/framework/formats:image_frame",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "@com_google_absl//absl/status",
    ] + [],
    alwayslink = 1,
)


### Simple calculators

mediapipe_proto_library(
    name = "gl_animation_overlay_calculator_proto",
    srcs = ["gl_animation_overlay_calculator.proto"],
    visibility = ["//visibility:public"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_options_proto",
        "//cv-cuda-pipeline/framework:calculator_proto",
    ],
)

mediapipe_proto_library(
    name = "gl_scaler_calculator_proto",
    srcs = ["gl_scaler_calculator.proto"],
    visibility = ["//visibility:public"],
    deps = [
        ":scale_mode_proto",
        "//cv-cuda-pipeline/framework:calculator_options_proto",
        "//cv-cuda-pipeline/framework:calculator_proto",
    ],
)

cc_library(
    name = "gl_scaler_calculator",
    srcs = ["gl_scaler_calculator.cc"],
    visibility = ["//visibility:public"],
    deps = [
        ":gl_calculator_helper",
        ":gl_quad_renderer",
        ":gl_scaler_calculator_cc_proto",
        ":gl_simple_shaders",
        ":gpu_buffer",
        ":gpu_buffer_format",
        ":shader_util",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:port",
        "//cv-cuda-pipeline/framework/formats:image",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/framework/tool:options_util",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
    ],
    alwayslink = 1,
)

mediapipe_proto_library(
    name = "gl_shader_calculator_proto",
    srcs = ["gl_shader_calculator.proto"],
    visibility = ["//visibility:public"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_options_proto",
        "//cv-cuda-pipeline/framework:calculator_proto",
    ],
)

cc_library(
    name = "gl_shader_calculator",
    srcs = ["gl_shader_calculator.cc"],
    hdrs = ["gl_shader_calculator.h"],
    deps = [
        ":gl_calculator_helper",
        ":gl_shader_calculator_cc_proto",
        ":gl_simple_shaders",
        ":gpu_buffer",
        ":gpu_buffer_format",
        ":shader_util",
        "//cv-cuda-pipeline/app/xeno/effect:asset_registry_service",  # mediapipe:google-only(internal library)
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:calculator_options_cc_proto",
        "//cv-cuda-pipeline/framework:timestamp",
        "//cv-cuda-pipeline/framework/api3:any",
        "//cv-cuda-pipeline/framework/api3:calculator",
        "//cv-cuda-pipeline/framework/api3:calculator_context",
        "//cv-cuda-pipeline/framework/api3:calculator_contract",
        "//cv-cuda-pipeline/framework/api3:contract",
        "//cv-cuda-pipeline/framework/api3:node",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/util:resource_util",
        "//third_party/GL:GLES2_headers",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/log:absl_log",
        "@com_google_absl//absl/log:check",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/types:span",
    ],
    alwayslink = 1,
)


cc_library(
    name = "gl_surface_sink_calculator",
    srcs = ["gl_surface_sink_calculator.cc"],
    visibility = ["//visibility:public"],
    deps = [
        ":egl_surface_holder",
        ":gl_base_hdr",
        ":gl_calculator_helper",
        ":gl_quad_renderer",
        ":gl_surface_sink_calculator_cc_proto",
        ":gpu_buffer",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/api2:node",
        "//cv-cuda-pipeline/framework/api2:packet",
        "//cv-cuda-pipeline/framework/api2:port",
        "//cv-cuda-pipeline/framework/formats:image",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "@com_google_absl//absl/log:absl_log",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/synchronization",
    ],
    alwayslink = 1,
)

mediapipe_proto_library(
    name = "gl_surface_sink_calculator_proto",
    srcs = ["gl_surface_sink_calculator.proto"],
    deps = [
        ":scale_mode_proto",
        "//cv-cuda-pipeline/framework:calculator_options_proto",
        "//cv-cuda-pipeline/framework:calculator_proto",
    ],
)



mediapipe_proto_library(
    name = "copy_calculator_proto",
    srcs = ["copy_calculator.proto"],
    visibility = ["//visibility:public"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_options_proto",
        "//cv-cuda-pipeline/framework:calculator_proto",
    ],
)
