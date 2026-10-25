# Copyright 2020-2021 The MediaPipe Authors.
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

load("@pybind11_bazel//:build_defs.bzl", "pybind_library")

licenses(["notice"])

package(default_visibility = ["//cv-cuda-pipeline:__subpackages__"])

pybind_library(
    name = "calculator_graph",
    srcs = ["calculator_graph.cc"],
    hdrs = ["calculator_graph.h"],
    deps = [
        ":util",
        "//cv-cuda-pipeline/framework:calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework:calculator_graph",
        "//cv-cuda-pipeline/framework:packet",
        "//cv-cuda-pipeline/framework/port:map_util",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
        "//cv-cuda-pipeline/framework/tool:calculator_graph_template_cc_proto",
        "@com_google_absl//absl/base:core_headers",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/synchronization",
    ],
)

pybind_library(
    name = "image",
    srcs = ["image.cc"],
    hdrs = ["image.h"],
    deps = [
        ":image_frame_util",
        ":util",
        "//cv-cuda-pipeline/framework:type_map",
        "//cv-cuda-pipeline/framework/formats:image",
        "@com_google_absl//absl/strings:str_format",
        "@stblib//:stb_image",
    ],
)

pybind_library(
    name = "image_frame",
    srcs = ["image_frame.cc"],
    hdrs = ["image_frame.h"],
    deps = [
        ":image_frame_util",
        ":util",
        "//cv-cuda-pipeline/framework:type_map",
    ],
)

pybind_library(
    name = "image_frame_util",
    hdrs = ["image_frame_util.h"],
    deps = [
        ":util",
        "//cv-cuda-pipeline/framework:packet",
        "//cv-cuda-pipeline/framework/formats:image_format_cc_proto",
        "//cv-cuda-pipeline/framework/formats:image_frame",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/strings",
    ],
)

pybind_library(
    name = "matrix",
    srcs = ["matrix.cc"],
    hdrs = ["matrix.h"],
    deps = [
        "//cv-cuda-pipeline/framework/formats:matrix",
    ],
)

pybind_library(
    name = "packet",
    srcs = ["packet.cc"],
    hdrs = ["packet.h"],
    deps = [
        ":util",
        "//cv-cuda-pipeline/framework:packet",
        "//cv-cuda-pipeline/framework:timestamp",
    ],
)

pybind_library(
    name = "packet_creator",
    srcs = ["packet_creator.cc"],
    hdrs = ["packet_creator.h"],
    deps = [
        ":image_frame_util",
        ":util",
        "//cv-cuda-pipeline/framework:packet",
        "//cv-cuda-pipeline/framework/formats:image",
        "//cv-cuda-pipeline/framework/formats:image_frame",
        "//cv-cuda-pipeline/framework/formats:matrix",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/strings",
    ],
)

pybind_library(
    name = "packet_getter",
    srcs = ["packet_getter.cc"],
    hdrs = ["packet_getter.h"],
    deps = [
        ":image_frame_util",
        ":util",
        "//cv-cuda-pipeline/framework:packet",
        "//cv-cuda-pipeline/framework/formats:image",
        "//cv-cuda-pipeline/framework/formats:image_frame",
        "//cv-cuda-pipeline/framework/formats:matrix",
        "@com_google_absl//absl/status:statusor",
    ],
)

pybind_library(
    name = "timestamp",
    srcs = ["timestamp.cc"],
    hdrs = ["timestamp.h"],
    deps = [
        ":util",
        "//cv-cuda-pipeline/framework:timestamp",
        "@com_google_absl//absl/strings",
    ],
)

pybind_library(
    name = "resource_util",
    srcs = ["resource_util.cc"],
    hdrs = ["resource_util.h"],
    deps = [
        "//cv-cuda-pipeline/util:resource_util",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/strings",
    ],
)

pybind_library(
    name = "util",
    hdrs = ["util.h"],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_cc_proto",
        "//cv-cuda-pipeline/framework:timestamp",
        "//cv-cuda-pipeline/framework/port:file_helpers",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings",
    ],
)

pybind_library(
    name = "validated_graph_config",
    srcs = ["validated_graph_config.cc"],
    hdrs = ["validated_graph_config.h"],
    deps = [
        ":util",
        "//cv-cuda-pipeline/framework:validated_graph_config",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
    ],
)

pybind_library(
    name = "model_ckpt_util",
    srcs = ["model_ckpt_util.cc"],
    hdrs = ["model_ckpt_util.h"],
    deps = [
        "//cv-cuda-pipeline/tasks/cc/text/utils:vocab_convert_utils",
        "@odml//odml/infra/genai/inference/ml_drift/llm/tensor_loaders:model_ckpt_util",
        "@odml//odml/infra/genai/inference/utils/xnn_utils:model_ckpt_util",
        "@pybind11_abseil//pybind11_abseil:status_casters",
    ],
)
