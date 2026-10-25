# Copyright 2026 The MediaPipe Authors.
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

load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

package(default_visibility = ["//visibility:public"])

licenses(["notice"])

cc_library(
    name = "audio_embedder",
    srcs = ["audio_embedder.cc"],
    hdrs = ["audio_embedder.h"],
    deps = [
        "//cv-cuda-pipeline/framework:timestamp",
        "//cv-cuda-pipeline/framework/formats:matrix",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/tasks/cc/components/containers:embedding_result",
        "//cv-cuda-pipeline/tasks/cc/components/processors:embedder_options",
        "//cv-cuda-pipeline/tasks/cc/core:base_options",
        "//cv-cuda-pipeline/tasks/cc/core:running_mode",
        "//cv-cuda-pipeline/tasks/cc/core:tasks_logging_factory",
        "//cv-cuda-pipeline/tasks/cc/core/logging:tasks_logger",
        "//cv-cuda-pipeline/tasks/cc/core/logging/factory:logging_factory_hdr",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@litert_lm//runtime/core:embedding_engine_impl",
        "@litert_lm//runtime/engine:embedding_engine",
        "@litert_lm//runtime/engine:embedding_engine_settings",
        "@litert_lm//runtime/engine:io_types",
        "@litert_lm//runtime/executor:embedding_executor_base",
        "@litert_lm//runtime/executor:executor_settings_base",
        "@litert_lm//support/util:io_types",
        "@litert_lm//support/util:memory_mapped_file",
    ],
)

cc_test(
    name = "audio_embedder_test",
    srcs = ["audio_embedder_test.cc"],
    data = [
        "//cv-cuda-pipeline/tasks/testdata/audio:test_audio_clips",
        "//cv-cuda-pipeline/tasks/testdata/audio:test_models",
    ],
    deps = [
        ":audio_embedder",
        "//cv-cuda-pipeline/framework/deps:file_path",
        "//cv-cuda-pipeline/framework/formats:matrix",
        "//cv-cuda-pipeline/framework/port:gtest",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/status",
        "@com_google_googletest//:gtest_main",
        "@litert//tflite:test_util",
    ],
)
