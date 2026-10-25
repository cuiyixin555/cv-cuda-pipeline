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

package(default_visibility = ["//visibility:public"])

licenses(["notice"])

# The C API of the interactive segmenter. Only the header is open sourced; the
# implementation is internal and linked into the prebuilt C API libraries.
cc_library(
    name = "interactive_segmenter_hdr",
    hdrs = ["interactive_segmenter.h"],
    visibility = ["//cv-cuda-pipeline/tasks/c:__subpackages__"],
    deps = [
        "//cv-cuda-pipeline/tasks/c/core:base_options",
        "//cv-cuda-pipeline/tasks/c/core:common",
        "//cv-cuda-pipeline/tasks/c/core:mp_status",
        "//cv-cuda-pipeline/tasks/c/vision/core:image",
    ],
)
