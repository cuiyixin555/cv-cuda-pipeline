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

load("@rules_cc//cc:cc_binary.bzl", "cc_binary")
load("@rules_cc//cc:cc_library.bzl", "cc_library")
load("@rules_cc//cc:cc_test.bzl", "cc_test")

package(default_visibility = ["//visibility:public"])

licenses(["notice"])

cc_library(
    name = "vocab_utils",
    srcs = [
        "vocab_utils.cc",
    ],
    hdrs = [
        "vocab_utils.h",
    ],
    deps = [
        "//cv-cuda-pipeline/util:resource_util",
        "@com_google_absl//absl/container:node_hash_map",
        "@com_google_absl//absl/strings",
    ],
)

cc_test(
    name = "vocab_utils_test",
    srcs = ["vocab_utils_test.cc"],
    data = [
        "//cv-cuda-pipeline/tasks/testdata/text:vocab_files",
    ],
    deps = [
        ":vocab_utils",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/tasks/cc/core:utils",
        "@com_google_absl//absl/container:node_hash_map",
    ],
)

cc_library(
    name = "text_model_utils",
    srcs = ["text_model_utils.cc"],
    hdrs = ["text_model_utils.h"],
    deps = [
        "//cv-cuda-pipeline/tasks/cc:common",
        "//cv-cuda-pipeline/tasks/cc/components/processors/proto:text_model_type_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/core:model_resources",
        "//cv-cuda-pipeline/tasks/cc/metadata:metadata_extractor",
        "//cv-cuda-pipeline/tasks/metadata:metadata_schema_cc",
        "@com_google_absl//absl/algorithm:container",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@litert//tflite/schema:schema_fbs",
    ],
)

cc_test(
    name = "text_model_utils_test",
    srcs = ["text_model_utils_test.cc"],
    data = [
        "//cv-cuda-pipeline/tasks/testdata/text:bert_text_classifier_models",
        "//cv-cuda-pipeline/tasks/testdata/text:mobilebert_embedding_model",
        "//cv-cuda-pipeline/tasks/testdata/text:regex_embedding_with_metadata",
        "//cv-cuda-pipeline/tasks/testdata/text:text_classifier_models",
        "//cv-cuda-pipeline/tasks/testdata/text:universal_sentence_encoder_qa",
    ],
    deps = [
        ":text_model_utils",
        "//cv-cuda-pipeline/framework/deps:file_path",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/tasks/cc/components/processors/proto:text_model_type_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/core:model_resources",
        "//cv-cuda-pipeline/tasks/cc/core/proto:external_file_cc_proto",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@litert//tflite:test_util",
    ],
)

cc_library(
    name = "vocab_convert_utils",
    srcs = [
        "vocab_convert_utils.cc",
    ],
    hdrs = [
        "vocab_convert_utils.h",
    ],
    visibility = [
        "//cv-cuda-pipeline/python/pybind:__subpackages__",
        "//cv-cuda-pipeline/tasks:__subpackages__",
    ],
    deps = [
        "//cv-cuda-pipeline/framework/deps:file_path",
        "//cv-cuda-pipeline/framework/port:file_helpers",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/util:resource_util",
        "@com_github_nlohmann_json//:json",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/container:node_hash_map",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_sentencepiece//:sentencepiece_model_cc_proto",
        "@com_google_sentencepiece//:sentencepiece_trainer",
    ],
)

cc_library(
    name = "genai_utils",
    srcs = ["genai_utils.cc"],
    hdrs = ["genai_utils.h"],
    deps = [
        "@com_github_nlohmann_json//:json",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
    ],
)

cc_test(
    name = "genai_utils_test",
    srcs = ["genai_utils_test.cc"],
    deps = [
        ":genai_utils",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "@com_google_absl//absl/strings",
    ],
)
