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

# Docs for Mediapipe Tasks Text Embedder
# https://developers.google.com/mediapipe/solutions/text/text_embedder

cc_library(
    name = "text_embedder_executor",
    hdrs = ["text_embedder_executor.h"],
    deps = [
        "//cv-cuda-pipeline/tasks/cc/components/containers:embedding_result",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
    ],
)

cc_library(
    name = "graph_text_embedder_executor",
    srcs = ["graph_text_embedder_executor.cc"],
    hdrs = ["graph_text_embedder_executor.h"],
    deps = [
        ":text_embedder_executor",
        ":text_embedder_graph",
        "//cv-cuda-pipeline/calculators/tensor:inference_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/api2:builder",
        "//cv-cuda-pipeline/tasks/cc/components/containers:embedding_result",
        "//cv-cuda-pipeline/tasks/cc/components/containers/proto:embeddings_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/components/processors:embedder_options",
        "//cv-cuda-pipeline/tasks/cc/components/processors/proto:embedder_options_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/core:base_options",
        "//cv-cuda-pipeline/tasks/cc/core:running_mode",
        "//cv-cuda-pipeline/tasks/cc/core:task_runner",
        "//cv-cuda-pipeline/tasks/cc/core/proto:base_options_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/text/text_embedder/proto:text_embedder_graph_options_cc_proto",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
    ],
)

cc_library(
    name = "litert_lm_text_embedder_executor",
    srcs = ["litert_lm_text_embedder_executor.cc"],
    hdrs = ["litert_lm_text_embedder_executor.h"],
    deps = [
        ":text_embedder_executor",
        "//cv-cuda-pipeline/framework:timestamp",
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
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@litert_lm//runtime/core:embedding_engine_impl",
        "@litert_lm//runtime/engine:embedding_engine",
        "@litert_lm//runtime/engine:embedding_engine_settings",
        "@litert_lm//runtime/engine:io_types",
        "@litert_lm//runtime/executor:executor_settings_base",
        "@litert_lm//runtime/util:memory_mapped_file",
    ],
)

cc_library(
    name = "text_embedder",
    srcs = ["text_embedder.cc"],
    hdrs = ["text_embedder.h"],
    deps = [
        ":graph_text_embedder_executor",
        ":litert_lm_text_embedder_executor",
        ":text_embedder_executor",
        "//cv-cuda-pipeline/calculators/tensor:inference_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/api2:builder",
        "//cv-cuda-pipeline/tasks/cc/components/containers:embedding_result",
        "//cv-cuda-pipeline/tasks/cc/components/processors:embedder_options",
        "//cv-cuda-pipeline/tasks/cc/components/processors/proto:embedder_options_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/components/utils:cosine_similarity",
        "//cv-cuda-pipeline/tasks/cc/core:base_options",
        "//cv-cuda-pipeline/tasks/cc/core:base_task_api",
        "//cv-cuda-pipeline/tasks/cc/core:embedding_provider",
        "//cv-cuda-pipeline/tasks/cc/core:running_mode",
        "//cv-cuda-pipeline/tasks/cc/core:task_api_factory",
        "//cv-cuda-pipeline/tasks/cc/core:task_runner",
        "//cv-cuda-pipeline/tasks/cc/core/proto:base_options_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/text/text_embedder/proto:text_embedder_graph_options_cc_proto",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:status_macros",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
    ],
)

cc_library(
    name = "text_embedder_graph",
    srcs = ["text_embedder_graph.cc"],
    deps = [
        "//cv-cuda-pipeline/calculators/tensor:inference_calculator_cc_proto",
        "//cv-cuda-pipeline/calculators/tensor:inference_calculator_cpu",
        "//cv-cuda-pipeline/framework:calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/api2:builder",
        "//cv-cuda-pipeline/framework/api2:port",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/tasks/cc/components/calculators:tensors_to_embeddings_calculator_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/components/containers/proto:embeddings_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/components/processors:embedding_postprocessing_graph",
        "//cv-cuda-pipeline/tasks/cc/components/processors:text_preprocessing_graph",
        "//cv-cuda-pipeline/tasks/cc/components/processors/proto:embedding_postprocessing_graph_options_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/components/processors/proto:text_model_type_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/components/processors/proto:text_preprocessing_graph_options_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/core:external_file_handler",
        "//cv-cuda-pipeline/tasks/cc/core:model_asset_bundle_resources",
        "//cv-cuda-pipeline/tasks/cc/core:model_resources",
        "//cv-cuda-pipeline/tasks/cc/core:model_resources_cache",
        "//cv-cuda-pipeline/tasks/cc/core:model_task_graph",
        "//cv-cuda-pipeline/tasks/cc/core/proto:model_resources_calculator_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/metadata/utils:zip_utils",
        "//cv-cuda-pipeline/tasks/cc/text/text_embedder/proto:text_embedder_graph_options_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/text/utils:text_model_utils",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/log:absl_check",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
    ],
    alwayslink = 1,
)

cc_test(
    name = "text_embedder_test",
    srcs = ["text_embedder_test.cc"],
    data = [
        "//cv-cuda-pipeline/models/embedding_gemma:embedding_gemma.task",
        "//cv-cuda-pipeline/tasks/testdata/text:gecko_model",
        "//cv-cuda-pipeline/tasks/testdata/text:mobilebert_embedding_model",
        "//cv-cuda-pipeline/tasks/testdata/text:regex_embedding_with_metadata",
        "//cv-cuda-pipeline/tasks/testdata/text:universal_sentence_encoder_qa",
    ],
    tags = ["not_run:arm"],
    deps = [
        ":text_embedder",
        "//cv-cuda-pipeline/framework/deps:file_path",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/tasks/cc:common",
        "//cv-cuda-pipeline/tasks/cc/components/containers:embedding_result",
        "//cv-cuda-pipeline/tasks/cc/core:embedding_provider",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:cord",
        "@litert//tflite:test_util",
    ],
)
