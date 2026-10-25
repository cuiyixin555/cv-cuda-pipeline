# Copyright 2019 The MediaPipe Authors.
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

load("//cv-cuda-pipeline/framework/port:build_config.bzl", "mediapipe_proto_library")
load(
    "//cv-cuda-pipeline/framework/tool:mediapipe_graph.bzl",
    "mediapipe_simple_subgraph",
)

licenses(["notice"])

package(default_visibility = [
    "//cv-cuda-pipeline:__subpackages__",
])

filegroup(
    name = "test_graph",
    srcs = ["test.pbtxt"],
)

exports_files([
    "test.pbtxt",
    "dub_quad_test_subgraph.pbtxt",
    "nested_test_subgraph.pbtxt",
    "single_flow_container_test.pbtxt",
    "dual_flow_container_test.pbtxt",
])

mediapipe_simple_subgraph(
    name = "dub_quad_test_subgraph",
    testonly = True,
    graph = "dub_quad_test_subgraph.pbtxt",
    register_as = "DubQuadTestSubgraph",
    deps = [
        "//cv-cuda-pipeline/framework:test_calculators",
    ],
)

mediapipe_simple_subgraph(
    name = "nested_test_subgraph",
    testonly = True,
    graph = "nested_test_subgraph.pbtxt",
    register_as = "NestedTestSubgraph",
    visibility = ["//cv-cuda-pipeline/framework:__subpackages__"],
    deps = [
        ":dub_quad_test_subgraph",
        "//cv-cuda-pipeline/framework:test_calculators",
    ],
)

mediapipe_proto_library(
    name = "frozen_generator_proto",
    srcs = ["frozen_generator.proto"],
    visibility = ["//cv-cuda-pipeline/framework:__subpackages__"],
    deps = [
        "//cv-cuda-pipeline/framework:packet_generator_proto",
    ],
)
