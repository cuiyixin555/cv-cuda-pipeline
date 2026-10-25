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

#include <cstring>
#include <memory>
#include <string>
#include <vector>

#include "absl/log/absl_check.h"
#include "absl/log/absl_log.h"
#include "absl/memory/memory.h"
#include "cv-cuda-pipeline/calculators/tflite/tflite_inference_calculator.pb.h"
#include "cv-cuda-pipeline/framework/calculator_framework.h"
#include "cv-cuda-pipeline/framework/port/logging.h"
#include "cv-cuda-pipeline/framework/port/ret_check.h"
#include "cv-cuda-pipeline/util/tflite/config.h"

#if !defined(__EMSCRIPTEN__) || defined(__EMSCRIPTEN_PTHREADS__)
#include "cv-cuda-pipeline/util/cpu_util.h"
#endif  // !__EMSCRIPTEN__ || __EMSCRIPTEN_PTHREADS__

#include "cv-cuda-pipeline/util/tflite/tflite_model_loader.h"
#include "tflite/error_reporter.h"
#include "tflite/interpreter.h"
#include "tflite/kernels/register.h"
#include "tflite/model.h"


#if MEDIAPIPE_TFLITE_GL_INFERENCE
#include "cv-cuda-pipeline/gpu/gl_calculator_helper.h"
#include "cv-cuda-pipeline/gpu/gpu_buffer.h"
#include "cv-cuda-pipeline/util/tflite/tflite_gpu_runner.h"
#include "tflite/delegates/gpu/common/shape.h"
#include "tflite/delegates/gpu/gl/gl_buffer.h"
#include "tflite/delegates/gpu/gl/gl_program.h"
#include "tflite/delegates/gpu/gl/gl_shader.h"
#include "tflite/delegates/gpu/gl_delegate.h"
#endif  // MEDIAPIPE_TFLITE_GL_INFERENCE


#if !defined(MEDIAPIPE_EDGE_TPU)
#include "tflite/delegates/xnnpack/xnnpack_delegate.h"
#endif  // !EDGETPU

namespace {
// Commonly used to compute the number of blocks to launch in a kernel.
int NumGroups(const int size, const int group_size) {  // NOLINT
  return (size + group_size - 1) / group_size;
}

// Round up n to next multiple of m.
size_t RoundUp(size_t n, size_t m) { return ((n + m - 1) / m) * m; }  // NOLINT

constexpr char kTensorsTag[] = "TENSORS";
constexpr char kTensorsGpuTag[] = "TENSORS_GPU";
}  // namespace

#if defined(MEDIAPIPE_EDGE_TPU)
#include "tflite/public/edgetpu.h"

// Checkes whether model contains Edge TPU custom op or not.
bool ContainsEdgeTpuCustomOp(const tflite::FlatBufferModel& model) {
  const auto* opcodes = model.GetModel()->operator_codes();
  for (const auto* subgraph : *model.GetModel()->subgraphs()) {
    for (const auto* op : *subgraph->operators()) {
      const auto* opcode = opcodes->Get(op->opcode_index());
      if (opcode->custom_code() &&
          opcode->custom_code()->str() == edgetpu::kCustomOp) {
        return true;
      }
    }
  }
  return false;
}

// Creates and returns an Edge TPU interpreter to run the given edgetpu model.
std::unique_ptr<tflite::Interpreter> BuildEdgeTpuInterpreter(
    const tflite::FlatBufferModel& model,
    tflite::ops::builtin::BuiltinOpResolver* resolver,
    edgetpu::EdgeTpuContext* edgetpu_context) {
  resolver->AddCustom(edgetpu::kCustomOp, edgetpu::RegisterCustomOp());
  std::unique_ptr<tflite::Interpreter> interpreter;
  ABSL_CHECK_EQ(tflite::InterpreterBuilder(model, *resolver)(&interpreter),
                kTfLiteOk);
  interpreter->SetExternalContext(kTfLiteEdgeTpuContext, edgetpu_context);
  return interpreter;
}
#endif  // MEDIAPIPE_EDGE_TPU

// TfLiteInferenceCalculator File Layout:
//  * Header
//  * Core
//  * Aux
namespace mediapipe {

#if MEDIAPIPE_TFLITE_GL_INFERENCE
using ::tflite::gpu::gl::CopyBuffer;
using ::tflite::gpu::gl::CreateReadWriteShaderStorageBuffer;
using ::tflite::gpu::gl::GlBuffer;
#endif

#if MEDIAPIPE_TFLITE_GPU_SUPPORTED
namespace {
struct GPUData {
  int elements = 1;
  GpuTensor buffer;
  ::tflite::gpu::BHWC shape;
};
}  // namespace
#endif  // MEDIAPIPE_TFLITE_GPU_SUPPORTED

namespace {

int GetXnnpackDefaultNumThreads() {
#if defined(__EMSCRIPTEN_PTHREADS__)
  constexpr int kMinNumThreadsByDefault = 1;
  constexpr int kMaxNumThreadsByDefault = 4;
  return std::clamp(NumCPUCores() / 2, kMinNumThreadsByDefault,
                    kMaxNumThreadsByDefault);
#else
  return 1;
#endif
}

// Returns number of threads to configure XNNPACK delegate with.
// Returns user provided value if specified. Otherwise, tries to choose optimal
// number of threads depending on the device.
int GetXnnpackNumThreads(
    const mediapipe::TfLiteInferenceCalculatorOptions& opts) {
  static constexpr int kDefaultNumThreads = -1;
  if (opts.has_delegate() && opts.delegate().has_xnnpack() &&
      opts.delegate().xnnpack().num_threads() != kDefaultNumThreads) {
    return opts.delegate().xnnpack().num_threads();
  }
  return GetXnnpackDefaultNumThreads();
}

}  // namespace

// Calculator Header Section

// Runs inference on the provided input TFLite tensors and TFLite model.
//
// Creates an interpreter with given model and calls invoke().
// Optionally run inference on CPU/GPU.
//
// This calculator is designed to be used with the TfLiteConverterCalculator,
// to get the appropriate inputs.
//
// When the input tensors are on CPU, gpu inference is optional and can be
// specified in the calculator options.
// When the input tensors are on GPU, inference is GPU and output can be CPU or
// GPU.
//
// Input:
//  TENSORS - Vector of TfLiteTensor of type kTfLiteFloat32 or kTfLiteUInt8
//  TENSORS_GPU - Vector of GlBuffer
//
// Output:
//  TENSORS - Vector of TfLiteTensor of type kTfLiteFloat32 or kTfLiteUInt8
//  TENSORS_GPU - Vector of GlBuffer
//
// Input side packet:
//  CUSTOM_OP_RESOLVER (optional) - Use a custom op resolver,
//                                  instead of the builtin one.
//  MODEL (optional) - Use to specify TfLite model
//                     (std::unique_ptr<tflite::FlatBufferModel,
//                       std::function<void(tflite::FlatBufferModel*)>>)
//
// Example use:
// node {
//   calculator: "TfLiteInferenceCalculator"
//   input_stream: "TENSORS:tensor_image"
//   output_stream: "TENSORS:tensors"
//   options: {
//     [mediapipe.TfLiteInferenceCalculatorOptions.ext] {
//       model_path: "modelname.tflite"
//     }
//   }
// }
//
// or
//
// node {
//   calculator: "TfLiteInferenceCalculator"
//   input_stream: "TENSORS_GPU:tensor_image"
//   input_side_packet: "MODEL:model"
//   output_stream: "TENSORS_GPU:tensors"
//   options: {
//     [mediapipe.TfLiteInferenceCalculatorOptions.ext] {
//       model_path: "modelname.tflite"
//       delegate { gpu {} }
//     }
//   }
// }
//
// IMPORTANT Notes:
//  Tensors are assumed to be ordered correctly (sequentially added to model).
//  Input tensors are assumed to be of the correct size and already normalized.
//  All output TfLiteTensors will be destroyed when the graph closes,
//  (i.e. after calling graph.WaitUntilDone()).
//  GPU tensor support rquires OpenGL ES 3.1+.
//  This calculator uses FixedSizeInputStreamHandler by default.
//
class TfLiteInferenceCalculator : public CalculatorBase {
 public:
  using TfLiteDelegatePtr =
      std::unique_ptr<TfLiteDelegate, std::function<void(TfLiteDelegate*)>>;

  static absl::Status GetContract(CalculatorContract* cc);

  absl::Status Open(CalculatorContext* cc) override;
  absl::Status Process(CalculatorContext* cc) override;
  absl::Status Close(CalculatorContext* cc) override;

 private:
  absl::Status ReadKernelsFromFile();
  absl::Status WriteKernelsToFile();
  absl::Status LoadModel(CalculatorContext* cc);
  absl::StatusOr<Packet> GetModelAsPacket(const CalculatorContext& cc);
  absl::Status LoadDelegate(CalculatorContext* cc);
  absl::Status InitTFLiteGPURunner(CalculatorContext* cc);
  absl::Status ProcessInputsCpu(CalculatorContext* cc,
                                std::vector<TfLiteTensor>* output_tensors_cpu);
  absl::Status ProcessOutputsCpu(
      CalculatorContext* cc,
      std::unique_ptr<std::vector<TfLiteTensor>> output_tensors_cpu);
  absl::Status ProcessInputsGpu(CalculatorContext* cc,
                                std::vector<GpuTensor>* output_tensors_gpu);
  absl::Status ProcessOutputsGpu(
      CalculatorContext* cc,
      std::unique_ptr<std::vector<TfLiteTensor>> output_tensors_cpu,
      std::unique_ptr<std::vector<GpuTensor>> output_tensors_gpu);

  absl::Status RunInContextIfNeeded(std::function<absl::Status(void)> f) {
    if (gpu_inference_) {
#if MEDIAPIPE_TFLITE_GL_INFERENCE
      return gpu_helper_.RunInGlContext(std::move(f));
#endif  // MEDIAPIPE_TFLITE_GL_INFERENCE
    }
    return f();
  }

  Packet model_packet_;
  TfLiteDelegatePtr delegate_;
  std::unique_ptr<tflite::Interpreter> interpreter_;

#if MEDIAPIPE_TFLITE_GL_INFERENCE
  mediapipe::GlCalculatorHelper gpu_helper_;
  std::vector<std::unique_ptr<GPUData>> gpu_data_in_;
  std::vector<std::unique_ptr<GPUData>> gpu_data_out_;
  std::unique_ptr<tflite::gpu::TFLiteGPURunner> tflite_gpu_runner_;
#endif  // MEDIAPIPE_TFLITE_GL_INFERENCE

#if defined(MEDIAPIPE_EDGE_TPU)
  std::shared_ptr<edgetpu::EdgeTpuContext> edgetpu_context_;
#endif

  bool gpu_inference_ = false;
  bool gpu_input_ = false;
  bool gpu_output_ = false;
  bool use_quantized_tensors_ = false;

  bool use_advanced_gpu_api_ = false;
  bool allow_precision_loss_ = false;
  mediapipe::TfLiteInferenceCalculatorOptions::Delegate::Gpu::Api
      tflite_gpu_runner_api_;
  mediapipe::TfLiteInferenceCalculatorOptions::Delegate::Gpu::InferenceUsage
      tflite_gpu_runner_usage_;

};
REGISTER_CALCULATOR(TfLiteInferenceCalculator);

// Calculator Core Section

namespace {

constexpr char kCustomOpResolverTag[] = "CUSTOM_OP_RESOLVER";
constexpr char kModelTag[] = "MODEL";

template <class CC>
bool ShouldUseGpu(CC* cc) {
#if MEDIAPIPE_TFLITE_GPU_SUPPORTED
  const auto& options =
      cc->template Options<::mediapipe::TfLiteInferenceCalculatorOptions>();
  return options.use_gpu() ||
         (options.has_delegate() && options.delegate().has_gpu()) ||
         cc->Inputs().HasTag(kTensorsGpuTag) ||
         cc->Outputs().HasTag(kTensorsGpuTag);
#else
  return false;
#endif  // MEDIAPIPE_TFLITE_GPU_SUPPORTED
}
}  // namespace

absl::Status TfLiteInferenceCalculator::GetContract(CalculatorContract* cc) {
  RET_CHECK(cc->Inputs().HasTag(kTensorsTag) ^
            cc->Inputs().HasTag(kTensorsGpuTag));
  RET_CHECK(cc->Outputs().HasTag(kTensorsTag) ^
            cc->Outputs().HasTag(kTensorsGpuTag));

  const auto& options =
      cc->Options<::mediapipe::TfLiteInferenceCalculatorOptions>();
  RET_CHECK(!options.model_path().empty() ^
            cc->InputSidePackets().HasTag(kModelTag))
      << "Either model as side packet or model path in options is required.";

  if (cc->Inputs().HasTag(kTensorsTag))
    cc->Inputs().Tag(kTensorsTag).Set<std::vector<TfLiteTensor>>();
  if (cc->Outputs().HasTag(kTensorsTag))
    cc->Outputs().Tag(kTensorsTag).Set<std::vector<TfLiteTensor>>();

  if (cc->Inputs().HasTag(kTensorsGpuTag))
    cc->Inputs().Tag(kTensorsGpuTag).Set<std::vector<GpuTensor>>();
  if (cc->Outputs().HasTag(kTensorsGpuTag))
    cc->Outputs().Tag(kTensorsGpuTag).Set<std::vector<GpuTensor>>();

  if (cc->InputSidePackets().HasTag(kCustomOpResolverTag)) {
    cc->InputSidePackets()
        .Tag(kCustomOpResolverTag)
        .Set<tflite::ops::builtin::BuiltinOpResolver>();
  }
  if (cc->InputSidePackets().HasTag(kModelTag)) {
    cc->InputSidePackets().Tag(kModelTag).Set<TfLiteModelPtr>();
  }

  if (ShouldUseGpu(cc)) {
#if MEDIAPIPE_TFLITE_GL_INFERENCE
    ABSL_RETURN_IF_ERROR(mediapipe::GlCalculatorHelper::UpdateContract(cc));
#endif
  }

  // Assign this calculator's default InputStreamHandler.
  cc->SetInputStreamHandler("FixedSizeInputStreamHandler");

  return absl::OkStatus();
}

absl::Status TfLiteInferenceCalculator::Open(CalculatorContext* cc) {
  cc->SetOffset(TimestampDiff(0));

  const auto& options =
      cc->Options<::mediapipe::TfLiteInferenceCalculatorOptions>();

  gpu_inference_ = ShouldUseGpu(cc);
  gpu_input_ = cc->Inputs().HasTag(kTensorsGpuTag);
  gpu_output_ = cc->Outputs().HasTag(kTensorsGpuTag);

  use_advanced_gpu_api_ = MEDIAPIPE_TFLITE_GL_INFERENCE &&
                          options.has_delegate() &&
                          options.delegate().has_gpu() &&
                          options.delegate().gpu().use_advanced_gpu_api();
  allow_precision_loss_ = options.delegate().gpu().allow_precision_loss();
  tflite_gpu_runner_api_ = options.delegate().gpu().api();
  tflite_gpu_runner_usage_ = options.delegate().gpu().usage();

  if (use_advanced_gpu_api_ && !gpu_input_) {
    ABSL_LOG(WARNING)
        << "Cannot use advanced GPU APIs, input must be GPU buffers."
           "Falling back to the default TFLite API.";
    use_advanced_gpu_api_ = false;
  }
  ABSL_CHECK(!use_advanced_gpu_api_ || gpu_inference_);

  ABSL_RETURN_IF_ERROR(LoadModel(cc));

  if (gpu_inference_) {
#if MEDIAPIPE_TFLITE_GL_INFERENCE
    ABSL_RETURN_IF_ERROR(gpu_helper_.Open(cc));
    ABSL_RETURN_IF_ERROR(gpu_helper_.RunInGlContext([this,
                                                     &cc]() -> absl::Status {
      return use_advanced_gpu_api_ ? InitTFLiteGPURunner(cc) : LoadDelegate(cc);
    }));
#endif
  } else {
    ABSL_RETURN_IF_ERROR(LoadDelegate(cc));
  }
  return absl::OkStatus();
}

absl::Status TfLiteInferenceCalculator::Process(CalculatorContext* cc) {
  return RunInContextIfNeeded([this, cc]() -> absl::Status {
    // 0. Declare outputs
    auto output_tensors_gpu = std::make_unique<std::vector<GpuTensor>>();
    auto output_tensors_cpu = std::make_unique<std::vector<TfLiteTensor>>();

    // 1. Receive pre-processed tensor inputs.
    if (gpu_input_) {
      ABSL_RETURN_IF_ERROR(ProcessInputsGpu(cc, output_tensors_gpu.get()));
    } else {
      ABSL_RETURN_IF_ERROR(ProcessInputsCpu(cc, output_tensors_cpu.get()));
    }

    // 2. Run inference.
#if MEDIAPIPE_TFLITE_GL_INFERENCE
    if (gpu_inference_ && use_advanced_gpu_api_) {
      RET_CHECK(tflite_gpu_runner_->Invoke().ok());
    } else {
      RET_CHECK_EQ(interpreter_->Invoke(), kTfLiteOk);
    }
#else   // MEDIAPIPE_TFLITE_GL_INFERENCE
    RET_CHECK_EQ(interpreter_->Invoke(), kTfLiteOk);
#endif  // MEDIAPIPE_TFLITE_GL_INFERENCE

    // 3. Output processed tensors.
    if (gpu_output_ || use_advanced_gpu_api_) {
      ABSL_RETURN_IF_ERROR(ProcessOutputsGpu(cc, std::move(output_tensors_cpu),
                                             std::move(output_tensors_gpu)));
    } else {
      ABSL_RETURN_IF_ERROR(
          ProcessOutputsCpu(cc, std::move(output_tensors_cpu)));
    }

    return absl::OkStatus();
  });
}

absl::Status TfLiteInferenceCalculator::WriteKernelsToFile() {
  return absl::OkStatus();
}

absl::Status TfLiteInferenceCalculator::Close(CalculatorContext* cc) {
  ABSL_RETURN_IF_ERROR(WriteKernelsToFile());

  return RunInContextIfNeeded([this]() -> absl::Status {
    interpreter_ = nullptr;
    if (delegate_) {
      delegate_ = nullptr;
#if MEDIAPIPE_TFLITE_GPU_SUPPORTED
      if (gpu_inference_) {
        for (int i = 0; i < gpu_data_in_.size(); ++i) {
          gpu_data_in_[i].reset();
        }
        for (int i = 0; i < gpu_data_out_.size(); ++i) {
          gpu_data_out_[i].reset();
        }
      }
#endif  // MEDIAPIPE_TFLITE_GPU_SUPPORTED
    }
#if defined(MEDIAPIPE_EDGE_TPU)
    edgetpu_context_ = nullptr;
#endif
    return absl::OkStatus();
  });
}

// Calculator Auxiliary Section

absl::Status TfLiteInferenceCalculator::ProcessInputsCpu(
    CalculatorContext* cc, std::vector<TfLiteTensor>* output_tensors_cpu) {
  if (cc->Inputs().Tag(kTensorsTag).IsEmpty()) {
    return absl::OkStatus();
  }
  // Read CPU input into tensors.
  const auto& input_tensors =
      cc->Inputs().Tag(kTensorsTag).Get<std::vector<TfLiteTensor>>();
  RET_CHECK_GT(input_tensors.size(), 0);
  for (int i = 0; i < input_tensors.size(); ++i) {
    const TfLiteTensor* input_tensor = &input_tensors[i];
    RET_CHECK(input_tensor->data.raw);
    if (use_quantized_tensors_) {
      const uint8_t* input_tensor_buffer = input_tensor->data.uint8;
      uint8_t* local_tensor_buffer =
          interpreter_->typed_input_tensor<uint8_t>(i);
      std::memcpy(local_tensor_buffer, input_tensor_buffer,
                  input_tensor->bytes);
    } else {
      const float* input_tensor_buffer = input_tensor->data.f;
      float* local_tensor_buffer = interpreter_->typed_input_tensor<float>(i);
      std::memcpy(local_tensor_buffer, input_tensor_buffer,
                  input_tensor->bytes);
    }
  }

  return absl::OkStatus();
}

absl::Status TfLiteInferenceCalculator::ProcessInputsGpu(
    CalculatorContext* cc, std::vector<GpuTensor>* output_tensors_gpu) {
  if (cc->Inputs().Tag(kTensorsGpuTag).IsEmpty()) {
    return absl::OkStatus();
  }
  if (use_advanced_gpu_api_) {
#if MEDIAPIPE_TFLITE_GL_INFERENCE
    const auto& input_tensors =
        cc->Inputs().Tag(kTensorsGpuTag).Get<std::vector<GpuTensor>>();
    RET_CHECK(!input_tensors.empty());
    for (int i = 0; i < input_tensors.size(); ++i) {
      ABSL_RETURN_IF_ERROR(
          tflite_gpu_runner_->BindSSBOToInputTensor(input_tensors[i].id(), i));
    }
    if (gpu_output_) {
      // Allocate new output tensor.
      output_tensors_gpu->resize(gpu_data_out_.size());
      for (int i = 0; i < gpu_data_out_.size(); ++i) {
        GpuTensor& tensor = output_tensors_gpu->at(i);
        ABSL_RETURN_IF_ERROR(CreateReadWriteShaderStorageBuffer<float>(
            gpu_data_out_[i]->elements, &tensor));
        ABSL_RETURN_IF_ERROR(
            tflite_gpu_runner_->BindSSBOToOutputTensor(tensor.id(), i));
      }
    } else {
      // Re-use internal output tensor.
      for (int i = 0; i < gpu_data_out_.size(); ++i) {
        ABSL_RETURN_IF_ERROR(tflite_gpu_runner_->BindSSBOToOutputTensor(
            gpu_data_out_[i]->buffer.id(), i));
      }
    }
#endif  // MEDIAPIPE_TFLITE_GL_INFERENCE
  } else if (gpu_input_) {
    // Read GPU input into SSBO.
#if MEDIAPIPE_TFLITE_GL_INFERENCE
    const auto& input_tensors =
        cc->Inputs().Tag(kTensorsGpuTag).Get<std::vector<GpuTensor>>();
    RET_CHECK_GT(input_tensors.size(), 0);
    // Explicit copy input.
    gpu_data_in_.resize(input_tensors.size());
    for (int i = 0; i < input_tensors.size(); ++i) {
      ABSL_RETURN_IF_ERROR(
          CopyBuffer(input_tensors[i], gpu_data_in_[i]->buffer));
    }
#endif  // MEDIAPIPE_TFLITE_GL_INFERENCE
  }

  return absl::OkStatus();
}

absl::Status TfLiteInferenceCalculator::ProcessOutputsCpu(
    CalculatorContext* cc,
    std::unique_ptr<std::vector<TfLiteTensor>> output_tensors_cpu) {
  // Output result tensors (CPU).
  const auto& tensor_indexes = interpreter_->outputs();
  for (int i = 0; i < tensor_indexes.size(); ++i) {
    TfLiteTensor* tensor = interpreter_->tensor(tensor_indexes[i]);
    output_tensors_cpu->emplace_back(*tensor);
  }
  cc->Outputs()
      .Tag(kTensorsTag)
      .Add(output_tensors_cpu.release(), cc->InputTimestamp());

  return absl::OkStatus();
}

absl::Status TfLiteInferenceCalculator::ProcessOutputsGpu(
    CalculatorContext* cc,
    std::unique_ptr<std::vector<TfLiteTensor>> output_tensors_cpu,
    std::unique_ptr<std::vector<GpuTensor>> output_tensors_gpu) {
  if (use_advanced_gpu_api_) {
#if MEDIAPIPE_TFLITE_GL_INFERENCE
    if (gpu_output_) {
      // Send out pre-allocated tensors.
      cc->Outputs()
          .Tag(kTensorsGpuTag)
          .Add(output_tensors_gpu.release(), cc->InputTimestamp());
    } else {
      // Download to CPU for output.
      const auto& tensor_indexes = interpreter_->inputs();
      for (int i = 0; i < tensor_indexes.size(); ++i) {
        TfLiteTensor* tensor = interpreter_->tensor(tensor_indexes[i]);
        std::vector<float> gpu_data(tensor->bytes / sizeof(float));
        ABSL_RETURN_IF_ERROR(gpu_data_out_[i]->buffer.Read(
            absl::MakeSpan(tensor->data.f, tensor->bytes)));
        output_tensors_cpu->emplace_back(*tensor);
      }
      // Output result tensors (CPU).
      cc->Outputs()
          .Tag(kTensorsTag)
          .Add(output_tensors_cpu.release(), cc->InputTimestamp());
    }
#endif  // MEDIAPIPE_TFLITE_GL_INFERENCE
  } else if (gpu_output_) {
#if MEDIAPIPE_TFLITE_GL_INFERENCE
    // Output result tensors (GPU).
    output_tensors_gpu->resize(gpu_data_out_.size());
    for (int i = 0; i < gpu_data_out_.size(); ++i) {
      GpuTensor& tensor = output_tensors_gpu->at(i);
      // Allocate output tensor.
      ABSL_RETURN_IF_ERROR(CreateReadWriteShaderStorageBuffer<float>(
          gpu_data_out_[i]->elements, &tensor));
      ABSL_RETURN_IF_ERROR(CopyBuffer(gpu_data_out_[i]->buffer, tensor));
    }
    cc->Outputs()
        .Tag(kTensorsGpuTag)
        .Add(output_tensors_gpu.release(), cc->InputTimestamp());
#endif  // MEDIAPIPE_TFLITE_GL_INFERENCE
  }

  return absl::OkStatus();
}

absl::Status TfLiteInferenceCalculator::ReadKernelsFromFile() {
  return absl::OkStatus();
}

absl::Status TfLiteInferenceCalculator::InitTFLiteGPURunner(
    CalculatorContext* cc) {
#if MEDIAPIPE_TFLITE_GL_INFERENCE
  ABSL_ASSIGN_OR_RETURN(model_packet_, GetModelAsPacket(*cc));
  const auto& model = *model_packet_.Get<TfLiteModelPtr>();

  tflite::ops::builtin::BuiltinOpResolverWithoutDefaultDelegates
      default_op_resolver;
  auto op_resolver_ptr =
      static_cast<const tflite::ops::builtin::BuiltinOpResolver*>(
          &default_op_resolver);
  if (cc->InputSidePackets().HasTag(kCustomOpResolverTag)) {
    op_resolver_ptr = &(cc->InputSidePackets()
                            .Tag(kCustomOpResolverTag)
                            .Get<tflite::ops::builtin::BuiltinOpResolver>());
  }

  // Create runner
  tflite::gpu::InferenceOptions options;
  options.priority1 = allow_precision_loss_
                          ? tflite::gpu::InferencePriority::MIN_LATENCY
                          : tflite::gpu::InferencePriority::MAX_PRECISION;
  options.priority2 = tflite::gpu::InferencePriority::AUTO;
  options.priority3 = tflite::gpu::InferencePriority::AUTO;
  switch (tflite_gpu_runner_usage_) {
    case mediapipe::TfLiteInferenceCalculatorOptions::Delegate::Gpu::
        FAST_SINGLE_ANSWER: {
      options.usage = tflite::gpu::InferenceUsage::FAST_SINGLE_ANSWER;
      break;
    }
    case mediapipe::TfLiteInferenceCalculatorOptions::Delegate::Gpu::
        SUSTAINED_SPEED: {
      options.usage = tflite::gpu::InferenceUsage::SUSTAINED_SPEED;
      break;
    }
    case mediapipe::TfLiteInferenceCalculatorOptions::Delegate::Gpu::
        UNSPECIFIED: {
      return absl::InternalError("inference usage need to be specified.");
    }
  }

  tflite_gpu_runner_ = std::make_unique<tflite::gpu::TFLiteGPURunner>(options);
  switch (tflite_gpu_runner_api_) {
    case mediapipe::TfLiteInferenceCalculatorOptions::Delegate::Gpu::OPENGL: {
      tflite_gpu_runner_->ForceOpenGL();
      break;
    }
    case mediapipe::TfLiteInferenceCalculatorOptions::Delegate::Gpu::OPENCL: {
      tflite_gpu_runner_->ForceOpenCL();
      break;
    }
    case mediapipe::TfLiteInferenceCalculatorOptions::Delegate::Gpu::ANY: {
      // Do not need to force any specific API.
      break;
    }
  }
  ABSL_RETURN_IF_ERROR(tflite_gpu_runner_->InitializeWithModel(
      model, *op_resolver_ptr, /*allow_quant_ops=*/true));

  // Allocate interpreter memory for cpu output.
  if (!gpu_output_) {
    interpreter_ = absl::make_unique<tflite::Interpreter>();
    const int num_outputs = tflite_gpu_runner_->GetOutputShapes().size();
    interpreter_->AddTensors(num_outputs);
    std::vector<int> indices(num_outputs);
    for (int i = 0; i < num_outputs; ++i) indices[i] = i;
    // There is no ResizeOutputTensor(), so we use 'inputs' space instead.
    interpreter_->SetInputs(indices);
    TfLiteQuantization quant;
    quant.type = kTfLiteNoQuantization;
    quant.params = nullptr;
    for (int i = 0; i < num_outputs; ++i) {
      auto shape = tflite_gpu_runner_->GetTFLiteOutputShapes()[i];
      const int tensor_idx = interpreter_->inputs()[i];
      interpreter_->SetTensorParametersReadWrite(tensor_idx, kTfLiteFloat32, "",
                                                 shape, quant);
      ABSL_CHECK(interpreter_->ResizeInputTensor(tensor_idx, shape) ==
                 kTfLiteOk);
    }
    ABSL_CHECK(interpreter_->AllocateTensors() == kTfLiteOk);
  }

  // Create and bind OpenGL buffers for outputs.
  // The buffers are created once and their ids are passed to calculator outputs
  gpu_data_out_.resize(tflite_gpu_runner_->outputs_size());
  for (int i = 0; i < tflite_gpu_runner_->outputs_size(); ++i) {
    gpu_data_out_[i] = absl::make_unique<GPUData>();
    ABSL_ASSIGN_OR_RETURN(gpu_data_out_[i]->elements,
                          tflite_gpu_runner_->GetOutputElements(i));
    // Create and bind input buffer.
    ABSL_RETURN_IF_ERROR(
        ::tflite::gpu::gl::CreateReadWriteShaderStorageBuffer<float>(
            gpu_data_out_[i]->elements, &gpu_data_out_[i]->buffer));
  }

  ABSL_RETURN_IF_ERROR(ReadKernelsFromFile());

  ABSL_RETURN_IF_ERROR(tflite_gpu_runner_->Build());
#endif  // MEDIAPIPE_TFLITE_GL_INFERENCE

  return absl::OkStatus();
}

absl::Status TfLiteInferenceCalculator::LoadModel(CalculatorContext* cc) {
  if (use_advanced_gpu_api_) {
    // Use InitTFLiteGPURunner for everything.
    return absl::OkStatus();
  }

  ABSL_ASSIGN_OR_RETURN(model_packet_, GetModelAsPacket(*cc));
  const auto& model = *model_packet_.Get<TfLiteModelPtr>();

  tflite::ops::builtin::BuiltinOpResolverWithoutDefaultDelegates
      default_op_resolver;
#if defined(MEDIAPIPE_EDGE_TPU)
  if (ContainsEdgeTpuCustomOp(model)) {
    edgetpu_context_ = edgetpu::EdgeTpuManager::GetSingleton()->OpenDevice();
    interpreter_ = BuildEdgeTpuInterpreter(model, &default_op_resolver,
                                           edgetpu_context_.get());
  } else {
#endif  // MEDIAPIPE_EDGE_TPU
    auto op_resolver_ptr =
        static_cast<const tflite::ops::builtin::BuiltinOpResolver*>(
            &default_op_resolver);

    if (cc->InputSidePackets().HasTag(kCustomOpResolverTag)) {
      op_resolver_ptr = &(cc->InputSidePackets()
                              .Tag(kCustomOpResolverTag)
                              .Get<tflite::ops::builtin::BuiltinOpResolver>());
    }

    tflite::InterpreterBuilder(model, *op_resolver_ptr)(&interpreter_);
#if defined(MEDIAPIPE_EDGE_TPU)
  }
#endif  // MEDIAPIPE_EDGE_TPU

  RET_CHECK(interpreter_);

#if defined(__EMSCRIPTEN__) || defined(MEDIAPIPE_EDGE_TPU)
  interpreter_->SetNumThreads(1);
#else
  interpreter_->SetNumThreads(
      cc->Options<mediapipe::TfLiteInferenceCalculatorOptions>()
          .cpu_num_thread());
#endif  // __EMSCRIPTEN__

  if (gpu_output_) {
    use_quantized_tensors_ = false;
  } else {
    RET_CHECK_EQ(interpreter_->AllocateTensors(), kTfLiteOk);
    use_quantized_tensors_ =
        (interpreter_->tensor(interpreter_->inputs()[0])->quantization.type ==
         kTfLiteAffineQuantization);
    if (use_quantized_tensors_) gpu_inference_ = false;
  }

  return absl::OkStatus();
}

absl::StatusOr<Packet> TfLiteInferenceCalculator::GetModelAsPacket(
    const CalculatorContext& cc) {
  const auto& options =
      cc.Options<mediapipe::TfLiteInferenceCalculatorOptions>();
  if (!options.model_path().empty()) {
    return TfLiteModelLoader::LoadFromPath(
        cc.GetResources(), options.model_path(), options.try_mmap_model());
  }
  if (cc.InputSidePackets().HasTag(kModelTag)) {
    return cc.InputSidePackets().Tag(kModelTag);
  }
  return absl::Status(absl::StatusCode::kNotFound,
                      "Must specify TFLite model as path or loaded model.");
}

absl::Status TfLiteInferenceCalculator::LoadDelegate(CalculatorContext* cc) {
  const auto& calculator_opts =
      cc->Options<mediapipe::TfLiteInferenceCalculatorOptions>();
  if (calculator_opts.has_delegate() &&
      calculator_opts.delegate().has_tflite()) {
    // Default tflite inference requeqsted - no need to modify graph.
    return absl::OkStatus();
  }

  if (!gpu_inference_) {

#if defined(__EMSCRIPTEN__)
    const bool use_xnnpack = true;
#else
    const bool use_xnnpack = calculator_opts.has_delegate() &&
                             calculator_opts.delegate().has_xnnpack();
#endif  // defined(__EMSCRIPTEN__)

#if !defined(MEDIAPIPE_EDGE_TPU)
    if (use_xnnpack) {
      auto xnnpack_opts = TfLiteXNNPackDelegateOptionsDefault();
      xnnpack_opts.num_threads = GetXnnpackNumThreads(calculator_opts);
      delegate_ = TfLiteDelegatePtr(TfLiteXNNPackDelegateCreate(&xnnpack_opts),
                                    &TfLiteXNNPackDelegateDelete);
      RET_CHECK_EQ(interpreter_->ModifyGraphWithDelegate(delegate_.get()),
                   kTfLiteOk);
      return absl::OkStatus();
    }
#else
    (void)use_xnnpack;
#endif  // !EDGETPU

    // Return and use default tflite infernece (on CPU). No need for GPU
    // delegate below.
    return absl::OkStatus();
  }

#if MEDIAPIPE_TFLITE_GL_INFERENCE
  // Configure and create the delegate.
  TfLiteGpuDelegateOptions options = TfLiteGpuDelegateOptionsDefault();
  options.compile_options.precision_loss_allowed =
      allow_precision_loss_ ? 1 : 0;
  options.compile_options.preferred_gl_object_type =
      TFLITE_GL_OBJECT_TYPE_FASTEST;
  options.compile_options.dynamic_batch_enabled = 0;
  options.compile_options.inline_parameters = 1;
  if (!delegate_)
    delegate_ = TfLiteDelegatePtr(TfLiteGpuDelegateCreate(&options),
                                  &TfLiteGpuDelegateDelete);

  if (gpu_input_) {
    // Get input image sizes.
    const auto& input_indices = interpreter_->inputs();
    gpu_data_in_.resize(input_indices.size());
    for (int i = 0; i < input_indices.size(); ++i) {
      const TfLiteTensor* tensor = interpreter_->tensor(input_indices[i]);
      gpu_data_in_[i] = absl::make_unique<GPUData>();
      gpu_data_in_[i]->elements = 1;
      for (int d = 0; d < tensor->dims->size; ++d) {
        gpu_data_in_[i]->elements *= tensor->dims->data[d];
      }
      // Create and bind input buffer.
      ABSL_RETURN_IF_ERROR(
          ::tflite::gpu::gl::CreateReadWriteShaderStorageBuffer<float>(
              gpu_data_in_[i]->elements, &gpu_data_in_[i]->buffer));
      RET_CHECK_EQ(TfLiteGpuDelegateBindBufferToTensor(
                       delegate_.get(), gpu_data_in_[i]->buffer.id(),
                       interpreter_->inputs()[i]),
                   kTfLiteOk);
    }
  }
  if (gpu_output_) {
    // Get output image sizes.
    const auto& output_indices = interpreter_->outputs();
    gpu_data_out_.resize(output_indices.size());
    for (int i = 0; i < gpu_data_out_.size(); ++i) {
      const TfLiteTensor* tensor = interpreter_->tensor(output_indices[i]);
      gpu_data_out_[i] = absl::make_unique<GPUData>();
      gpu_data_out_[i]->elements = 1;
      // TODO handle *2 properly on some dialated models
      for (int d = 0; d < tensor->dims->size; ++d) {
        gpu_data_out_[i]->elements *= tensor->dims->data[d];
      }
    }
    // Create and bind output buffers.
    interpreter_->SetAllowBufferHandleOutput(true);
    for (int i = 0; i < gpu_data_out_.size(); ++i) {
      ABSL_RETURN_IF_ERROR(CreateReadWriteShaderStorageBuffer<float>(
          gpu_data_out_[i]->elements, &gpu_data_out_[i]->buffer));
      RET_CHECK_EQ(TfLiteGpuDelegateBindBufferToTensor(
                       delegate_.get(), gpu_data_out_[i]->buffer.id(),
                       output_indices[i]),
                   kTfLiteOk);
    }
  }

  // Must call this last.
  RET_CHECK_EQ(interpreter_->ModifyGraphWithDelegate(delegate_.get()),
               kTfLiteOk);
#endif  // MEDIAPIPE_TFLITE_GL_INFERENCE


  return absl::OkStatus();
}

}  // namespace mediapipe
