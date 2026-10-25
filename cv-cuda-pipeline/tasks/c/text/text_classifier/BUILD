# Copyright 2023 The MediaPipe Authors.
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

licenses(["notice"])

TEXT_CLASSIFIER_DEPS = [
    "@com_google_absl//absl/log:absl_log",
    "@com_google_absl//absl/status",
    "@com_google_absl//absl/log:absl_check",
    "//cv-cuda-pipeline/tasks/c/components/containers:classification_result",
    "//cv-cuda-pipeline/tasks/c/components/containers:classification_result_converter",
    "//cv-cuda-pipeline/tasks/c/components/processors:classifier_options",
    "//cv-cuda-pipeline/tasks/c/components/processors:classifier_options_converter",
    "//cv-cuda-pipeline/tasks/c/core:base_options",
    "//cv-cuda-pipeline/tasks/c/core:base_options_converter",
    "//cv-cuda-pipeline/tasks/c/core:mp_status",
    "//cv-cuda-pipeline/tasks/c/core:mp_status_converter",
    "//cv-cuda-pipeline/tasks/cc/text/text_classifier",
]

cc_library(
    name = "text_classifier_lib",
    srcs = ["text_classifier.cc"],
    hdrs = ["text_classifier.h"],
    visibility = ["//visibility:public"],
    deps = TEXT_CLASSIFIER_DEPS,
)

cc_library(
    name = "text_classifier_c_lib",
    srcs = ["text_classifier.cc"],
    hdrs = ["text_classifier.h"],
    visibility = ["//cv-cuda-pipeline/tasks/c:__subpackages__"],
    deps = TEXT_CLASSIFIER_DEPS,
    alwayslink = 1,
)

# bazel build -c opt --linkopt -s --strip always --define MEDIAPIPE_DISABLE_GPU=1 \
# //cv-cuda-pipeline/tasks/c/text/text_classifier:libtext_classifier.so
cc_binary(
    name = "libtext_classifier.so",
    linkopts = [
        "-Wl,-soname=libtext_classifier.so",
        "-fvisibility=hidden",
    ],
    linkshared = True,
    tags = [
        "manual",
        "nobuilder",
        "notap",
    ],
    deps = [":text_classifier_lib"],
)

# bazel build --config darwin_arm64 -c opt --strip always --define MEDIAPIPE_DISABLE_GPU=1 \
# //cv-cuda-pipeline/tasks/c/text/text_classifier:libtext_classifier.dylib
cc_binary(
    name = "libtext_classifier.dylib",
    linkopts = [
        "-Wl,-install_name,libtext_classifier.dylib",
        "-fvisibility=hidden",
    ],
    linkshared = True,
    tags = [
        "manual",
        "nobuilder",
        "notap",
    ],
    deps = [":text_classifier_lib"],
)

cc_test(
    name = "text_classifier_test",
    srcs = ["text_classifier_test.cc"],
    data = ["//cv-cuda-pipeline/tasks/testdata/text:bert_text_classifier_models"],
    linkstatic = 1,
    deps = [
        ":text_classifier_lib",
        "//cv-cuda-pipeline/framework/deps:file_path",
        "//cv-cuda-pipeline/framework/port:gtest",
        "//cv-cuda-pipeline/tasks/c/components/containers:category",
        "//cv-cuda-pipeline/tasks/c/core:common",
        "//cv-cuda-pipeline/tasks/c/core:mp_status",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/strings",
        "@com_google_googletest//:gtest_main",
    ],
)
