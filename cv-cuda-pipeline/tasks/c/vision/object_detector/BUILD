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

OBJECT_DETECTOR_DEPS = [
    "@com_google_absl//absl/log:absl_log",
    "@com_google_absl//absl/status",
    "@com_google_absl//absl/status:statusor",
    "@com_google_absl//absl/log:absl_check",
    "//cv-cuda-pipeline/framework/formats:image",
    "//cv-cuda-pipeline/tasks/c/components/containers:detection_result",
    "//cv-cuda-pipeline/tasks/c/components/containers:detection_result_converter",
    "//cv-cuda-pipeline/tasks/c/core:base_options",
    "//cv-cuda-pipeline/tasks/c/core:base_options_converter",
    "//cv-cuda-pipeline/tasks/c/core:common",
    "//cv-cuda-pipeline/tasks/c/core:mp_status",
    "//cv-cuda-pipeline/tasks/c/core:mp_status_converter",
    "//cv-cuda-pipeline/tasks/c/vision/core:image",
    "//cv-cuda-pipeline/tasks/c/vision/core:image_frame_util",
    "//cv-cuda-pipeline/tasks/c/vision/core:image_processing_options",
    "//cv-cuda-pipeline/tasks/c/vision/core:image_processing_options_converter",
    "//cv-cuda-pipeline/tasks/cc/vision/core:image_processing_options",
    "//cv-cuda-pipeline/tasks/cc/vision/core:running_mode",
    "//cv-cuda-pipeline/tasks/cc/vision/object_detector",
]

cc_library(
    name = "object_detector_lib",
    srcs = ["object_detector.cc"],
    hdrs = ["object_detector.h"],
    visibility = ["//visibility:public"],
    deps = OBJECT_DETECTOR_DEPS,
)

cc_library(
    name = "object_detector_c_lib",
    srcs = ["object_detector.cc"],
    hdrs = ["object_detector.h"],
    visibility = ["//cv-cuda-pipeline/tasks/c:__subpackages__"],
    deps = OBJECT_DETECTOR_DEPS,
    alwayslink = 1,
)

cc_test(
    name = "object_detector_test",
    srcs = ["object_detector_test.cc"],
    data = [
        "//cv-cuda-pipeline/framework/formats:image_frame_opencv",
        "//cv-cuda-pipeline/framework/port:opencv_core",
        "//cv-cuda-pipeline/framework/port:opencv_imgproc",
        "//cv-cuda-pipeline/tasks/testdata/vision:test_images",
        "//cv-cuda-pipeline/tasks/testdata/vision:test_models",
    ],
    linkstatic = 1,
    deps = [
        ":object_detector_lib",
        "//cv-cuda-pipeline/framework/deps:file_path",
        "//cv-cuda-pipeline/framework/port:gtest",
        "//cv-cuda-pipeline/tasks/c/components/containers:category",
        "//cv-cuda-pipeline/tasks/c/core:common",
        "//cv-cuda-pipeline/tasks/c/core:mp_status",
        "//cv-cuda-pipeline/tasks/c/test:test_utils",
        "//cv-cuda-pipeline/tasks/c/vision/core:image",
        "//cv-cuda-pipeline/tasks/c/vision/core:image_processing_options",
        "//cv-cuda-pipeline/tasks/c/vision/core:image_test_util",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/synchronization",
        "@com_google_absl//absl/time",
        "@com_google_googletest//:gtest_main",
    ],
)

# bazel build -c opt --linkopt -s --strip always --define MEDIAPIPE_DISABLE_GPU=1 \
# //cv-cuda-pipeline/tasks/c/vision/object_detector:libobject_detector.so
cc_binary(
    name = "libobject_detector.so",
    linkopts = [
        "-Wl,-soname=libobject_detector.so",
        "-fvisibility=hidden",
        "-Wl,--export-dynamic-symbol=MpObjectDetectorCreate",
        "-Wl,--export-dynamic-symbol=MpObjectDetectorDetectImage",
        "-Wl,--export-dynamic-symbol=MpObjectDetectorDetectForVideo",
        "-Wl,--export-dynamic-symbol=MpObjectDetectorDetectAsync",
        "-Wl,--export-dynamic-symbol=MpObjectDetectorCloseResult",
        "-Wl,--export-dynamic-symbol=MpObjectDetectorClose",
    ],
    linkshared = True,
    tags = [
        "manual",
        "nobuilder",
        "notap",
    ],
    deps = [":object_detector_lib"],
)

# bazel build --config darwin_arm64 -c opt --strip always --define MEDIAPIPE_DISABLE_GPU=1 \
# //cv-cuda-pipeline/tasks/c/vision/object_detector:libobject_detector.dylib
cc_binary(
    name = "libobject_detector.dylib",
    linkopts = [
        "-Wl,-install_name,libobject_detector.dylib",
        "-fvisibility=hidden",
        "-Wl,-export_symbol,_MpObjectDetectorCreate",
        "-Wl,-export_symbol,_MpObjectDetectorDetectImage",
        "-Wl,-export_symbol,_MpObjectDetectorDetectForVideo",
        "-Wl,-export_symbol,_MpObjectDetectorDetectAsync",
        "-Wl,-export_symbol,_MpObjectDetectorCloseResult",
        "-Wl,-export_symbol,_MpObjectDetectorClose",
    ],
    linkshared = True,
    tags = [
        "manual",
        "nobuilder",
        "notap",
    ],
    deps = [":object_detector_lib"],
)
