# Copyright 2020 The MediaPipe Authors.
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

licenses(["notice"])

package(default_visibility = ["//visibility:public"])

py_library(
    name = "hands",
    srcs = [
        "hands.py",
        "hands_connections.py",
    ],
    data = [
        "//cv-cuda-pipeline/modules/hand_landmark:hand_landmark_full.tflite",
        "//cv-cuda-pipeline/modules/hand_landmark:hand_landmark_lite.tflite",
        "//cv-cuda-pipeline/modules/hand_landmark:hand_landmark_tracking_cpu_graph",
        "//cv-cuda-pipeline/modules/hand_landmark:handedness.txt",
        "//cv-cuda-pipeline/modules/palm_detection:palm_detection_full.tflite",
        "//cv-cuda-pipeline/modules/palm_detection:palm_detection_lite.tflite",
    ],
    srcs_version = "PY3",
    deps = [
        "//cv-cuda-pipeline/calculators/core:constant_side_packet_calculator_py_pb2",
        "//cv-cuda-pipeline/calculators/core:gate_calculator_py_pb2",
        "//cv-cuda-pipeline/calculators/core:split_vector_calculator_py_pb2",
        "//cv-cuda-pipeline/calculators/tensor:image_to_tensor_calculator_py_pb2",
        "//cv-cuda-pipeline/calculators/tensor:inference_calculator_py_pb2",
        "//cv-cuda-pipeline/calculators/tensor:tensors_to_classification_calculator_py_pb2",
        "//cv-cuda-pipeline/calculators/tensor:tensors_to_detections_calculator_py_pb2",
        "//cv-cuda-pipeline/calculators/tensor:tensors_to_landmarks_calculator_py_pb2",
        "//cv-cuda-pipeline/calculators/tflite:ssd_anchors_calculator_py_pb2",
        "//cv-cuda-pipeline/calculators/util:association_calculator_py_pb2",
        "//cv-cuda-pipeline/calculators/util:detections_to_rects_calculator_py_pb2",
        "//cv-cuda-pipeline/calculators/util:logic_calculator_py_pb2",
        "//cv-cuda-pipeline/calculators/util:non_max_suppression_calculator_py_pb2",
        "//cv-cuda-pipeline/calculators/util:rect_transformation_calculator_py_pb2",
        "//cv-cuda-pipeline/calculators/util:thresholding_calculator_py_pb2",
        "//cv-cuda-pipeline/python:solution_base",
    ],
)

py_library(
    name = "drawing_styles",
    srcs = ["drawing_styles.py"],
    srcs_version = "PY3",
    deps = [
        "drawing_utils",
        "face_mesh",
        "hands",
        "pose",
    ],
)

py_library(
    name = "drawing_utils",
    srcs = ["drawing_utils.py"],
    srcs_version = "PY3",
    deps = [
        "//cv-cuda-pipeline/framework/formats:detection_py_pb2",
        "//cv-cuda-pipeline/framework/formats:landmark_py_pb2",
        "//cv-cuda-pipeline/framework/formats:location_data_py_pb2",
    ],
)


