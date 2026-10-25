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

# Docs for Mediapipe Tasks Image Classifier
# https://developers.google.com/mediapipe/solutions/vision/image_classifier
cc_library(
    name = "image_classifier",
    srcs = ["image_classifier.cc"],
    hdrs = ["image_classifier.h"],
    deps = [
        ":image_classifier_graph",
        "//cv-cuda-pipeline/framework:packet",
        "//cv-cuda-pipeline/framework:timestamp",
        "//cv-cuda-pipeline/framework/api2:builder",
        "//cv-cuda-pipeline/framework/formats:image",
        "//cv-cuda-pipeline/framework/formats:rect_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/components/containers:classification_result",
        "//cv-cuda-pipeline/tasks/cc/components/containers/proto:classifications_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/components/processors:classifier_options",
        "//cv-cuda-pipeline/tasks/cc/components/processors/proto:classifier_options_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/core:base_options",
        "//cv-cuda-pipeline/tasks/cc/core:task_runner",
        "//cv-cuda-pipeline/tasks/cc/core:utils",
        "//cv-cuda-pipeline/tasks/cc/core/proto:base_options_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/core/proto:inference_subgraph_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/vision/core:base_vision_task_api",
        "//cv-cuda-pipeline/tasks/cc/vision/core:image_processing_options",
        "//cv-cuda-pipeline/tasks/cc/vision/core:running_mode",
        "//cv-cuda-pipeline/tasks/cc/vision/core:vision_task_api_factory",
        "//cv-cuda-pipeline/tasks/cc/vision/image_classifier/proto:image_classifier_graph_options_cc_proto",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
    ],
)

cc_library(
    name = "image_classifier_graph",
    srcs = ["image_classifier_graph.cc"],
    deps = [
        "//cv-cuda-pipeline/calculators/tensor:inference_calculator",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/api2:builder",
        "//cv-cuda-pipeline/framework/api2:port",
        "//cv-cuda-pipeline/framework/formats:image",
        "//cv-cuda-pipeline/framework/formats:rect_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/components/containers/proto:classifications_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/components/processors:classification_postprocessing_graph",
        "//cv-cuda-pipeline/tasks/cc/components/processors:image_preprocessing_graph",
        "//cv-cuda-pipeline/tasks/cc/components/processors/proto:classification_postprocessing_graph_options_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/components/processors/proto:image_preprocessing_graph_options_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/core:model_resources",
        "//cv-cuda-pipeline/tasks/cc/core:model_task_graph",
        "//cv-cuda-pipeline/tasks/cc/vision/image_classifier/proto:image_classifier_graph_options_cc_proto",
        "@com_google_absl//absl/status:statusor",
    ],
    alwayslink = 1,
)

# TODO: This test fails in OSS
