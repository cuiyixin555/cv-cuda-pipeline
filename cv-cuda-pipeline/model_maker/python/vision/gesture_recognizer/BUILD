# Copyright 2022 The MediaPipe Authors.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# Placeholder for internal Python strict binary and library compatibility macro.
# Placeholder: load py_test

licenses(["notice"])

# TODO: Remove the unnecessary test data once the demo data are moved to an open-sourced
# directory.
filegroup(
    name = "testdata",
    srcs = glob([
        "testdata/**",
    ]),
)

py_library(
    name = "constants",
    srcs = ["constants.py"],
    deps = [
        "//cv-cuda-pipeline/model_maker/python/core/utils:file_util",
    ],
)

py_library(
    name = "dataset",
    srcs = ["dataset.py"],
    deps = [
        ":constants",
        ":metadata_writer",
        "//cv-cuda-pipeline/model_maker/python/core/data:classification_dataset",
        "//cv-cuda-pipeline/model_maker/python/core/utils:model_util",
        "//cv-cuda-pipeline/tasks/python/core:base_options",
        "//cv-cuda-pipeline/tasks/python/vision:hand_landmarker",
        "//cv-cuda-pipeline/tasks/python/vision/core:image",
        "@model_maker_pip_deps_tensorflow//:pkg",
    ],
)

# TODO: Remove notsan tag once tasks no longer has race condition issue

py_library(
    name = "hyperparameters",
    srcs = ["hyperparameters.py"],
    deps = [
        "//cv-cuda-pipeline/model_maker/python/core:hyperparameters",
    ],
)

py_library(
    name = "model_options",
    srcs = ["model_options.py"],
)

py_library(
    name = "gesture_recognizer_options",
    srcs = ["gesture_recognizer_options.py"],
    deps = [
        ":hyperparameters",
        ":model_options",
    ],
)

py_library(
    name = "gesture_recognizer",
    srcs = ["gesture_recognizer.py"],
    deps = [
        ":constants",
        ":gesture_recognizer_options",
        ":hyperparameters",
        ":metadata_writer",
        ":model_options",
        "//cv-cuda-pipeline/model_maker/python/core/data:classification_dataset",
        "//cv-cuda-pipeline/model_maker/python/core/tasks:classifier",
        "//cv-cuda-pipeline/model_maker/python/core/utils:loss_functions",
        "//cv-cuda-pipeline/model_maker/python/core/utils:model_util",
        "//cv-cuda-pipeline/tasks/python/metadata/metadata_writers:metadata_writer",
        "@model_maker_pip_deps_tensorflow//:pkg",
    ],
)

######################################################################
# Public target of the MediaPipe Model Maker GestureRecognizer APIs.

# Please see https://developers.google.com/mediapipe/solutions/vision/gesture_recognizer/customize
# for more information about the MediaPipe Model Maker GestureRecognizer APIs.
######################################################################
py_library(
    name = "gesture_recognizer_import",
    srcs = ["__init__.py"],
    visibility = ["//visibility:public"],
    deps = [
        ":dataset",
        ":gesture_recognizer",
        ":gesture_recognizer_options",
        ":hyperparameters",
        ":model_options",
    ],
)

py_library(
    name = "metadata_writer",
    srcs = ["metadata_writer.py"],
    deps = [
        "//cv-cuda-pipeline/tasks/python/metadata/metadata_writers:metadata_writer",
        "//cv-cuda-pipeline/tasks/python/metadata/metadata_writers:model_asset_bundle_utils",
        "@model_maker_pip_deps_tensorflow//:pkg",
    ],
)

# TODO: Remove notsan tag once tasks no longer has race condition issue


py_binary(
    name = "gesture_recognizer_demo",
    srcs = ["gesture_recognizer_demo.py"],
    data = [":testdata"],
    tags = ["requires-net:external"],
    deps = [
        ":gesture_recognizer_import",
        "@model_maker_pip_deps_absl_py//:pkg",
    ],
)
