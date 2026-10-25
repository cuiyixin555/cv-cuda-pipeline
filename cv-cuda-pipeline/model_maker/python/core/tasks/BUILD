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

# Placeholder for internal Python strict library and test compatibility macro.
# Placeholder: load py_test

package(default_visibility = ["//cv-cuda-pipeline:__subpackages__"])

licenses(["notice"])

py_library(
    name = "custom_model",
    srcs = ["custom_model.py"],
    deps = [
        "//cv-cuda-pipeline/model_maker/python/core/data:dataset",
        "//cv-cuda-pipeline/model_maker/python/core/utils:model_util",
        "//cv-cuda-pipeline/model_maker/python/core/utils:quantization",
        "@model_maker_pip_deps_tensorflow//:pkg",
    ],
)


py_library(
    name = "classifier",
    srcs = ["classifier.py"],
    deps = [
        ":custom_model",
        "//cv-cuda-pipeline/model_maker/python/core:hyperparameters",
        "//cv-cuda-pipeline/model_maker/python/core/data:classification_dataset",
        "//cv-cuda-pipeline/model_maker/python/core/data:dataset",
        "//cv-cuda-pipeline/model_maker/python/core/utils:model_util",
        "@model_maker_pip_deps_tensorflow//:pkg",
    ],
)

