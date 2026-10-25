# This package contains options shared by all MediaPipe Tasks for Web.

load("//cv-cuda-pipeline/framework/port:build_config.bzl", "ts_declaration", "ts_library")
load("//cv-cuda-pipeline/tasks/web:build_defs.bzl", "jasmine_node_test")

package(default_visibility = ["//cv-cuda-pipeline/tasks:internal"])

ts_declaration(
    name = "core",
    srcs = [
        "embedding_provider.d.ts",
        "task_runner_options.d.ts",
        "wasm_fileset.d.ts",
    ],
)

ts_library(
    name = "async_mutex",
    srcs = ["async_mutex.ts"],
)

ts_library(
    name = "async_mutex_test_lib",
    testonly = True,
    srcs = ["async_mutex_test.ts"],
    deps = [":async_mutex"],
)

jasmine_node_test(
    name = "async_mutex_test",
    srcs = [":async_mutex_test_lib"],
)

ts_library(
    name = "task_logger",
    srcs = ["task_logger.ts"],
)

genrule(
    name = "task_logger_factory_gen",
    outs = ["task_logger_factory.ts"],
    cmd = select({
        "//conditions:default": """
            echo "import {DummyTaskLogger} from './task_logger';" > $@
            echo "export function createTasksLogger(" >> $@
            echo "  taskName: string," >> $@
            echo "  runningMode: string," >> $@
            echo "  apiKey?: string," >> $@
            echo "): DummyTaskLogger { return new DummyTaskLogger(); }" >> $@
        """,
    }),
)

ts_library(
    name = "task_logger_factory",
    srcs = ["task_logger_factory.ts"],
    deps = [
        ":task_logger",
    ],
)

ts_library(
    name = "task_runner",
    srcs = ["task_runner.ts"],
    deps = [
        ":core",
        ":task_logger",
        ":task_logger_factory",
        "//cv-cuda-pipeline/calculators/tensor:inference_calculator_jspb_proto",
        "//cv-cuda-pipeline/framework:calculator_jspb_proto",
        "//cv-cuda-pipeline/tasks/cc/core/proto:acceleration_jspb_proto",
        "//cv-cuda-pipeline/tasks/cc/core/proto:base_options_jspb_proto",
        "//cv-cuda-pipeline/tasks/cc/core/proto:external_file_jspb_proto",
        "//cv-cuda-pipeline/tasks/web/genai/llm_inference:model_loading_utils",
        "//cv-cuda-pipeline/web/graph_runner:graph_runner_logging_lib_ts",
        "//cv-cuda-pipeline/web/graph_runner:graph_runner_ts",
        "//cv-cuda-pipeline/web/graph_runner:register_model_resources_graph_service_ts",
    ],
)

genrule(
    name = "run_script_genrule",
    srcs = ["fileset_resolver.ts.template"],
    outs = ["fileset_resolver.ts"],
    cmd = "cp $< $@",
)

ts_library(
    name = "fileset_resolver",
    srcs = ["fileset_resolver.ts"],
    visibility = ["//visibility:public"],
    deps = [":core"],
)

ts_library(
    name = "task_runner_test_utils",
    testonly = True,
    srcs = [
        "task_runner_test_utils.ts",
    ],
    deps = [
        "//cv-cuda-pipeline/framework:calculator_jspb_proto",
        "//cv-cuda-pipeline/web/graph_runner:graph_runner_ts",
        "//cv-cuda-pipeline/web/graph_runner:register_model_resources_graph_service_ts",
    ],
)

ts_library(
    name = "task_runner_test_lib",
    testonly = True,
    srcs = [
        "task_runner_test.ts",
    ],
    deps = [
        ":core",
        ":task_runner",
        ":task_runner_test_utils",
        "//cv-cuda-pipeline/calculators/tensor:inference_calculator_jspb_proto",
        "//cv-cuda-pipeline/gpu:gpu_origin_jspb_proto",
        "//cv-cuda-pipeline/tasks/cc/core/proto:base_options_jspb_proto",
        "//cv-cuda-pipeline/web/graph_runner:graph_runner_ts",
    ],
)

jasmine_node_test(
    name = "task_runner_test",
    srcs = [":task_runner_test_lib"],
)

ts_declaration(
    name = "classifier_options",
    srcs = ["classifier_options.d.ts"],
    deps = [":core"],
)

ts_declaration(
    name = "embedder_options",
    srcs = ["embedder_options.d.ts"],
    deps = [":core"],
)
