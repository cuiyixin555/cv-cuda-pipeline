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
    name = "language_detector",
    srcs = ["language_detector.cc"],
    hdrs = ["language_detector.h"],
    deps = [
        "//cv-cuda-pipeline/framework/api2:builder",
        "//cv-cuda-pipeline/tasks/cc/components/containers:category",
        "//cv-cuda-pipeline/tasks/cc/components/containers:classification_result",
        "//cv-cuda-pipeline/tasks/cc/components/processors:classifier_options",
        "//cv-cuda-pipeline/tasks/cc/core:base_options",
        "//cv-cuda-pipeline/tasks/cc/core:base_task_api",
        "//cv-cuda-pipeline/tasks/cc/core:running_mode",
        "//cv-cuda-pipeline/tasks/cc/core:task_api_factory",
        "//cv-cuda-pipeline/tasks/cc/core:task_runner",
        "//cv-cuda-pipeline/tasks/cc/text/text_classifier:text_classifier_graph",
        "//cv-cuda-pipeline/tasks/cc/text/text_classifier/proto:text_classifier_graph_options_cc_proto",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
    ],
)
