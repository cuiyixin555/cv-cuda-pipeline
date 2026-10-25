load("@rules_cc//cc:cc_library.bzl", "cc_library")
load("@rules_cc//cc:cc_test.bzl", "cc_test")

package(default_visibility = ["//visibility:public"])

licenses(["notice"])

cc_library(
    name = "concatenate",
    hdrs = ["concatenate.h"],
    deps = [
        "//cv-cuda-pipeline/calculators/core:concatenate_proto_list_calculator",
        "//cv-cuda-pipeline/calculators/core:concatenate_vector_calculator",
        "//cv-cuda-pipeline/calculators/core:concatenate_vector_calculator_cc_proto",
        "//cv-cuda-pipeline/framework/api2:builder",
        "//cv-cuda-pipeline/framework/api2:port",
        "//cv-cuda-pipeline/framework/formats:body_rig_cc_proto",
        "//cv-cuda-pipeline/framework/formats:landmark_cc_proto",
        "//cv-cuda-pipeline/framework/formats:tensor",
    ],
)

cc_test(
    name = "concatenate_test",
    srcs = ["concatenate_test.cc"],
    deps = [
        ":concatenate",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/api2:builder",
        "//cv-cuda-pipeline/framework/formats:body_rig_cc_proto",
        "//cv-cuda-pipeline/framework/formats:landmark_cc_proto",
        "//cv-cuda-pipeline/framework/formats:tensor",
        "//cv-cuda-pipeline/framework/port:gtest",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
        "//cv-cuda-pipeline/framework/port:status_matchers",
    ],
)

cc_library(
    name = "detections_to_rects",
    srcs = ["detections_to_rects.cc"],
    hdrs = ["detections_to_rects.h"],
    deps = [
        "//cv-cuda-pipeline/calculators/util:alignment_points_to_rects_calculator",
        "//cv-cuda-pipeline/calculators/util:detections_to_rects_calculator",
        "//cv-cuda-pipeline/calculators/util:detections_to_rects_calculator_cc_proto",
        "//cv-cuda-pipeline/framework/api2:builder",
        "//cv-cuda-pipeline/framework/formats:detection_cc_proto",
        "//cv-cuda-pipeline/framework/formats:rect_cc_proto",
    ],
)

cc_test(
    name = "detections_to_rects_test",
    srcs = ["detections_to_rects_test.cc"],
    deps = [
        ":detections_to_rects",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/api2:builder",
        "//cv-cuda-pipeline/framework/formats:detection_cc_proto",
        "//cv-cuda-pipeline/framework/formats:rect_cc_proto",
        "//cv-cuda-pipeline/framework/port:gtest",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
        "//cv-cuda-pipeline/framework/port:status_matchers",
    ],
)

cc_library(
    name = "get_vector_item",
    hdrs = ["get_vector_item.h"],
    deps = [
        "//cv-cuda-pipeline/calculators/core:get_vector_item_calculator",
        "//cv-cuda-pipeline/framework/api2:builder",
        "//cv-cuda-pipeline/framework/api2:port",
        "//cv-cuda-pipeline/framework/formats:classification_cc_proto",
        "//cv-cuda-pipeline/framework/formats:landmark_cc_proto",
        "//cv-cuda-pipeline/framework/formats:rect_cc_proto",
        "@litert//tflite/c:common",
    ],
)

cc_test(
    name = "get_vector_item_test",
    srcs = ["get_vector_item_test.cc"],
    deps = [
        ":get_vector_item",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/api2:builder",
        "//cv-cuda-pipeline/framework/formats:classification_cc_proto",
        "//cv-cuda-pipeline/framework/formats:landmark_cc_proto",
        "//cv-cuda-pipeline/framework/formats:rect_cc_proto",
        "//cv-cuda-pipeline/framework/port:gtest",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
        "//cv-cuda-pipeline/framework/port:status_matchers",
    ],
)

cc_library(
    name = "landmarks_to_detection",
    srcs = ["landmarks_to_detection.cc"],
    hdrs = ["landmarks_to_detection.h"],
    deps = [
        "//cv-cuda-pipeline/calculators/util:landmarks_to_detection_calculator",
        "//cv-cuda-pipeline/framework/api2:builder",
        "//cv-cuda-pipeline/framework/formats:detection_cc_proto",
        "//cv-cuda-pipeline/framework/formats:landmark_cc_proto",
    ],
)

cc_test(
    name = "landmarks_to_detection_test",
    srcs = ["landmarks_to_detection_test.cc"],
    deps = [
        ":landmarks_to_detection",
        "//cv-cuda-pipeline/framework/api2:builder",
        "//cv-cuda-pipeline/framework/formats:detection_cc_proto",
        "//cv-cuda-pipeline/framework/formats:landmark_cc_proto",
        "//cv-cuda-pipeline/framework/port:gtest",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/framework/port:status_matchers",
    ],
)

cc_library(
    name = "landmarks_to_tensor",
    srcs = ["landmarks_to_tensor.cc"],
    hdrs = ["landmarks_to_tensor.h"],
    deps = [
        "//cv-cuda-pipeline/calculators/tensor:landmarks_to_tensor_calculator",
        "//cv-cuda-pipeline/calculators/tensor:landmarks_to_tensor_calculator_cc_proto",
        "//cv-cuda-pipeline/framework/api2:builder",
        "//cv-cuda-pipeline/framework/api2:port",
        "//cv-cuda-pipeline/framework/formats:landmark_cc_proto",
        "//cv-cuda-pipeline/framework/formats:tensor",
        "@com_google_absl//absl/types:span",
    ],
)

cc_test(
    name = "landmarks_to_tensor_test",
    srcs = ["landmarks_to_tensor_test.cc"],
    deps = [
        ":landmarks_to_tensor",
        "//cv-cuda-pipeline/calculators/tensor:landmarks_to_tensor_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/api2:builder",
        "//cv-cuda-pipeline/framework/formats:landmark_cc_proto",
        "//cv-cuda-pipeline/framework/formats:tensor",
        "//cv-cuda-pipeline/framework/port:gtest",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
        "//cv-cuda-pipeline/framework/port:status_matchers",
    ],
)

cc_library(
    name = "landmarks_projection",
    srcs = ["landmarks_projection.cc"],
    hdrs = ["landmarks_projection.h"],
    deps = [
        "//cv-cuda-pipeline/calculators/util:landmark_projection_calculator",
        "//cv-cuda-pipeline/framework/api2:builder",
        "//cv-cuda-pipeline/framework/formats:landmark_cc_proto",
    ],
)

cc_test(
    name = "landmarks_projection_test",
    srcs = ["landmarks_projection_test.cc"],
    deps = [
        ":landmarks_projection",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/api2:builder",
        "//cv-cuda-pipeline/framework/formats:landmark_cc_proto",
        "//cv-cuda-pipeline/framework/port:gtest",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
        "//cv-cuda-pipeline/framework/port:status",
        "//cv-cuda-pipeline/framework/port:status_matchers",
    ],
)

cc_library(
    name = "loopback",
    hdrs = ["loopback.h"],
    deps = [
        "//cv-cuda-pipeline/calculators/core:previous_loopback_calculator",
        "//cv-cuda-pipeline/framework/api2:builder",
        "//cv-cuda-pipeline/framework/api2:port",
    ],
)

cc_test(
    name = "loopback_test",
    srcs = ["loopback_test.cc"],
    deps = [
        ":loopback",
        "//cv-cuda-pipeline/framework/api2:builder",
        "//cv-cuda-pipeline/framework/api2:node",
        "//cv-cuda-pipeline/framework/api2:port",
        "//cv-cuda-pipeline/framework/port:gtest",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
        "//cv-cuda-pipeline/framework/port:status_matchers",
    ],
)

cc_library(
    name = "image_size",
    hdrs = ["image_size.h"],
    deps = [
        "//cv-cuda-pipeline/calculators/image:image_properties_calculator",
        "//cv-cuda-pipeline/framework/api2:builder",
        "//cv-cuda-pipeline/framework/api2:port",
        "//cv-cuda-pipeline/framework/formats:image",
        "//cv-cuda-pipeline/framework/formats:image_frame",
        "//cv-cuda-pipeline/gpu:gpu_buffer",
    ],
)


cc_library(
    name = "merge",
    hdrs = ["merge.h"],
    deps = [
        "//cv-cuda-pipeline/calculators/core:merge_calculator",
        "//cv-cuda-pipeline/framework/api2:builder",
    ],
)

cc_test(
    name = "merge_test",
    srcs = ["merge_test.cc"],
    deps = [
        ":merge",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/api2:builder",
        "//cv-cuda-pipeline/framework/port:gtest",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
    ],
)

cc_library(
    name = "presence",
    hdrs = ["presence.h"],
    deps = [
        "//cv-cuda-pipeline/calculators/core:packet_presence_calculator",
        "//cv-cuda-pipeline/framework/api2:builder",
    ],
)

cc_test(
    name = "presence_test",
    srcs = ["presence_test.cc"],
    deps = [
        ":presence",
        "//cv-cuda-pipeline/framework:calculator_cc_proto",
        "//cv-cuda-pipeline/framework/api2:builder",
        "//cv-cuda-pipeline/framework/port:gtest",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
    ],
)

cc_library(
    name = "rect_transformation",
    srcs = ["rect_transformation.cc"],
    hdrs = ["rect_transformation.h"],
    deps = [
        "//cv-cuda-pipeline/calculators/util:rect_transformation_calculator",
        "//cv-cuda-pipeline/calculators/util:rect_transformation_calculator_cc_proto",
        "//cv-cuda-pipeline/framework/api2:builder",
        "//cv-cuda-pipeline/framework/formats:rect_cc_proto",
        "@com_google_absl//absl/types:optional",
    ],
)

cc_test(
    name = "rect_transformation_test",
    srcs = ["rect_transformation_test.cc"],
    deps = [
        ":rect_transformation",
        "//cv-cuda-pipeline/framework/api2:builder",
        "//cv-cuda-pipeline/framework/formats:rect_cc_proto",
        "//cv-cuda-pipeline/framework/port:gtest",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
    ],
)

cc_library(
    name = "threshold",
    srcs = ["threshold.cc"],
    hdrs = ["threshold.h"],
    deps = [
        "//cv-cuda-pipeline/calculators/util:thresholding_calculator",
        "//cv-cuda-pipeline/calculators/util:thresholding_calculator_cc_proto",
        "//cv-cuda-pipeline/framework/api2:builder",
    ],
)

cc_test(
    name = "threshold_test",
    srcs = ["threshold_test.cc"],
    deps = [
        ":threshold",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/api2:builder",
        "//cv-cuda-pipeline/framework/port:gtest",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
        "//cv-cuda-pipeline/framework/port:status_matchers",
    ],
)

cc_library(
    name = "smoothing",
    srcs = ["smoothing.cc"],
    hdrs = ["smoothing.h"],
    deps = [
        "//cv-cuda-pipeline/calculators/util:landmarks_smoothing_calculator",
        "//cv-cuda-pipeline/calculators/util:landmarks_smoothing_calculator_cc_proto",
        "//cv-cuda-pipeline/calculators/util:multi_landmarks_smoothing_calculator",
        "//cv-cuda-pipeline/calculators/util:multi_world_landmarks_smoothing_calculator",
        "//cv-cuda-pipeline/calculators/util:visibility_smoothing_calculator",
        "//cv-cuda-pipeline/calculators/util:visibility_smoothing_calculator_cc_proto",
        "//cv-cuda-pipeline/framework/api2:builder",
        "//cv-cuda-pipeline/framework/formats:landmark_cc_proto",
        "//cv-cuda-pipeline/framework/formats:rect_cc_proto",
        "@com_google_absl//absl/types:optional",
    ],
)

cc_test(
    name = "smoothing_test",
    srcs = ["smoothing_test.cc"],
    deps = [
        ":smoothing",
        "//cv-cuda-pipeline/calculators/util:landmarks_smoothing_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/api2:builder",
        "//cv-cuda-pipeline/framework/formats:landmark_cc_proto",
        "//cv-cuda-pipeline/framework/formats:rect_cc_proto",
        "//cv-cuda-pipeline/framework/port:gtest",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
        "//cv-cuda-pipeline/framework/port:status_matchers",
        "@com_google_absl//absl/types:optional",
    ],
)

cc_library(
    name = "segmentation_smoothing",
    srcs = ["segmentation_smoothing.cc"],
    hdrs = ["segmentation_smoothing.h"],
    deps = [
        "//cv-cuda-pipeline/calculators/image:segmentation_smoothing_calculator",
        "//cv-cuda-pipeline/calculators/image:segmentation_smoothing_calculator_cc_proto",
        "//cv-cuda-pipeline/framework/api2:builder",
        "//cv-cuda-pipeline/framework/formats:image",
    ],
)

cc_test(
    name = "segmentation_smoothing_test",
    srcs = ["segmentation_smoothing_test.cc"],
    deps = [
        ":segmentation_smoothing",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/api2:builder",
        "//cv-cuda-pipeline/framework/formats:image",
        "//cv-cuda-pipeline/framework/port:gtest",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
        "//cv-cuda-pipeline/framework/port:status_matchers",
    ],
)

cc_library(
    name = "split",
    hdrs = ["split.h"],
    deps = [
        "//cv-cuda-pipeline/calculators/core:split_proto_list_calculator",
        "//cv-cuda-pipeline/calculators/core:split_vector_calculator",
        "//cv-cuda-pipeline/calculators/core:split_vector_calculator_cc_proto",
        "//cv-cuda-pipeline/framework/api2:builder",
        "//cv-cuda-pipeline/framework/api2:port",
        "//cv-cuda-pipeline/framework/formats:body_rig_cc_proto",
        "//cv-cuda-pipeline/framework/formats:classification_cc_proto",
        "//cv-cuda-pipeline/framework/formats:detection_cc_proto",
        "//cv-cuda-pipeline/framework/formats:landmark_cc_proto",
        "//cv-cuda-pipeline/framework/formats:matrix",
        "//cv-cuda-pipeline/framework/formats:rect_cc_proto",
        "//cv-cuda-pipeline/framework/formats:tensor",
        "@litert//tflite/c:common",
    ],
)

cc_test(
    name = "split_test",
    srcs = ["split_test.cc"],
    deps = [
        ":split",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/api2:builder",
        "//cv-cuda-pipeline/framework/formats:landmark_cc_proto",
        "//cv-cuda-pipeline/framework/formats:rect_cc_proto",
        "//cv-cuda-pipeline/framework/formats:tensor",
        "//cv-cuda-pipeline/framework/port:gtest",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
        "//cv-cuda-pipeline/framework/port:status_matchers",
    ],
)

cc_library(
    name = "tensor_to_joints",
    srcs = ["tensor_to_joints.cc"],
    hdrs = ["tensor_to_joints.h"],
    deps = [
        "//cv-cuda-pipeline/calculators/tensor:tensor_to_joints_calculator",
        "//cv-cuda-pipeline/calculators/tensor:tensor_to_joints_calculator_cc_proto",
        "//cv-cuda-pipeline/framework/api2:builder",
        "//cv-cuda-pipeline/framework/api2:port",
        "//cv-cuda-pipeline/framework/formats:body_rig_cc_proto",
        "//cv-cuda-pipeline/framework/formats:tensor",
    ],
)

cc_test(
    name = "tensor_to_joints_test",
    srcs = ["tensor_to_joints_test.cc"],
    deps = [
        ":tensor_to_joints",
        "//cv-cuda-pipeline/calculators/tensor:tensor_to_joints_calculator_cc_proto",
        "//cv-cuda-pipeline/framework:calculator_framework",
        "//cv-cuda-pipeline/framework/api2:builder",
        "//cv-cuda-pipeline/framework/formats:body_rig_cc_proto",
        "//cv-cuda-pipeline/framework/formats:tensor",
        "//cv-cuda-pipeline/framework/port:gtest",
        "//cv-cuda-pipeline/framework/port:gtest_main",
        "//cv-cuda-pipeline/framework/port:parse_text_proto",
        "//cv-cuda-pipeline/framework/port:status_matchers",
    ],
)
