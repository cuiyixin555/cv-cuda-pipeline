// Copyright 2019 The MediaPipe Authors.
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

#include "cv-cuda-pipeline/gpu/image_frame_to_gpu_buffer_calculator.h"

#include "absl/status/status.h"
#include "cv-cuda-pipeline/framework/api3/calculator.h"
#include "cv-cuda-pipeline/framework/api3/calculator_context.h"
#include "cv-cuda-pipeline/framework/api3/calculator_contract.h"
#include "cv-cuda-pipeline/framework/api3/contract.h"
#include "cv-cuda-pipeline/framework/port/ret_check.h"
#include "cv-cuda-pipeline/framework/port/status_macros.h"
#include "cv-cuda-pipeline/gpu/gl_calculator_helper.h"
#include "cv-cuda-pipeline/gpu/gpu_buffer.h"


namespace mediapipe::api3 {

class ImageFrameToGpuBufferCalculator
    : public Calculator<ImageFrameToGpuBufferNode,
                        ImageFrameToGpuBufferCalculator> {
 public:
  static absl::Status UpdateContract(
      CalculatorContract<ImageFrameToGpuBufferNode>& cc);
  absl::Status Open(CalculatorContext<ImageFrameToGpuBufferNode>& cc) override;
  absl::Status Process(
      CalculatorContext<ImageFrameToGpuBufferNode>& cc) override;

 private:
  mediapipe::GlCalculatorHelper helper_;
};

// static
absl::Status ImageFrameToGpuBufferCalculator::UpdateContract(
    CalculatorContract<ImageFrameToGpuBufferNode>& cc) {
  // Note: we call this method even on platforms where we don't use the helper,
  // to ensure the calculator's contract is the same. In particular, the helper
  // enables support for the legacy side packet, which several graphs still use.
  return GlCalculatorHelper::UpdateContract(&cc.GetGenericContract());
}

absl::Status ImageFrameToGpuBufferCalculator::Open(
    CalculatorContext<ImageFrameToGpuBufferNode>& cc) {
  ABSL_RETURN_IF_ERROR(helper_.Open(&cc.GetGenericContext()));
  return absl::OkStatus();
}

absl::Status ImageFrameToGpuBufferCalculator::Process(
    CalculatorContext<ImageFrameToGpuBufferNode>& cc) {
  RET_CHECK(cc.image_frame);

  const auto& input = cc.image_frame.GetOrDie();
  helper_.RunInGlContext([this, &input, &cc]() {
    GlTexture dst = helper_.CreateDestinationTexture(input);
    cc.gpu_buffer.Send(dst.GetFrame<GpuBuffer>());
    dst.Release();
  });
  return absl::OkStatus();
}

}  // namespace mediapipe::api3
