// Copyright 2024 The MediaPipe Authors.
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

#ifndef MEDIAPIPE_CALCULATORS_TENSOR_INFERENCE_RUNNER_LITERT_H_
#define MEDIAPIPE_CALCULATORS_TENSOR_INFERENCE_RUNNER_LITERT_H_

#include <cstddef>
#include <cstdint>
#include <deque>
#include <memory>
#include <optional>
#include <vector>

#include "absl/base/nullability.h"
#include "absl/base/thread_annotations.h"
#include "absl/container/flat_hash_map.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/string_view.h"
#include "absl/synchronization/mutex.h"
#include "cv-cuda-pipeline/calculators/tensor/inference_calculator.pb.h"
#include "cv-cuda-pipeline/calculators/tensor/inference_feedback_manager_litert.h"
#include "cv-cuda-pipeline/calculators/tensor/inference_io_mapper.h"
#include "cv-cuda-pipeline/calculators/tensor/inference_runner.h"
#include "cv-cuda-pipeline/calculators/tensor/litert/litert_service.h"
#include "cv-cuda-pipeline/calculators/tensor/tensor_span.h"
#include "cv-cuda-pipeline/framework/api2/packet.h"
#include "cv-cuda-pipeline/framework/calculator_context.h"
#include "cv-cuda-pipeline/framework/calculator_framework.h"
#include "cv-cuda-pipeline/framework/formats/tensor.h"
#include "litert/cc/litert_compiled_model.h"       // from @litert
#include "litert/cc/litert_environment.h"          // from @litert
#include "litert/cc/litert_layout.h"               // from @litert
#include "litert/cc/litert_model_types.h"          // from @litert
#include "litert/cc/litert_options.h"              // from @litert
#include "litert/cc/litert_ranked_tensor_type.h"   // from @litert
#include "litert/cc/litert_tensor_buffer.h"        // from @litert
#include "litert/cc/litert_tensor_buffer_types.h"  // from @litert
#include "cv-cuda-pipeline/framework/memory_manager.h"
#include "cv-cuda-pipeline/framework/port/port.h"
#include "cv-cuda-pipeline/util/tflite/tflite_model_loader.h"

#if MEDIAPIPE_OPENGL_ES_VERSION >= MEDIAPIPE_OPENGL_ES_30
#include "cv-cuda-pipeline/gpu/gl_context.h"
#endif  // MEDIAPIPE_OPENGL_ES_VERSION >= MEDIAPIPE_OPENGL_ES_30


#if defined(__EMSCRIPTEN__)
#include "cv-cuda-pipeline/gpu/webgpu/webgpu_service.h"
#endif  // __EMSCRIPTEN__

namespace mediapipe {

// Forward declaration of GlContext for when the full header is not included.
// This is necessary because GlContext is used in the Create() signature.
#if !(MEDIAPIPE_OPENGL_ES_VERSION >= MEDIAPIPE_OPENGL_ES_30)
class GlContext;
#endif

// Inference runner for LiteRt API.
class InferenceRunnerLiteRt : public InferenceRunner {
 public:
  ~InferenceRunnerLiteRt() override;

  static absl::StatusOr<std::unique_ptr<InferenceRunnerLiteRt>> Create(
      api2::Packet<TfLiteModelPtr> model_packet,
      const mediapipe::InferenceCalculatorOptions::Delegate::LiteRt& options,
      const mediapipe::InferenceCalculatorOptions::
          InputOutputConfig* /*absl_nullable - not yet supported*/
              input_output_config,
      MemoryManager* memory_manager,
      std::shared_ptr<mediapipe::GlContext> gl_context = nullptr,
#if defined(__EMSCRIPTEN__)
      WebGpuService* webgpu_service = nullptr,
#endif  // __EMSCRIPTEN__
      std::optional<litert::Options> litert_options = std::nullopt,
      LiteRtSystemHandles system_handles = {});

  absl::StatusOr<std::vector<Tensor>> Run(
      CalculatorContext* cc, const TensorSpan& tensor_span) override;

  const InputOutputTensorNames& GetInputOutputTensorNames() const override {
    return input_output_tensor_names_;
  }

  // Returns true if the model packet is empty.
  bool IsModelPacketEmpty() const { return model_packet_.IsEmpty(); }

  absl::Status Close();

 private:
  // Explicitly clear the model packet to save peak memory.
  void ClearModelPacket() { model_packet_ = api2::Packet<TfLiteModelPtr>(); }
#if defined(__EMSCRIPTEN__)
  using MpTensorReadView =
      std::variant<Tensor::CpuReadView, Tensor::WebGpuTexture2dView>;
  using MpTensorWriteView =
      std::variant<Tensor::CpuWriteView, Tensor::WebGpuTexture2dView>;
#elif MEDIAPIPE_OPENGL_ES_VERSION >= MEDIAPIPE_OPENGL_ES_31
  using MpTensorReadView =
      std::variant<Tensor::CpuReadView, Tensor::OpenGlBufferView>;
  using MpTensorWriteView =
      std::variant<Tensor::CpuWriteView, Tensor::OpenGlBufferView>;
#else
  using MpTensorReadView = Tensor::CpuReadView;
  using MpTensorWriteView = Tensor::CpuWriteView;
#endif

  // Context for holding tensors and views for a single inference run.
  struct InferenceRunContext {
    // Active views for the input and output tensors. Should be kept alive
    // during the inference run.
    std::vector<InferenceRunnerLiteRt::MpTensorReadView> active_input_views;
    std::vector<InferenceRunnerLiteRt::MpTensorWriteView> active_output_views;
    // Temporary tensors used for alignment or channel conversion.
    std::vector<std::unique_ptr<Tensor>> temporary_tensors;
    // LiteRT tensor buffers for the input and output tensors.
    std::vector<litert::TensorBuffer> litert_inputs;
    std::vector<litert::TensorBuffer> litert_outputs;
  };


  explicit InferenceRunnerLiteRt(
      MemoryManager* memory_manager,
#if defined(__EMSCRIPTEN__)
      WebGpuService* webgpu_service,
#endif  // __EMSCRIPTEN__
      std::shared_ptr<mediapipe::GlContext> gl_context,
      api2::Packet<TfLiteModelPtr> model_packet,
      std::unique_ptr<litert::Environment> environment,
      std::unique_ptr<litert::CompiledModel> compiled_model, bool run_async,
      bool enable_dynamic_resize, bool has_dynamic_dimension, bool use_npu,
      bool release_model_packet, int signature_index,
      litert::SimpleSignature signature,
      InputOutputTensorNames input_output_tensor_names,
      std::unique_ptr<InferenceFeedbackManagerLiteRt> feedback_manager
  );

  static InputOutputTensorNames CreateInputOutputTensorNames(
      const std::vector<litert::SimpleSignature>& signatures);


#if MEDIAPIPE_OPENGL_ES_VERSION >= MEDIAPIPE_OPENGL_ES_31
  // Creates an input tensor buffer from a GL buffer with zero-copy.
  absl::StatusOr<litert::TensorBuffer> CreateGlInputTensorBufferFromMpTensor(
      const Tensor& mp_input_tensor,
      const litert::RankedTensorType& tensor_type,
      std::vector<InferenceRunnerLiteRt::MpTensorReadView>&
          mp_input_tensor_views);

  // Creates an output tensor buffer from a GL buffer with zero-copy.
  absl::StatusOr<litert::TensorBuffer> CreateGlOutputTensorBufferFromMpTensor(
      const Tensor& mp_output_tensor,
      const litert::RankedTensorType& tensor_type,
      std::vector<InferenceRunnerLiteRt::MpTensorWriteView>&
          mp_output_tensor_views);
#endif  // MEDIAPIPE_OPENGL_ES_VERSION >= MEDIAPIPE_OPENGL_ES_31

#if defined(__EMSCRIPTEN__)
  // Creates an input tensor buffer from a WebGPU texture with zero-copy.
  absl::StatusOr<litert::TensorBuffer>
  CreateWebGpuInputTensorBufferFromMpTensor(
      const Tensor& mp_input_tensor,
      const litert::RankedTensorType& tensor_type,
      std::vector<InferenceRunnerLiteRt::MpTensorReadView>&
          mp_input_tensor_views);

  // Creates an output tensor buffer from a WebGPU texture with zero-copy.
  absl::StatusOr<litert::TensorBuffer>
  CreateWebGpuOutputTensorBufferFromMpTensor(
      const Tensor& mp_output_tensor,
      const litert::RankedTensorType& tensor_type,
      std::vector<InferenceRunnerLiteRt::MpTensorWriteView>&
          mp_output_tensor_views);
#endif  // defined(__EMSCRIPTEN__)

  // Creates a ranked tensor type for the given input tensor. If the tensor is
  // dynamic the litert tensor type is resized to match the MP input tensor
  // type.
  absl::StatusOr<litert::RankedTensorType> CreateInputRankedTensorType(
      const litert::RankedTensorType& tensor_type,
      const Tensor& mp_input_tensor, int tensor_index);

  // Creates a ranked tensor type for the given output tensor. If the tensor is
  // dynamic, a updated ranked tensor type is returned.
  absl::StatusOr<litert::RankedTensorType> CreateOutputRankedTensorType(
      const litert::RankedTensorType& model_tensor_type, litert::Layout layout);


  // Creates an input tensor buffer from host memory where the LiteRT buffer is
  // backed by MP tensor memory (zero-copy).
  absl::StatusOr<litert::TensorBuffer> CreateCpuInputTensorBufferFromMpTensor(
      const Tensor& mp_input_tensor,
      const litert::RankedTensorType& tensor_type,
      std::vector<InferenceRunnerLiteRt::MpTensorReadView>&
          mp_input_tensor_views,
      std::vector<std::unique_ptr<Tensor>>& mp_aligned_input_tensors);

  // Creates an output tensor buffer from host memory where the LiteRT buffer is
  // backed by MP tensor memory (zero-copy).
  absl::StatusOr<litert::TensorBuffer> CreateCpuOutputTensorBufferFromMpTensor(
      const Tensor& mp_output_tensor,
      const litert::RankedTensorType& tensor_type,
      std::vector<InferenceRunnerLiteRt::MpTensorWriteView>&
          mp_output_tensor_views);

  // Creates a vector of MP output tensors that matches the model signature of
  // the output tensors.
  absl::StatusOr<std::vector<Tensor>> CreateMpOutputTensors(
      const litert::SimpleSignature& signature,
      const std::vector<litert::Layout>& output_tensor_layouts);

  // Prepares the input buffers for a single inference run.
  absl::Status PrepareInputBuffers(const litert::SimpleSignature& signature,
                                   const TensorSpan& tensor_span,
                                   InferenceRunContext& ctx);

  // Prepares the output buffers for a single inference run.
  absl::Status PrepareOutputBuffers(
      const litert::SimpleSignature& signature,
      const std::vector<litert::Layout>& output_tensor_layouts,
      const std::vector<Tensor>& mp_output_tensors, InferenceRunContext& ctx);

  // Checks if a managed LiteRT buffer needs to be reallocated.
  absl::StatusOr<bool> NeedsReallocation(
      const std::optional<litert::TensorBuffer>& managed_buffer,
      size_t buffer_size, const litert::RankedTensorType& tensor_type) const;

  // Returns a duplicated feedback tensor buffer if the tensor at `tensor_index`
  // is a feedback tensor, or `std::nullopt` otherwise.
  absl::StatusOr<std::optional<litert::TensorBuffer>> TryGetFeedbackBuffer(
      bool is_input, int tensor_index, absl::string_view tensor_name) const;

  // Copies data from LiteRT managed output buffers to MP tensors. This is
  // required for buffer types that don't support zero-copy with MP tensors.
  absl::Status CopyLiteRtManagedOutputBuffersToMpTensors(
      std::vector<litert::TensorBuffer>& output_buffers,
      std::vector<Tensor>& mp_output_tensors);

  // Converts channels on CPU if needed.
  absl::StatusOr<const Tensor*> ConvertTensorChannelsIfNeeded(
      const Tensor& mp_input_tensor,
      const litert::RankedTensorType& tensor_type, InferenceRunContext& ctx);

  // Creates an input tensor buffer for the given buffer type.
  absl::StatusOr<litert::TensorBuffer> CreateInputTensorBufferFromMpTensor(
      litert::TensorBufferType buffer_type, const Tensor& tensor,
      const litert::RankedTensorType& tensor_type, InferenceRunContext& ctx);

  // Creates an output tensor buffer for the given buffer type.
  absl::StatusOr<litert::TensorBuffer> CreateOutputTensorBufferFromMpTensor(
      litert::TensorBufferType buffer_type, const Tensor& tensor,
      const litert::RankedTensorType& tensor_type, InferenceRunContext& ctx);

  // Returns the number of feedback tensors.
  int GetNumberOfFeedbackTensors() const {
    return feedback_manager_ ? feedback_manager_->GetNumberOfFeedbackTensors()
                             : 0;
  }

  MemoryManager* memory_manager_ = nullptr;

#if defined(__EMSCRIPTEN__)
  const WebGpuService* webgpu_service_ = nullptr;
#endif  // __EMSCRIPTEN__

  std::shared_ptr<mediapipe::GlContext> gl_context_;

  // Inference runner must own the model packet. LiteRt doesn't take ownership
  // of the model.
  api2::Packet<TfLiteModelPtr> model_packet_;
  std::unique_ptr<litert::Environment> environment_;
  std::unique_ptr<litert::CompiledModel> compiled_model_;
  bool run_async_ = false;

  int signature_index_ = -1;

  // Signature of the model at signature_index_. Cached to avoid rebuilding it
  // for every inference run.
  litert::SimpleSignature signature_;

  // Whether dynamic tensor resizing is enabled
  bool enable_dynamic_resize_ = true;

  // Whether the NPU delegate is used
  bool use_npu_ = false;

  // Whether any input or output tensor only gets its real shape at runtime
  // (dynamic -1 input dims with resize enabled, or rank-0 declared outputs).
  // Computed once from the model signature in Create(); when true, output
  // tensors are sized from the runtime layouts of the compiled model.
  const bool has_dynamic_dimension_;

  InputOutputTensorNames input_output_tensor_names_;


  // Feedback manager is only created if the model has feedback tensors.
  std::unique_ptr<InferenceFeedbackManagerLiteRt> feedback_manager_;

  // Managed input and output buffers that are not backed by MP tensor memory,
  // i.e. don't support zero-copy. They are cached in the inference runner to
  // avoid reallocating them for each run.
  std::vector<std::optional<litert::TensorBuffer>> managed_input_buffers_;
  std::vector<std::optional<litert::TensorBuffer>> managed_output_buffers_;

  // Precomputed buffer requirements for the default signature's input tensors.
  std::vector<litert::TensorBufferRequirements>
      cached_input_buffer_requirements_;

};

}  // namespace mediapipe

#endif  // MEDIAPIPE_CALCULATORS_TENSOR_INFERENCE_RUNNER_LITERT_H_
