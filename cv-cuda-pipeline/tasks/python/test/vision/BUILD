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

# Placeholder for internal Python strict contrib test macro and library compatibility macro.

licenses(["notice"])

py_library(
    name = "proto_utils",
    testonly = True,
    srcs = ["proto_utils.py"],
    deps = [
        "//cv-cuda-pipeline/framework/formats:classification_py_pb2",
        "//cv-cuda-pipeline/framework/formats:detection_py_pb2",
        "//cv-cuda-pipeline/framework/formats:landmark_py_pb2",
        "//cv-cuda-pipeline/framework/formats:location_data_py_pb2",
        "//cv-cuda-pipeline/tasks/cc/components/containers/proto:landmarks_detection_result_py_pb2",
        "//cv-cuda-pipeline/tasks/python/components/containers:bounding_box",
        "//cv-cuda-pipeline/tasks/python/components/containers:category",
        "//cv-cuda-pipeline/tasks/python/components/containers:detections",
        "//cv-cuda-pipeline/tasks/python/components/containers:keypoint",
        "//cv-cuda-pipeline/tasks/python/components/containers:landmark",
        "//cv-cuda-pipeline/tasks/python/components/containers:landmark_detection_result",
        "//cv-cuda-pipeline/tasks/python/components/containers:rect",
        "//cv-cuda-pipeline/tasks/python/test:test_utils",
        "//cv-cuda-pipeline/tasks/python/vision:hand_landmarker",
        "//cv-cuda-pipeline/tasks/python/vision:pose_landmarker",
        "//third_party/py/google/protobuf",
    ],
)










