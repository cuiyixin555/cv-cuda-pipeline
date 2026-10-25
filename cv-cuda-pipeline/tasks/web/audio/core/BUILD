# This package contains options shared by all MediaPipe Audio Tasks for Web.

load("//cv-cuda-pipeline/framework/port:build_config.bzl", "ts_library")

package(default_visibility = ["//cv-cuda-pipeline/tasks:internal"])

ts_library(
    name = "audio_task_runner",
    srcs = ["audio_task_runner.ts"],
    deps = [
        "//cv-cuda-pipeline/tasks/web/core",
        "//cv-cuda-pipeline/tasks/web/core:task_runner",
        "//cv-cuda-pipeline/web/graph_runner:graph_runner_ts",
    ],
)
