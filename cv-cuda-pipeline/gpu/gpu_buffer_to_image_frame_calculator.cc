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

#include "cv-cuda-pipeline/gpu/gpu_buffer_to_image_frame_calculator.h"

#include <memory>
#include <utility>

#include "absl/status/status.h"
#include "cv-cuda-pipeline/framework/api3/calculator.h"
#include "cv-cuda-pipeline/framework/api3/calculator_context.h"
#include "cv-cuda-pipeline/framework/api3/calculator_contract.h"
#include "cv-cuda-pipeline/framework/formats/image_frame.h"
#include "cv-cuda-pipeline/framework/port/status_macros.h"
#include "cv-cuda-pipeline/gpu/gpu_buffer.h"
#include "cv-cuda-pipeline/gpu/gpu_buffer_format.h"


#include "cv-cuda-pipeline/gpu/gl_calculator_helper.h"

namespace mediapipe::api3 {

class GpuBufferToImageFrameCalculator
    : public Calculator<GpuBufferToImageFrameNode,
                        GpuBufferToImageFrameCalculator> {
 public:
  static absl::Status UpdateContract(
      CalculatorContract<GpuBufferToImageFrameNode>& cc);

  absl::Status Open(CalculatorContext<GpuBufferToImageFrameNode>& cc) override;
  absl::Status Process(
      CalculatorContext<GpuBufferToImageFrameNode>& cc) override;

 private:
  GlCalculatorHelper helper_;
};

/* static */
absl::Status GpuBufferToImageFrameCalculator::UpdateContract(
    CalculatorContract<GpuBufferToImageFrameNode>& cc) {
  // Note: we call this method even on platforms where we don't use the
  // helper, to ensure the calculator's contract is the same. In particular,
  // the helper enables support for the legacy side packet, which several
  // graphs still use.
  return GlCalculatorHelper::UpdateContract(&cc.GetGenericContract());
}

absl::Status GpuBufferToImageFrameCalculator::Open(
    CalculatorContext<GpuBufferToImageFrameNode>& cc) {
  ABSL_RETURN_IF_ERROR(helper_.Open(&cc.GetGenericContext()));
  return absl::OkStatus();
}

absl::Status GpuBufferToImageFrameCalculator::Process(
    CalculatorContext<GpuBufferToImageFrameNode>& cc) {
  if (cc.in.Has<ImageFrame>()) {
    cc.out.Send(cc.in.PacketOrDie<ImageFrame>());
    return absl::OkStatus();
  }

  if (cc.in.Has<GpuBuffer>()) {
    const auto& input = cc.in.GetOrDie<GpuBuffer>();
    helper_.RunInGlContext([this, &input, &cc]() {
      auto src = helper_.CreateSourceTexture(input);
      auto frame = std::make_unique<ImageFrame>(
          ImageFormatForGpuBufferFormat(input.format()), src.width(),
          src.height(), ImageFrame::kGlDefaultAlignmentBoundary);
      helper_.BindFramebuffer(src);
      const auto info = GlTextureInfoForGpuBufferFormat(input.format(), 0,
                                                        helper_.GetGlVersion());
      glReadPixels(0, 0, src.width(), src.height(), info.gl_format,
                   info.gl_type, frame->MutablePixelData());
      cc.out.Send(std::move(frame));
      src.Release();
    });

    return absl::OkStatus();
  }

  return absl::InvalidArgumentError(
      "Input packets must be ImageFrame or GpuBuffer.");
}

}  // namespace mediapipe::api3
