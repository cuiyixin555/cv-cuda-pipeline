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

load("@litert//tflite/core/shims:cc_library_with_tflite.bzl", "cc_library_with_tflite")
load("@rules_cc//cc:cc_library.bzl", "cc_library")

licenses(["notice"])

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "running_mode",
    hdrs = ["running_mode.h"],
    deps = [
        "//cv-cuda-pipeline/tasks/cc/core:running_mode",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
    ],
)

cc_library(
    name = "image_processing_options",
    hdrs = ["image_processing_options.h"],
    deps = [
        "//cv-cuda-pipeline/tasks/cc/components/containers:rect",
    ],
)

cc_library_with_tflite(
    name = "base_vision_task_api",
    hdrs = ["base_vision_task_api.h"],
    tflite_deps = [
        "//cv-cuda-pipeline/tasks/cc/core:base_task_api",
        "//cv-cuda-pipeline/tasks/cc/core:task_api_factory",
        "//cv-cuda-pipeline/tasks/cc/core:task_runner",
        "//cv-cuda-pipeline/tasks/cc/vision/utils:image_tensor_specs",
    ],
    deps = [
        ":image_processing_options",
        ":running_mode",
        "//cv-cuda-pipeline/calculators/core:flow_limiter_calculator",
        "//cv-cuda-pipeline/calculators/tensor:image_to_tensor_calculator_cc_proto",
        "//cv-cuda-pipeline/framework/formats:image",
        "//cv-cuda-pipeline/framework/formats:rect_cc_proto",
        "//cv-cuda-pipeline/tasks/cc/components/containers:rect",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
    ],
)

cc_library_with_tflite(
    name = "vision_task_api_factory",
    hdrs = ["vision_task_api_factory.h"],
    tflite_deps = [
        ":base_vision_task_api",
        "//cv-cuda-pipeline/tasks/cc/core:task_runner",
        "//cv-cuda-pipeline/tasks/cc/core:task_api_factory",
    ],
    deps = [
        ":running_mode",
        "//cv-cuda-pipeline/calculators/core:flow_limiter_calculator",
        "//cv-cuda-pipeline/framework:calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@litert//tflite/core/api:op_resolver",
    ],
)
