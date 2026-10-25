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

load("@rules_cc//cc:cc_library.bzl", "cc_library")
load("//cv-cuda-pipeline/framework/port:build_config.bzl", "ts_declaration", "ts_library")

package(default_visibility = ["//cv-cuda-pipeline/tasks:internal"])

licenses(["notice"])

ts_library(
    name = "semantic_retriever",
    srcs = [
        "default_text_chunker.ts",
        "memory_vector_store.ts",
        "semantic_retriever.ts",
        "semantic_retriever_components.ts",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":semantic_retriever_types",
        "//cv-cuda-pipeline/tasks/web/components/containers:embedding_result",
        "//cv-cuda-pipeline/tasks/web/components/utils:cosine_similarity",
        "//cv-cuda-pipeline/tasks/web/core",
        "//cv-cuda-pipeline/tasks/web/core:task_logger",
        "//cv-cuda-pipeline/tasks/web/core:task_logger_factory",
        "//cv-cuda-pipeline/tasks/web/retrieval:logging_utils",
        "//cv-cuda-pipeline/web/graph_runner:graph_runner_ts",
    ],
)

ts_declaration(
    name = "semantic_retriever_types",
    srcs = [
        "retrieval_options.d.ts",
        "retrieval_record.d.ts",
        "semantic_retriever_result.d.ts",
        "text_chunker.d.ts",
        "vector_store.d.ts",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cv-cuda-pipeline/tasks/web/core",
        "//cv-cuda-pipeline/tasks/web/retrieval/universal_embedder:universal_embedder_types",
    ],
)
