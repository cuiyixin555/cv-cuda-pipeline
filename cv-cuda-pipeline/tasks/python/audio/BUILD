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

# Placeholder for internal Python strict library and test compatibility macro.

package(default_visibility = ["//visibility:public"])

licenses(["notice"])

py_library(
    name = "audio_classifier",
    srcs = [
        "audio_classifier.py",
    ],
    deps = [
        "//cv-cuda-pipeline/tasks/python/audio/core:audio_record",
        "//cv-cuda-pipeline/tasks/python/audio/core:audio_task_running_mode",
        "//cv-cuda-pipeline/tasks/python/components/containers:audio_data",
        "//cv-cuda-pipeline/tasks/python/components/containers:audio_data_c",
        "//cv-cuda-pipeline/tasks/python/components/containers:classification_result",
        "//cv-cuda-pipeline/tasks/python/components/containers:classification_result_c",
        "//cv-cuda-pipeline/tasks/python/components/processors:classifier_options",
        "//cv-cuda-pipeline/tasks/python/components/processors:classifier_options_c",
        "//cv-cuda-pipeline/tasks/python/core:async_result_dispatcher",
        "//cv-cuda-pipeline/tasks/python/core:base_options",
        "//cv-cuda-pipeline/tasks/python/core:base_options_c",
        "//cv-cuda-pipeline/tasks/python/core:mediapipe_c_bindings",
        "//cv-cuda-pipeline/tasks/python/core:mediapipe_c_utils",
        "//cv-cuda-pipeline/tasks/python/core:optional_dependencies",
        "//cv-cuda-pipeline/tasks/python/core:serial_dispatcher",
    ],
)

py_library(
    name = "audio_embedder",
    srcs = [
        "audio_embedder.py",
    ],
    deps = [
        "//cv-cuda-pipeline/tasks/python/audio/core:audio_record",
        "//cv-cuda-pipeline/tasks/python/components/containers:audio_data",
        "//cv-cuda-pipeline/tasks/python/components/containers:audio_data_c",
        "//cv-cuda-pipeline/tasks/python/components/containers:embedding_result",
        "//cv-cuda-pipeline/tasks/python/components/containers:embedding_result_c",
        "//cv-cuda-pipeline/tasks/python/components/utils:cosine_similarity",
        "//cv-cuda-pipeline/tasks/python/core:base_options",
        "//cv-cuda-pipeline/tasks/python/core:base_options_c",
        "//cv-cuda-pipeline/tasks/python/core:mediapipe_c_bindings",
        "//cv-cuda-pipeline/tasks/python/core:mediapipe_c_utils",
        "//cv-cuda-pipeline/tasks/python/core:optional_dependencies",
        "//cv-cuda-pipeline/tasks/python/core:serial_dispatcher",
    ],
)

py_library(
    name = "audio_transcriber",
    srcs = [
        "audio_transcriber.py",
    ],
    deps = [
        "//cv-cuda-pipeline/tasks/python/core:base_options",
        "//cv-cuda-pipeline/tasks/python/core:base_options_c",
        "//cv-cuda-pipeline/tasks/python/core:mediapipe_c_bindings",
        "//cv-cuda-pipeline/tasks/python/core:mediapipe_c_utils",
        "@mediapipe_pip_deps_numpy//:pkg",
    ],
)

py_library(
    name = "text_to_speech",
    srcs = [
        "text_to_speech.py",
    ],
    deps = [
        "//cv-cuda-pipeline/tasks/python/core:base_options",
        "//cv-cuda-pipeline/tasks/python/core:base_options_c",
        "//cv-cuda-pipeline/tasks/python/core:mediapipe_c_bindings",
        "//cv-cuda-pipeline/tasks/python/core:mediapipe_c_utils",
        "@mediapipe_pip_deps_numpy//:pkg",
    ],
)
