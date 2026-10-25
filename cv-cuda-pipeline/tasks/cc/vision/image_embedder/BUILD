# Copyright 2022 The MediaPipe Authors.
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

load("@rules_cc//cc:cc_library.bzl", "cc_library")
load("@rules_cc//cc:cc_test.bzl", "cc_test")

package(default_visibility = ["//visibility:public"])

licenses(["notice"])

cc_library(
    name = "image_embedder_executor",
    hdrs = ["image_embedder_executor.h"],
    deps = [
        "//cv-cuda-pipeline/framework/formats:image",
        "//cv-cuda-pipeline/framework/formats:rect_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/components/containers:embedding_result",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
    ],
)

cc_library(
    name = "graph_image_embedder_executor",
    srcs = ["graph_image_embedder_executor.cc"],
    hdrs = ["graph_image_embedder_executor.h"],
    deps = [
        ":image_embedder_executor",
        "//cv-cuda-pipeline/framework:packet",
        "//cv-cuda-pipeline/framework/formats:image",
        "//cv-cuda-pipeline/framework/formats:rect_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/components/containers:embedding_result",
        "//cv-cuda-pipeline/tasks/cc/components/containers/proto:embeddings_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/core:task_runner",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
    ],
)

cc_library(
    name = "litert_lm_image_embedder_executor",
    srcs = ["litert_lm_image_embedder_executor.cc"],
    hdrs = ["litert_lm_image_embedder_executor.h"],
    deps = [
        ":image_embedder_executor",
        "//cv-cuda-pipeline/framework:timestamp",
        "//cv-cuda-pipeline/framework/formats:image",
        "//cv-cuda-pipeline/framework/formats:image_frame",
        "//cv-cuda-pipeline/framework/formats:rect_cc_proto",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/tasks/cc/components/containers:embedding_result",
        "//cv-cuda-pipeline/tasks/cc/components/processors:embedder_options",
        "//cv-cuda-pipeline/tasks/cc/core:base_options",
        "//cv-cuda-pipeline/tasks/cc/core:running_mode",
        "//cv-cuda-pipeline/tasks/cc/core:tasks_logging_factory",  # build_cleaner: keep
        "//cv-cuda-pipeline/tasks/cc/core/logging:tasks_logger",
        "//cv-cuda-pipeline/tasks/cc/core/logging/factory:logging_factory_hdr",
        "@com_google_absl//absl/cleanup",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:status_macros",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
        "@litert_lm//runtime/core:embedding_engine_impl",
        "@litert_lm//runtime/engine:embedding_engine",
        "@litert_lm//runtime/engine:embedding_engine_settings",
        "@litert_lm//runtime/engine:io_types",
        "@litert_lm//runtime/executor:executor_settings_base",
        "@litert_lm//runtime/util:memory_mapped_file",
        "@litert_lm//support/util:io_types",
    ],
)

# Docs for Mediapipe Tasks Image Embedder
# https://developers.google.com/mediapipe/solutions/vision/image_embedder
cc_library(
    name = "image_embedder",
    srcs = ["image_embedder.cc"],
    hdrs = ["image_embedder.h"],
    deps = [
        ":graph_image_embedder_executor",
        ":image_embedder_executor",
        ":image_embedder_graph",  # buildcleaner: keep
        ":litert_lm_image_embedder_executor",
        "//cv-cuda-pipeline/framework/api2:builder",
        "//cv-cuda-pipeline/framework/formats:image",
        "//cv-cuda-pipeline/framework/formats:image_frame",
        "//cv-cuda-pipeline/framework/formats:rect_cc_proto",
        "//cv-cuda-pipeline/framework/tool:options_map",
        "//cv-cuda-pipeline/tasks/cc:common",
        "//cv-cuda-pipeline/tasks/cc/components/containers:embedding_result",
        "//cv-cuda-pipeline/tasks/cc/components/containers/proto:embeddings_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/components/processors:embedder_options",
        "//cv-cuda-pipeline/tasks/cc/components/processors/proto:embedder_options_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/components/utils:cosine_similarity",
        "//cv-cuda-pipeline/tasks/cc/core:base_options",
        "//cv-cuda-pipeline/tasks/cc/core:embedding_provider",
        "//cv-cuda-pipeline/tasks/cc/core:utils",
        "//cv-cuda-pipeline/tasks/cc/core/proto:base_options_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/core:base_vision_task_api",
        "//cv-cuda-pipeline/tasks/cc/vision/core:image_processing_options",
        "//cv-cuda-pipeline/tasks/cc/vision/core:running_mode",
        "//cv-cuda-pipeline/tasks/cc/vision/core:vision_task_api_factory",
        "//cv-cuda-pipeline/tasks/cc/vision/image_embedder/proto:image_embedder_graph_options_cc_proto",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:status_macros",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@stblib//:stb_image",
    ],
)

cc_library(
    name = "image_embedder_graph",
    srcs = ["image_embedder_graph.cc"],
    deps = [
        "//cv-cuda-pipeline/calculators/tensor:inference_calculator",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/api2:builder",
        "//cv-cuda-pipeline/framework/api2:port",
        "//cv-cuda-pipeline/framework/formats:image",
        "//cv-cuda-pipeline/framework/formats:rect_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/components/calculators:tensors_to_embeddings_calculator",
        "//cv-cuda-pipeline/tasks/cc/components/containers/proto:embeddings_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/components/processors:embedding_postprocessing_graph",
        "//cv-cuda-pipeline/tasks/cc/components/processors:image_preprocessing_graph",
        "//cv-cuda-pipeline/tasks/cc/components/processors/proto:embedding_postprocessing_graph_options_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/components/processors/proto:image_preprocessing_graph_options_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/core:model_task_graph",
        "//cv-cuda-pipeline/tasks/cc/vision/image_embedder/proto:image_embedder_graph_options_cc_proto",
        "@com_google_absl//absl/status:statusor",
    ],
    alwayslink = 1,
)

# TODO: This test fails in OSS
