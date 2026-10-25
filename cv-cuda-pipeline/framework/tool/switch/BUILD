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
#

load("@rules_cc//cc:cc_library.bzl", "cc_library")

licenses(["notice"])

package(default_visibility = ["//visibility:private"])

cc_library(
    name = "packet_processor",
    hdrs = ["packet_processor.h"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_contract",
        "//cv-cuda-pipeline/framework:collection_item_id",
        "//cv-cuda-pipeline/framework:packet",
        "//cv-cuda-pipeline/framework/port:logging",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
    ],
)

cc_library(
    name = "graph_processor",
    srcs = ["graph_processor.cc"],
    hdrs = ["graph_processor.h"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":packet_processor",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:collection_item_id",
        "//cv-cuda-pipeline/framework:input_stream_shard",
        "//cv-cuda-pipeline/framework:output_stream_shard",
        "//cv-cuda-pipeline/framework:validated_graph_config",
        "//cv-cuda-pipeline/framework/api2:node",
        "//cv-cuda-pipeline/framework/api2:packet",
        "//cv-cuda-pipeline/framework/port:integral_types",
        "//cv-cuda-pipeline/framework/port:logging",
        "//cv-cuda-pipeline/framework/port:ret_check",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/framework/stream_handler:immediate_input_stream_handler",
        "@com_google_absl//absl/synchronization",
    ],
    alwayslink = 1,
)
