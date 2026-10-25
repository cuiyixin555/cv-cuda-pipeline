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

# Placeholder: load py_binary

package(default_visibility = ["//visibility:public"])

py_binary(
    name = "hand_landmarker_benchmark",
    srcs = ["hand_landmarker_benchmark.py"],
    data = [
        "//cv-cuda-pipeline/tasks/testdata/vision:test_images",
        "//cv-cuda-pipeline/tasks/testdata/vision:test_models",
    ],
    main = "hand_landmarker_benchmark.py",
    deps = [
        "//cv-cuda-pipeline/python:_framework_bindings",
        "//cv-cuda-pipeline/tasks/python/benchmark:benchmark_utils",
        "//cv-cuda-pipeline/tasks/python/benchmark/vision:benchmark",
        "//cv-cuda-pipeline/tasks/python/benchmark/vision/core:base_vision_benchmark_api",
        "//cv-cuda-pipeline/tasks/python/core:base_options",
        "//cv-cuda-pipeline/tasks/python/vision:hand_landmarker",
    ],
)
