// Copyright 2020-2021 The MediaPipe Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "cv-cuda-pipeline/python/pybind/calculator_graph.h"
#include "cv-cuda-pipeline/python/pybind/image.h"
#include "cv-cuda-pipeline/python/pybind/image_frame.h"
#include "cv-cuda-pipeline/python/pybind/matrix.h"
#include "cv-cuda-pipeline/python/pybind/model_ckpt_util.h"
#include "cv-cuda-pipeline/python/pybind/packet.h"
#include "cv-cuda-pipeline/python/pybind/packet_creator.h"
#include "cv-cuda-pipeline/python/pybind/packet_getter.h"
#include "cv-cuda-pipeline/python/pybind/resource_util.h"
#include "cv-cuda-pipeline/python/pybind/timestamp.h"
#include "cv-cuda-pipeline/python/pybind/validated_graph_config.h"

namespace mediapipe {
namespace python {

PYBIND11_MODULE(_framework_bindings, m) {
  ResourceUtilSubmodule(&m);
  ImageSubmodule(&m);
  ImageFrameSubmodule(&m);
  MatrixSubmodule(&m);
  TimestampSubmodule(&m);
  PacketSubmodule(&m);
  PacketCreatorSubmodule(&m);
  PacketGetterSubmodule(&m);
  CalculatorGraphSubmodule(&m);
  ValidatedGraphConfigSubmodule(&m);
  ModelCkptUtilModule(&m);
}

}  // namespace python
}  // namespace mediapipe
