#include "cv-cuda-pipeline/calculators/tensor/inference_runner_litert.h"

#include <optional>

#include "litert/cc/litert_api_types.h"  // from @litert


#include <algorithm>
#include <cstdint>
#include <cstring>  // IWYU pragma: keep
#include <functional>
#include <iterator>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "absl/algorithm/container.h"
#include "absl/base/nullability.h"
#include "absl/log/absl_log.h"
#include "absl/log/log.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_format.h"  // IWYU pragma: keep
#include "absl/strings/str_join.h"
#include "absl/strings/string_view.h"
#include "absl/types/span.h"
#include "litert/cc/litert_buffer_ref.h"                     // from @litert
#include "litert/cc/litert_common.h"                         // from @litert
#include "litert/cc/litert_compiled_model.h"                 // from @litert
#include "litert/cc/litert_environment.h"                    // from @litert
#include "litert/cc/litert_environment_options.h"            // from @litert
#include "litert/cc/litert_layout.h"                         // from @litert
#include "litert/cc/litert_macros.h"                         // from @litert
#include "litert/cc/litert_model_types.h"                    // from @litert
#include "litert/cc/litert_options.h"                        // from @litert
#include "litert/cc/litert_ranked_tensor_type.h"             // from @litert
#include "litert/cc/litert_tensor_buffer.h"                  // from @litert
#include "litert/cc/litert_tensor_buffer_requirements.h"     // from @litert
#include "litert/cc/litert_tensor_buffer_types.h"            // from @litert
#include "litert/cc/options/litert_cpu_options.h"            // from @litert
#include "litert/cc/options/litert_google_tensor_options.h"  // from @litert
#include "litert/cc/options/litert_gpu_options.h"            // from @litert
#include "litert/cc/options/litert_qualcomm_options.h"       // from @litert
#include "cv-cuda-pipeline/calculators/tensor/inference_calculator.pb.h"
#include "cv-cuda-pipeline/calculators/tensor/inference_calculator_utils.h"
#include "cv-cuda-pipeline/calculators/tensor/inference_feedback_manager_litert.h"
#include "cv-cuda-pipeline/calculators/tensor/inference_io_mapper.h"
#include "cv-cuda-pipeline/calculators/tensor/litert/litert_service.h"
#include "cv-cuda-pipeline/calculators/tensor/litert/litert_utils.h"
#include "cv-cuda-pipeline/calculators/tensor/tensor_span.h"
#include "cv-cuda-pipeline/framework/api2/packet.h"
#include "cv-cuda-pipeline/framework/calculator_context.h"
#include "cv-cuda-pipeline/framework/calculator_framework.h"
#include "cv-cuda-pipeline/framework/formats/tensor.h"
#include "cv-cuda-pipeline/framework/memory_manager.h"
#include "cv-cuda-pipeline/framework/port.h"  // IWYU pragma: keep
#include "cv-cuda-pipeline/framework/port/ret_check.h"
#include "cv-cuda-pipeline/framework/port/status_macros.h"
#include "cv-cuda-pipeline/util/tflite/tflite_model_loader.h"
#include "cv-cuda-pipeline/util/tflite/tflite_signature_reader.h"

#if MEDIAPIPE_OPENGL_ES_VERSION >= MEDIAPIPE_OPENGL_ES_31
#include "cv-cuda-pipeline/gpu/gl_context.h"
#endif


#if defined(__EMSCRIPTEN__)
#include "cv-cuda-pipeline/gpu/webgpu/webgpu_service.h"
#endif  // __EMSCRIPTEN__

namespace mediapipe {

namespace {

// Preferred buffer types for input and output tensors.
// Note: Order indicates the priority of the buffer type, i.e. the order in
// which LiteRT's buffer requirements will be checked.
constexpr litert::TensorBufferType kPreferredBufferTypes[] = {
    litert::TensorBufferType::kWebGpuTexture,
    litert::TensorBufferType::kHostMemory,
    litert::TensorBufferType::kFastRpc,
    litert::TensorBufferType::kDmaBuf,
    litert::TensorBufferType::kGlBuffer,
    litert::TensorBufferType::kOpenClTextureFp16,
    litert::TensorBufferType::kOpenClBuffer,
    litert::TensorBufferType::kOpenClBufferPacked,
    litert::TensorBufferType::kWebGpuBuffer,
    litert::TensorBufferType::kWebGpuBufferPacked,
};

// Buffer types that require copying to/from the MP tensor and are managed by
// LiteRT. Should be a subset of kPreferredBufferTypes.
constexpr litert::TensorBufferType kLiteRtManagedBufferTypes[] = {
    litert::TensorBufferType::kFastRpc,
    litert::TensorBufferType::kDmaBuf,
    litert::TensorBufferType::kOpenClBuffer,
    litert::TensorBufferType::kOpenClTextureFp16,
    litert::TensorBufferType::kOpenClBufferPacked,
    litert::TensorBufferType::kWebGpuBuffer,
    litert::TensorBufferType::kWebGpuBufferPacked,
};

absl::StatusOr<litert::qualcomm::QualcommOptions::HtpPerformanceMode>
ToLiteRtHtpPerformanceMode(InferenceCalculatorOptions::Delegate::LiteRt::Npu::
                               Qualcomm::HtpPerformanceMode mp_mode) {
  using HtpPerformanceMode =
      litert::qualcomm::QualcommOptions::HtpPerformanceMode;
  using QualcommDelegateOptions =
      InferenceCalculatorOptions::Delegate::LiteRt::Npu::Qualcomm;

  switch (mp_mode) {
    case QualcommDelegateOptions::DEFAULT:
      return HtpPerformanceMode::kDefault;
    case QualcommDelegateOptions::SUSTAINED_HIGH_PERFORMANCE:
      return HtpPerformanceMode::kSustainedHighPerformance;
    case QualcommDelegateOptions::BURST:
      return HtpPerformanceMode::kBurst;
    case QualcommDelegateOptions::HIGH_PERFORMANCE:
      return HtpPerformanceMode::kHighPerformance;
    case QualcommDelegateOptions::POWER_SAVER:
      return HtpPerformanceMode::kPowerSaver;
    case QualcommDelegateOptions::LOW_POWER_SAVER:
      return HtpPerformanceMode::kLowPowerSaver;
    case QualcommDelegateOptions::HIGH_POWER_SAVER:
      return HtpPerformanceMode::kHighPowerSaver;
    case QualcommDelegateOptions::LOW_BALANCED:
      return HtpPerformanceMode::kLowBalanced;
    case QualcommDelegateOptions::BALANCED:
      return HtpPerformanceMode::kBalanced;
    case QualcommDelegateOptions::EXTREME_POWER_SAVER:
      return HtpPerformanceMode::kExtremePowerSaver;
    default:
      return absl::InvalidArgumentError(
          absl::StrCat("Unsupported Qualcomm HTP performance mode ",
                       static_cast<int>(mp_mode)));
  }
}

absl::StatusOr<litert::qualcomm::QualcommOptions::LogLevel> ToLiteRtLogLevel(
    InferenceCalculatorOptions::Delegate::LiteRt::Npu::Qualcomm::LogLevel
        mp_log_level) {
  using LogLevel = litert::qualcomm::QualcommOptions::LogLevel;
  using QualcommDelegateOptions =
      InferenceCalculatorOptions::Delegate::LiteRt::Npu::Qualcomm;
  switch (mp_log_level) {
    case QualcommDelegateOptions::LOG_LEVEL_OFF:
      return LogLevel::kOff;
    case QualcommDelegateOptions::LOG_LEVEL_ERROR:
      return LogLevel::kError;
    case QualcommDelegateOptions::LOG_LEVEL_WARN:
      return LogLevel::kWarn;
    case QualcommDelegateOptions::LOG_LEVEL_INFO:
      return LogLevel::kInfo;
    case QualcommDelegateOptions::LOG_LEVEL_VERBOSE:
      return LogLevel::kVerbose;
    case QualcommDelegateOptions::LOG_LEVEL_DEBUG:
      return LogLevel::kDebug;
    default:
      return absl::InvalidArgumentError(absl::StrCat(
          "Unsupported Qualcomm log level ", static_cast<int>(mp_log_level)));
  }
}

// Returns the best supported LiteRT tensor buffer type.
absl::StatusOr<litert::TensorBufferType> ChooseBestBufferType(
    const litert::TensorBufferRequirements& buffer_requirements) {
  LITERT_ASSIGN_OR_RETURN(const auto supported_types,
                          buffer_requirements.SupportedTypes());
  for (const auto& buffer_type : kPreferredBufferTypes) {
    if (absl::c_contains(supported_types, buffer_type)) {
      return buffer_type;
    }
  }
  std::vector<std::string> supported_types_str;
  absl::c_transform(supported_types, std::back_inserter(supported_types_str),
                    litert::BufferTypeToStringCC);
  std::vector<std::string> preferred_types_str;
  absl::c_transform(kPreferredBufferTypes,
                    std::back_inserter(preferred_types_str),
                    litert::BufferTypeToStringCC);
  return absl::InternalError(absl::StrCat(
      "LiteRT buffer requirements not supported. Supported types: ",
      absl::StrJoin(supported_types_str, ", "),
      ". Preferred types: ", absl::StrJoin(preferred_types_str, ", ")));
}

absl::StatusOr<litert::HwAcceleratorSet> GetLiteRtHwAccelerators(
    const InferenceCalculatorOptions::Delegate::LiteRt& options) {
  litert::HwAcceleratorSet accelerators(litert::HwAccelerators::kNone);
  if (options.has_cpu()) {
    accelerators |= litert::HwAccelerators::kCpu;
  }
  if (options.has_gpu()) {
    accelerators |= litert::HwAccelerators::kGpu;
  }
  if (options.has_npu()) {
    accelerators |= litert::HwAccelerators::kNpu;
  }
#if defined(__EMSCRIPTEN__)
  if (options.has_webnn()) {
    accelerators = accelerators | litert::HwAccelerators::kWebNn;
  }
#endif  // __EMSCRIPTEN__
  if (accelerators.value == static_cast<int>(litert::HwAccelerators::kNone)) {
    return absl::InvalidArgumentError("No accelerator is specified");
  }
  return accelerators;
}

absl::StatusOr<litert::Environment> InitializeLiteRtEnvironment(
    const InferenceCalculatorOptions::Delegate::LiteRt& options,
    const mediapipe::GlContext* gl_context,
#if defined(__EMSCRIPTEN__)
    WebGpuService* webgpu_service,
#endif  // __EMSCRIPTEN__
    LiteRtSystemHandles system_handles) {
  std::vector<litert::EnvironmentOptions::Option> environment_options;
  if (options.has_npu()) {
    if (options.npu().has_compiler_plugin_library_path()) {
      environment_options.push_back(litert::EnvironmentOptions::Option{
          /*.tag=*/litert::EnvironmentOptions::Tag::kCompilerPluginLibraryDir,
          /*.value=*/options.npu().compiler_plugin_library_path().c_str(),
      });
    }
    if (options.npu().has_dispatch_library_path()) {
      environment_options.push_back(litert::EnvironmentOptions::Option{
          /*.tag=*/litert::EnvironmentOptions::Tag::kDispatchLibraryDir,
          /*.value=*/options.npu().dispatch_library_path().c_str(),
      });
    }
  }

#if defined(__EMSCRIPTEN__)
  if (webgpu_service != nullptr) {
    const wgpu::Device& device = webgpu_service->device();
    environment_options.push_back(litert::EnvironmentOptions::Option{
        /*.tag=*/litert::EnvironmentOptions::Tag::kWebGpuDevice,
        /*.value=*/reinterpret_cast<int64_t>(device.Get()),
    });
    environment_options.push_back(litert::EnvironmentOptions::Option{
        /*.tag=*/litert::EnvironmentOptions::Tag::kWebGpuQueue,
        /*.value=*/reinterpret_cast<int64_t>(device.GetQueue().Get()),
    });
  }

#endif  // defined(__EMSCRIPTEN__)

#if MEDIAPIPE_OPENGL_ES_VERSION >= MEDIAPIPE_OPENGL_ES_31
  if (gl_context != nullptr) {
    // We expect EGL context and display to outlive the LiteRT environment.
    environment_options.push_back(litert::EnvironmentOptions::Option{
        /*.tag=*/litert::EnvironmentOptions::Tag::kEglContext,
        /*.value=*/reinterpret_cast<int64_t>(gl_context->egl_context()),
    });
    environment_options.push_back(litert::EnvironmentOptions::Option{
        /*.tag=*/litert::EnvironmentOptions::Tag::kEglDisplay,
        /*.value=*/reinterpret_cast<int64_t>(gl_context->egl_display()),
    });
  }
#endif  // MEDIAPIPE_OPENGL_ES_VERSION >= MEDIAPIPE_OPENGL_ES_31

  if (system_handles.runtime != 0) {
    return CreateLiteRtEnvironment(system_handles, environment_options);
  }
  auto environment = litert::Environment::Create(
      litert::EnvironmentOptions(environment_options));
  if (!environment) {
    return absl::InternalError(
        absl::StrCat("Failed to create LiteRt environment: ",
                     environment.Error().Message()));
  }
  return std::move(*environment);
}

// Returns true if the given MP tensor buffer is aligned to the given alignment.
bool IsTensorAligned(const Tensor& tensor, size_t alignment) {
  const auto cpu_read_view = tensor.GetCpuReadView();
  const void* ptr = cpu_read_view.buffer<void>();
  return reinterpret_cast<uintptr_t>(ptr) % alignment == 0;
}

// Copies the source MP tensor to the destination MP tensor.
absl::Status CopyMpTensorToMpTensor(const Tensor& src_tensor,
                                    Tensor& dst_tensor) {
  RET_CHECK_EQ(src_tensor.bytes(), dst_tensor.bytes())
      << "Source and destination tensors must have the same size";
  const auto src_tensor_view = src_tensor.GetCpuReadView();
  auto dst_tensor_view = dst_tensor.GetCpuWriteView();
  std::memcpy(dst_tensor_view.buffer<void>(), src_tensor_view.buffer<void>(),
              src_tensor.bytes());
  return absl::OkStatus();
}

}  // namespace

absl::StatusOr<std::unique_ptr<InferenceRunnerLiteRt>>
InferenceRunnerLiteRt::Create(
    api2::Packet<TfLiteModelPtr> model_packet,
    const InferenceCalculatorOptions::Delegate::LiteRt& options,
    const InferenceCalculatorOptions::
        InputOutputConfig* /*absl_nullable - not yet supported*/
            input_output_config,
    MemoryManager* memory_manager,
    std::shared_ptr<mediapipe::GlContext> gl_context,
#if defined(__EMSCRIPTEN__)
    WebGpuService* webgpu_service,
#endif  // __EMSCRIPTEN__
    std::optional<litert::Options> litert_options,
    LiteRtSystemHandles system_handles) {
  ABSL_ASSIGN_OR_RETURN(auto environment, InitializeLiteRtEnvironment(
                                              options, gl_context.get(),
#if defined(__EMSCRIPTEN__)
                                              webgpu_service,
#endif  // __EMSCRIPTEN__
                                              system_handles));

  // LiteRT does not copy the model buffer, so the model packet must outlive
  // the compiled model -- except in the fully-accelerated GPU case, where
  // ClearModelPacket() intentionally drops it after compilation (see below).
  // The buffer is handed to CompiledModel::Create below, which loads and
  // compiles the model through the LiteRT runtime C API function table.
  const auto* model_allocation = (*model_packet)->allocation();
  const litert::BufferRef<uint8_t> model_buffer(model_allocation->base(),
                                                model_allocation->bytes());
  ABSL_ASSIGN_OR_RETURN(litert::HwAcceleratorSet accelerator,
                        GetLiteRtHwAccelerators(options));

  litert::Options jit_compilation_options;
  if (litert_options) {
    jit_compilation_options = *std::move(litert_options);
  } else {
    LITERT_ASSIGN_OR_RETURN(jit_compilation_options, litert::Options::Create());
  }

  LITERT_RETURN_IF_ERROR(
      jit_compilation_options.SetHardwareAccelerators(accelerator).HasValue());

  if (accelerator & litert::HwAccelerators::kCpu) {
    LITERT_ASSIGN_OR_RETURN(auto& cpu_options,
                            jit_compilation_options.GetCpuOptions());
    cpu_options.SetNumThreads(options.cpu().has_num_threads()
                                  ? options.cpu().num_threads()
                                  : GetCpuDefaultNumThreads());
    if (options.cpu().has_xnnpack() && options.cpu().xnnpack().has_flags()) {
      LITERT_RETURN_IF_ERROR(
          cpu_options.SetXNNPackFlags(options.cpu().xnnpack().flags()));
    }
  }
  if (accelerator & litert::HwAccelerators::kGpu) {
    LITERT_ASSIGN_OR_RETURN(auto& gpu_options,
                            jit_compilation_options.GetGpuOptions());
    if (options.gpu().has_precision()) {
      switch (options.gpu().precision()) {
        case InferenceCalculatorOptions::Delegate::LiteRt::Gpu::FP16:
          gpu_options.SetPrecision(litert::GpuOptions::Precision::kFp16);
          break;
        case InferenceCalculatorOptions::Delegate::LiteRt::Gpu::FP32:
          gpu_options.SetPrecision(litert::GpuOptions::Precision::kFp32);
          break;
        case InferenceCalculatorOptions::Delegate::LiteRt::Gpu::
            FP16_WITH_FP32_ACCUM:
          gpu_options.SetPrecision(
              litert::GpuOptions::Precision::kFp16WithFp32Accum);
          break;
        case InferenceCalculatorOptions::Delegate::LiteRt::Gpu::DEFAULT:
          gpu_options.SetPrecision(litert::GpuOptions::Precision::kDefault);
          break;
        default:
          return absl::InvalidArgumentError("Unsupported GPU precision");
      }
    }
    if (options.gpu().enable_constant_tensors_sharing()) {
      gpu_options.EnableConstantTensorSharing(true);
    }

    if (options.gpu().hint_fully_delegated_to_single_delegate()) {
      gpu_options.SetHintFullyDelegatedToSingleDelegate(true);
    }

    if (options.gpu().has_backend()) {
      switch (options.gpu().backend()) {
        case InferenceCalculatorOptions::Delegate::LiteRt::Gpu::OPENGL:
          gpu_options.SetBackend(litert::GpuOptions::Backend::kOpenGl);
          break;
        case InferenceCalculatorOptions::Delegate::LiteRt::Gpu::OPENCL:
          gpu_options.SetBackend(litert::GpuOptions::Backend::kOpenCl);
          break;
        case InferenceCalculatorOptions::Delegate::LiteRt::Gpu::AUTOMATIC:
          gpu_options.SetBackend(litert::GpuOptions::Backend::kAutomatic);
          break;
        case InferenceCalculatorOptions::Delegate::LiteRt::Gpu::WEBGPU:
          gpu_options.SetBackend(litert::GpuOptions::Backend::kWebGpu);
          break;
        default:
          return absl::InvalidArgumentError("Unsupported GPU backend");
      }
    }
    if (options.gpu().has_wait_type()) {
      switch (options.gpu().wait_type()) {
        case InferenceCalculatorOptions::Delegate::LiteRt::Gpu::
            WAIT_TYPE_PASSIVE:
          gpu_options.SetSyncExecutionModeWaitType(
              litert::GpuOptions::SyncExecutionModeWaitType::kPassive);
          break;
        case InferenceCalculatorOptions::Delegate::LiteRt::Gpu::
            WAIT_TYPE_ACTIVE:
          gpu_options.SetSyncExecutionModeWaitType(
              litert::GpuOptions::SyncExecutionModeWaitType::kActive);
          break;
        case InferenceCalculatorOptions::Delegate::LiteRt::Gpu::
            WAIT_TYPE_DO_NOT_WAIT:
          gpu_options.SetSyncExecutionModeWaitType(
              litert::GpuOptions::SyncExecutionModeWaitType::kDoNotWait);
          break;
        case InferenceCalculatorOptions::Delegate::LiteRt::Gpu::
            WAIT_TYPE_UNSPECIFIED:
          gpu_options.SetSyncExecutionModeWaitType(
              litert::GpuOptions::SyncExecutionModeWaitType::kDefault);
          break;
      }
    }
    if (options.gpu().has_cache_options()) {
      const auto& cache = options.gpu().cache_options();
      if (cache.has_serialize_program_cache()) {
        LITERT_RETURN_IF_ERROR(gpu_options.SetSerializeProgramCache(
            cache.serialize_program_cache()));
      }
      if (cache.has_serialization_dir()) {
        LITERT_RETURN_IF_ERROR(
            gpu_options.SetSerializationDir(cache.serialization_dir().c_str()));
      }
      if (cache.has_model_cache_key()) {
        LITERT_RETURN_IF_ERROR(
            gpu_options.SetModelCacheKey(cache.model_cache_key().c_str()));
      }
      if (cache.has_cache_only_compiled_programs()) {
        LITERT_RETURN_IF_ERROR(gpu_options.CacheCompiledProgramsOnly(
            cache.cache_only_compiled_programs()));
      }
      if (cache.has_program_cache_fd()) {
        LITERT_RETURN_IF_ERROR(
            gpu_options.SetProgramCacheFd(cache.program_cache_fd()));
      }
      if (cache.has_weight_cache_fd()) {
        LITERT_RETURN_IF_ERROR(
            gpu_options.SetWeightCacheFd(cache.weight_cache_fd()));
      }
      if (cache.has_serialize_external_tensors()) {
        LITERT_RETURN_IF_ERROR(gpu_options.SetSerializeExternalTensors(
            cache.serialize_external_tensors()));
      }
    }
    if (options.gpu().has_enable_infinite_float_capping()) {
      LITERT_RETURN_IF_ERROR(gpu_options.EnableInfiniteFloatCapping(
          options.gpu().enable_infinite_float_capping()));
    }
    if (options.gpu().has_benchmark_mode()) {
      LITERT_RETURN_IF_ERROR(
          gpu_options.EnableBenchmarkMode(options.gpu().benchmark_mode()));
    }
    if (options.gpu().has_allow_src_quantized_fc_conv_ops()) {
      LITERT_RETURN_IF_ERROR(gpu_options.EnableAllowSrcQuantizedFcConvOps(
          options.gpu().allow_src_quantized_fc_conv_ops()));
    }
    if (options.gpu().has_buffer_storage_type()) {
      switch (options.gpu().buffer_storage_type()) {
        case InferenceCalculatorOptions::Delegate::LiteRt::Gpu::
            BUFFER_STORAGE_TYPE_UNSPECIFIED:
          LITERT_RETURN_IF_ERROR(gpu_options.SetBufferStorageType(
              litert::GpuOptions::BufferStorageType::kDefault));
          break;
        case InferenceCalculatorOptions::Delegate::LiteRt::Gpu::
            BUFFER_STORAGE_TYPE_BUFFER:
          LITERT_RETURN_IF_ERROR(gpu_options.SetBufferStorageType(
              litert::GpuOptions::BufferStorageType::kBuffer));
          break;
        case InferenceCalculatorOptions::Delegate::LiteRt::Gpu::
            BUFFER_STORAGE_TYPE_TEXTURE_2D:
          LITERT_RETURN_IF_ERROR(gpu_options.SetBufferStorageType(
              litert::GpuOptions::BufferStorageType::kTexture2D));
          break;
        default:
          return absl::InvalidArgumentError(
              "Unsupported GPU buffer storage type");
      }
    }
    for (const std::string& pattern :
         options.gpu().buffer_storage_tensor_patterns()) {
      LITERT_RETURN_IF_ERROR(
          gpu_options.AddBufferStorageTensorPattern(pattern.c_str()));
    }
    if (options.gpu().has_external_tensors_mode()) {
      LITERT_RETURN_IF_ERROR(gpu_options.EnableExternalTensorsMode(
          options.gpu().external_tensors_mode()));
    }
    for (const std::string& pattern :
         options.gpu().external_tensor_patterns()) {
      LITERT_RETURN_IF_ERROR(
          gpu_options.AddExternalTensorPattern(pattern.c_str()));
    }
    if (options.gpu().has_madvise_original_shared_tensors()) {
      LITERT_RETURN_IF_ERROR(gpu_options.SetMadviseOriginalSharedTensors(
          options.gpu().madvise_original_shared_tensors()));
    }
    if (options.gpu().has_priority()) {
      switch (options.gpu().priority()) {
        case InferenceCalculatorOptions::Delegate::LiteRt::Gpu::PRIORITY_HIGH:
          LITERT_RETURN_IF_ERROR(
              gpu_options.SetPriority(litert::GpuOptions::Priority::kHigh));
          break;
        case InferenceCalculatorOptions::Delegate::LiteRt::Gpu::PRIORITY_NORMAL:
          LITERT_RETURN_IF_ERROR(
              gpu_options.SetPriority(litert::GpuOptions::Priority::kNormal));
          break;
        case InferenceCalculatorOptions::Delegate::LiteRt::Gpu::PRIORITY_LOW:
          LITERT_RETURN_IF_ERROR(
              gpu_options.SetPriority(litert::GpuOptions::Priority::kLow));
          break;
        default:
          break;
      }
    }
    if (options.gpu().has_convert_weights_on_gpu()) {
      LITERT_RETURN_IF_ERROR(gpu_options.SetConvertWeightsOnGpu(
          options.gpu().convert_weights_on_gpu()));
    }
    if (options.gpu().has_prefer_texture_weights()) {
      LITERT_RETURN_IF_ERROR(gpu_options.SetPreferTextureWeights(
          options.gpu().prefer_texture_weights()));
    }
    if (options.gpu().has_wait_for_weights_conversion_complete()) {
      LITERT_RETURN_IF_ERROR(gpu_options.WaitForWeightsConversionComplete(
          options.gpu().wait_for_weights_conversion_complete()));
    }
    if (options.gpu().has_num_steps_of_command_buffer_preparations()) {
      LITERT_RETURN_IF_ERROR(gpu_options.SetNumStepsOfCommandBufferPreparations(
          options.gpu().num_steps_of_command_buffer_preparations()));
    }
    if (options.gpu().has_opencl()) {
      const auto& opencl = options.gpu().opencl();
      if (opencl.has_hint_waiting_for_completion()) {
        LITERT_RETURN_IF_ERROR(gpu_options.HintWaitingForCompletion(
            opencl.hint_waiting_for_completion()));
      }
      if (opencl.has_kernel_batch_size()) {
        LITERT_RETURN_IF_ERROR(
            gpu_options.SetKernelBatchSize(opencl.kernel_batch_size()));
      }
    }
    if (options.gpu().has_webgpu()) {
      const auto& webgpu = options.gpu().webgpu();
      if (webgpu.has_preferred_device_substr()) {
        LITERT_RETURN_IF_ERROR(gpu_options.SetPreferredDeviceSubstr(
            webgpu.preferred_device_substr().c_str()));
      }
      if (webgpu.has_num_threads_to_upload()) {
        LITERT_RETURN_IF_ERROR(
            gpu_options.SetNumThreadsToUpload(webgpu.num_threads_to_upload()));
      }
      if (webgpu.has_num_threads_to_compile()) {
        LITERT_RETURN_IF_ERROR(gpu_options.SetNumThreadsToCompile(
            webgpu.num_threads_to_compile()));
      }
    }
    if (options.gpu().has_vulkan() &&
        options.gpu().vulkan().has_disable_shader_optimization()) {
      LITERT_RETURN_IF_ERROR(gpu_options.DisableShaderOptimization(
          options.gpu().vulkan().disable_shader_optimization()));
    }
  }
  if (accelerator & litert::HwAccelerators::kNpu) {
    if (options.npu().has_darwinn()) {
      if (options.npu().darwinn().has_inference_priority()) {
      } else {
        LITERT_ASSIGN_OR_RETURN(
            auto& google_tensor_options,
            jit_compilation_options
                .GetOptions<litert::google_tensor::GoogleTensorOptions>());
        // Default performance mode is kBalanced; this can be updated if
        // required.
        google_tensor_options.SetPerformanceMode(
            litert::google_tensor::GoogleTensorOptions::PerformanceMode::
                kBalanced);
      }
    } else if (options.npu().has_qualcomm()) {
      LITERT_ASSIGN_OR_RETURN(
          auto& qualcomm_options,
          jit_compilation_options
              .GetOptions<::litert::qualcomm::QualcommOptions>());
      const auto& qualcomm_proto = options.npu().qualcomm();
      if (qualcomm_proto.has_htp_performance_mode()) {
        LITERT_ASSIGN_OR_RETURN(
            auto mode,
            ToLiteRtHtpPerformanceMode(qualcomm_proto.htp_performance_mode()));
        qualcomm_options.SetHtpPerformanceMode(mode);
      }
      if (qualcomm_proto.has_log_level()) {
        LITERT_ASSIGN_OR_RETURN(auto log_level,
                                ToLiteRtLogLevel(qualcomm_proto.log_level()));
        qualcomm_options.SetLogLevel(log_level);
      }
    }
  }

  LITERT_ASSIGN_OR_RETURN(auto compiled_model, litert::CompiledModel::Create(
                                                   environment, model_buffer,
                                                   jit_compilation_options));
  LITERT_ASSIGN_OR_RETURN(auto signatures, compiled_model.GetSignatures());
  const int num_signatures = signatures.size();
  RET_CHECK_GT(num_signatures, 0) << "Model must have at least one signature";

  LITERT_ASSIGN_OR_RETURN(bool is_fully_accelerated,
                          compiled_model.IsFullyAccelerated());
  bool release_model_packet = false;
  // Release the model packet if the model is fully accelerated and the
  // accelerator is GPU. This is aiming to reduce the peak memory usage.
  if (is_fully_accelerated && (accelerator & litert::HwAccelerators::kGpu)) {
    release_model_packet = true;
  }
  int signature_index = 0;
  if (num_signatures > 1) {
    // Find the specified signature for models with multiple signatures.
    // If not specified, use the default signature key.
    absl::string_view signature_key =
        options.has_signature_key()
            ? options.signature_key()
            : litert::CompiledModel::DefaultSignatureKey();
    auto signature_itr =
        absl::c_find_if(signatures, [signature_key](const auto& signature) {
          return signature.Key() == signature_key;
        });
    RET_CHECK(signature_itr != signatures.end())
        << "Signature key not found: " << signature_key;
    signature_index = std::distance(signatures.begin(), signature_itr);
  }

  bool run_async = options.run_async();
  bool enable_dynamic_resize = options.enable_dynamic_resize();
  bool use_npu = options.has_npu();

  InputOutputTensorNames input_output_tensor_names =
      CreateInputOutputTensorNames(signatures);

  litert::SimpleSignature signature = std::move(signatures[signature_index]);

  std::unique_ptr<InferenceFeedbackManagerLiteRt> feedback_manager;
  if (input_output_config) {
    // The feedback manager is only using the compiled model during
    // initialization, so it's safe to move it after the feedback manager is
    // created.
    feedback_manager = std::make_unique<InferenceFeedbackManagerLiteRt>();
    ABSL_RETURN_IF_ERROR(feedback_manager->Init(*input_output_config,
                                                input_output_tensor_names,
                                                &compiled_model, signature));
  }

  const size_t num_inputs = signature.InputNames().size();
  const size_t num_outputs = signature.OutputNames().size();

  // Decided once per model: see HasDynamicDimension() for which tensors force
  // the runner onto the runtime-layout path.
  ABSL_ASSIGN_OR_RETURN(const bool has_dynamic_dimension,
                        HasDynamicDimension(signature, enable_dynamic_resize));

  auto runner =
      std::unique_ptr<InferenceRunnerLiteRt>(new InferenceRunnerLiteRt(
          memory_manager,
#if defined(__EMSCRIPTEN__)
          webgpu_service,
#endif  // __EMSCRIPTEN__
          std::move(gl_context), std::move(model_packet),
          std::make_unique<litert::Environment>(std::move(environment)),
          std::make_unique<litert::CompiledModel>(std::move(compiled_model)),
          run_async, enable_dynamic_resize, has_dynamic_dimension, use_npu,
          release_model_packet, signature_index, std::move(signature),
          std::move(input_output_tensor_names), std::move(feedback_manager)
          ));

  runner->managed_input_buffers_.resize(num_inputs);
  runner->managed_output_buffers_.resize(num_outputs);

  std::vector<litert::TensorBufferRequirements> reqs;
  reqs.reserve(num_inputs);
  for (int i = 0; i < num_inputs; ++i) {
    LITERT_ASSIGN_OR_RETURN(
        litert::TensorBufferRequirements req,
        runner->compiled_model_->GetInputBufferRequirements(signature_index, i),
        _ << "Failed to get input buffer requirements for input " << i);
    reqs.push_back(std::move(req));
  }
  runner->cached_input_buffer_requirements_ = std::move(reqs);

  if (release_model_packet) {
    runner->ClearModelPacket();
  }

  return runner;
}

InferenceRunnerLiteRt::InferenceRunnerLiteRt(
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
    )
    : memory_manager_(memory_manager),
#if defined(__EMSCRIPTEN__)
      webgpu_service_(webgpu_service),
#endif  // __EMSCRIPTEN__
      gl_context_(std::move(gl_context)),
      model_packet_(std::move(model_packet)),
      environment_(std::move(environment)),
      compiled_model_(std::move(compiled_model)),
      run_async_(run_async),
      signature_index_(signature_index),
      signature_(std::move(signature)),
      enable_dynamic_resize_(enable_dynamic_resize),
      use_npu_(use_npu),
      has_dynamic_dimension_(has_dynamic_dimension),
      input_output_tensor_names_(std::move(input_output_tensor_names)),
      feedback_manager_(std::move(feedback_manager)) {
}

InferenceRunnerLiteRt::~InferenceRunnerLiteRt() {
  absl::Status status = Close();
  if (!status.ok()) {
    ABSL_LOG(DFATAL) << "Failed to shut down InferenceRunnerLiteRt: " << status;
  }
#if MEDIAPIPE_OPENGL_ES_VERSION >= MEDIAPIPE_OPENGL_ES_30
  if (gl_context_) {
    gl_context_->Run([this]() {
      managed_input_buffers_.clear();
      managed_output_buffers_.clear();
      feedback_manager_.reset();
      compiled_model_.reset();
      environment_.reset();
    });
  }
#endif  // MEDIAPIPE_OPENGL_ES_VERSION >= MEDIAPIPE_OPENGL_ES_30
  managed_input_buffers_.clear();
  managed_output_buffers_.clear();
  feedback_manager_.reset();
  compiled_model_.reset();
  environment_.reset();
}

absl::Status InferenceRunnerLiteRt::Close() {
  return absl::OkStatus();
}

// Creates a ranked tensor type for the given input tensor.
absl::StatusOr<litert::RankedTensorType>
InferenceRunnerLiteRt::CreateInputRankedTensorType(
    const litert::RankedTensorType& tensor_type, const Tensor& mp_input_tensor,
    int tensor_index) {
  absl::Span<const int> litert_shape = tensor_type.Layout().Dimensions();

  // Check if dynamic resizing is enabled and the tensor has a dynamic
  // dimension. This mirrors the input condition of HasDynamicDimension(), so
  // `has_dynamic_dimension_` is already true whenever this branch is taken.
  if (enable_dynamic_resize_ && absl::c_linear_search(litert_shape, -1)) {
    // Check if shapes are compatible first (allowing for dynamic dimensions)
    ABSL_RETURN_IF_ERROR(
        AreTensorSpecsCompatible(tensor_type, mp_input_tensor));

    // Shapes are compatible, try resize
    const auto& mp_dims = mp_input_tensor.shape().dims;
    VLOG(1) << "Resizing input tensor " << tensor_index << " from "
            << absl::StrJoin(litert_shape, "x") << " to "
            << absl::StrJoin(mp_dims, "x");

    LITERT_RETURN_IF_ERROR(compiled_model_->ResizeInputTensor(
        signature_index_, tensor_index, mp_dims));

    // Resizing invalidates the previously computed buffer requirements of this
    // input tensor, so refresh the cached entry.
    LITERT_ASSIGN_OR_RETURN(
        litert::TensorBufferRequirements resized_requirements,
        compiled_model_->GetInputBufferRequirements(signature_index_,
                                                    tensor_index),
        _ << "Failed to get input buffer requirements for input "
          << tensor_index);
    cached_input_buffer_requirements_[tensor_index] =
        std::move(resized_requirements);

    return litert::RankedTensorType(
        tensor_type.ElementType(),
        litert::Layout(litert::BuildLayout(mp_dims)));
  } else {
    ABSL_RETURN_IF_ERROR(AreTensorSpecsEqual(tensor_type, mp_input_tensor));
  }
  return tensor_type;
}

// Creates a ranked tensor type for the given output tensor.
absl::StatusOr<litert::RankedTensorType>
InferenceRunnerLiteRt::CreateOutputRankedTensorType(
    const litert::RankedTensorType& model_tensor_type, litert::Layout layout) {
  // Models whose tensors only get their real shape at runtime (dynamic input
  // dims, rank-0 declared outputs; see HasDynamicDimension()) must use the
  // runtime layout reported by the compiled model. Otherwise the declared type
  // is already exact.
  if (!has_dynamic_dimension_) {
    return model_tensor_type;
  }

  return litert::RankedTensorType(model_tensor_type.ElementType(),
                                  std::move(layout));
}

InputOutputTensorNames InferenceRunnerLiteRt::CreateInputOutputTensorNames(
    const std::vector<litert::SimpleSignature>& signatures) {
  InputOutputTensorNames input_output_tensor_names;
  for (const auto& signature : signatures) {
    SignatureInputOutputTensorNames signature_input_output_tensor_names;
    for (const auto& tensor_name : signature.InputNames()) {
      signature_input_output_tensor_names.input_tensor_names.emplace_back(
          tensor_name);
    }
    for (const auto& tensor_name : signature.OutputNames()) {
      signature_input_output_tensor_names.output_tensor_names.emplace_back(
          tensor_name);
    }
    input_output_tensor_names[signature.Key()] =
        std::move(signature_input_output_tensor_names);
  }
  return input_output_tensor_names;
}

#if MEDIAPIPE_OPENGL_ES_VERSION >= MEDIAPIPE_OPENGL_ES_31
absl::StatusOr<litert::TensorBuffer>
InferenceRunnerLiteRt::CreateGlInputTensorBufferFromMpTensor(
    const Tensor& mp_input_tensor, const litert::RankedTensorType& tensor_type,
    std::vector<InferenceRunnerLiteRt::MpTensorReadView>&
        mp_input_tensor_views) {
  auto input_tensor_view = mp_input_tensor.GetOpenGlBufferReadView();
  LITERT_ASSIGN_OR_RETURN(
      auto litert_buffer,
      litert::TensorBuffer::CreateFromGlBuffer(
          *environment_, tensor_type, GL_SHADER_STORAGE_BUFFER,
          input_tensor_view.name(), mp_input_tensor.bytes(), /*offset=*/0));
  mp_input_tensor_views.push_back(std::move(input_tensor_view));
  return litert_buffer;
}

absl::StatusOr<litert::TensorBuffer>
InferenceRunnerLiteRt::CreateGlOutputTensorBufferFromMpTensor(
    const Tensor& mp_output_tensor, const litert::RankedTensorType& tensor_type,
    std::vector<InferenceRunnerLiteRt::MpTensorWriteView>&
        mp_output_tensor_views) {
  auto output_tensor_view = mp_output_tensor.GetOpenGlBufferWriteView();
  LITERT_ASSIGN_OR_RETURN(
      auto litert_buffer,
      litert::TensorBuffer::CreateFromGlBuffer(
          *environment_, tensor_type, GL_SHADER_STORAGE_BUFFER,
          output_tensor_view.name(), mp_output_tensor.bytes(), /*offset=*/0));
  mp_output_tensor_views.push_back(std::move(output_tensor_view));
  return litert_buffer;
}
#endif

#if defined(__EMSCRIPTEN__)
absl::StatusOr<litert::TensorBuffer>
InferenceRunnerLiteRt::CreateWebGpuInputTensorBufferFromMpTensor(
    const Tensor& mp_input_tensor, const litert::RankedTensorType& tensor_type,
    std::vector<InferenceRunnerLiteRt::MpTensorReadView>&
        mp_input_tensor_views) {
  auto input_tensor_view =
      mp_input_tensor.GetWebGpuTexture2dReadView(*webgpu_service_);
  LITERT_ASSIGN_OR_RETURN(
      auto litert_buffer,
      litert::TensorBuffer::CreateFromWebGpuTexture(
          *environment_, tensor_type, input_tensor_view.name().Get(),
          mp_input_tensor.bytes()));
  mp_input_tensor_views.push_back(std::move(input_tensor_view));
  return litert_buffer;
}

absl::StatusOr<litert::TensorBuffer>
InferenceRunnerLiteRt::CreateWebGpuOutputTensorBufferFromMpTensor(
    const Tensor& mp_output_tensor, const litert::RankedTensorType& tensor_type,
    std::vector<InferenceRunnerLiteRt::MpTensorWriteView>&
        mp_output_tensor_views) {
  auto output_tensor_view =
      mp_output_tensor.GetWebGpuTexture2dWriteView(*webgpu_service_);
  LITERT_ASSIGN_OR_RETURN(
      auto litert_buffer,
      litert::TensorBuffer::CreateFromWebGpuTexture(
          *environment_, tensor_type, output_tensor_view.name().Get(),
          mp_output_tensor.bytes()));
  mp_output_tensor_views.push_back(std::move(output_tensor_view));
  return litert_buffer;
}
#endif  // defined(__EMSCRIPTEN__)



absl::StatusOr<litert::TensorBuffer>
InferenceRunnerLiteRt::CreateCpuInputTensorBufferFromMpTensor(
    const Tensor& tensor, const litert::RankedTensorType& tensor_type,
    std::vector<InferenceRunnerLiteRt::MpTensorReadView>& mp_input_tensor_views,
    std::vector<std::unique_ptr<Tensor>>& mp_aligned_input_tensors) {
  if (IsTensorAligned(tensor, LITERT_HOST_MEMORY_BUFFER_ALIGNMENT)) {
    // Alignment requirement met, create with zero-copy.
    auto input_tensor_view = tensor.GetCpuReadView();
    const void* input_tensor_data = input_tensor_view.buffer<void>();
    LITERT_ASSIGN_OR_RETURN(
        auto litert_buffer,
        litert::TensorBuffer::CreateFromHostMemory(
            *environment_, tensor_type, const_cast<void*>(input_tensor_data),
            tensor.bytes()));
    mp_input_tensor_views.push_back(std::move(input_tensor_view));
    return litert_buffer;
  }
  // Alignment requirement not met, create a new aligned MP tensor.
  ABSL_LOG_FIRST_N(WARNING, 1)
      << "Input tensor memory is not aligned according to LiteRt's "
         "memory "
         "alignment requirements. Reallocating memory and copying data.";
  ABSL_ASSIGN_OR_RETURN(
      Tensor aligned_mp_input_tensor,
      CreateTensorFromLiteRtRankedTensorType(
          tensor_type, memory_manager_, LITERT_HOST_MEMORY_BUFFER_ALIGNMENT));
  auto aligned_mp_input_tensor_ptr =
      std::make_unique<Tensor>(std::move(aligned_mp_input_tensor));
  ABSL_RETURN_IF_ERROR(
      CopyMpTensorToMpTensor(tensor, *aligned_mp_input_tensor_ptr));
  auto input_tensor_view = aligned_mp_input_tensor_ptr->GetCpuReadView();
  const void* input_tensor_data = input_tensor_view.buffer<void>();
  LITERT_ASSIGN_OR_RETURN(
      auto litert_buffer,
      litert::TensorBuffer::CreateFromHostMemory(
          *environment_, tensor_type, const_cast<void*>(input_tensor_data),
          aligned_mp_input_tensor_ptr->bytes()));
  mp_aligned_input_tensors.push_back(std::move(aligned_mp_input_tensor_ptr));
  mp_input_tensor_views.push_back(std::move(input_tensor_view));
  return litert_buffer;
}

absl::StatusOr<litert::TensorBuffer>
InferenceRunnerLiteRt::CreateCpuOutputTensorBufferFromMpTensor(
    const Tensor& mp_output_tensor, const litert::RankedTensorType& tensor_type,
    std::vector<InferenceRunnerLiteRt::MpTensorWriteView>&
        mp_output_tensor_views) {
  const int output_tensor_size_bytes = mp_output_tensor.bytes();
  auto output_tensor_view = mp_output_tensor.GetCpuWriteView();
  void* output_tensor_data = output_tensor_view.buffer<void>();
  LITERT_ASSIGN_OR_RETURN(auto output_buffer,
                          litert::TensorBuffer::CreateFromHostMemory(
                              *environment_, tensor_type, output_tensor_data,
                              output_tensor_size_bytes));
  mp_output_tensor_views.push_back(std::move(output_tensor_view));
  return output_buffer;
}

absl::StatusOr<litert::TensorBuffer>
InferenceRunnerLiteRt::CreateInputTensorBufferFromMpTensor(
    litert::TensorBufferType buffer_type, const Tensor& tensor,
    const litert::RankedTensorType& tensor_type, InferenceRunContext& ctx) {
  switch (buffer_type) {
    case litert::TensorBufferType::kHostMemory:
      return CreateCpuInputTensorBufferFromMpTensor(
          tensor, tensor_type, ctx.active_input_views, ctx.temporary_tensors);
#if MEDIAPIPE_OPENGL_ES_VERSION >= MEDIAPIPE_OPENGL_ES_31
    case litert::TensorBufferType::kGlBuffer:
      return CreateGlInputTensorBufferFromMpTensor(tensor, tensor_type,
                                                   ctx.active_input_views);
#endif  // MEDIAPIPE_OPENGL_ES_VERSION >= MEDIAPIPE_OPENGL_ES_31
#if defined(__EMSCRIPTEN__)
    case litert::TensorBufferType::kWebGpuTexture:
      if (webgpu_service_ != nullptr) {
        return CreateWebGpuInputTensorBufferFromMpTensor(
            tensor, tensor_type, ctx.active_input_views);
      } else {
        return absl::InvalidArgumentError(absl::StrCat(
            "Unsupported buffer type to create input tensor: ", buffer_type));
      }
#endif  // defined(__EMSCRIPTEN__)
    default:
      return absl::InvalidArgumentError(absl::StrCat(
          "Unsupported buffer type to create input tensor: ", buffer_type));
  }
}

absl::StatusOr<bool> InferenceRunnerLiteRt::NeedsReallocation(
    const std::optional<litert::TensorBuffer>& managed_buffer,
    size_t buffer_size, const litert::RankedTensorType& tensor_type) const {
  if (!managed_buffer.has_value()) return true;

  LITERT_ASSIGN_OR_RETURN(size_t cache_size, managed_buffer->Size());
  if (cache_size != buffer_size) return true;

  LITERT_ASSIGN_OR_RETURN(auto cached_tensor_type,
                          managed_buffer->TensorType());
  return !absl::c_equal(cached_tensor_type.Layout().Dimensions(),
                        tensor_type.Layout().Dimensions());
}

absl::StatusOr<std::optional<litert::TensorBuffer>>
InferenceRunnerLiteRt::TryGetFeedbackBuffer(
    bool is_input, int tensor_index, absl::string_view tensor_name) const {
  if (!feedback_manager_) {
    return std::nullopt;
  }
  bool is_feedback_tensor =
      is_input ? feedback_manager_->IsFeedbackInputTensorAtIndex(tensor_index)
               : feedback_manager_->IsFeedbackOutputTensorAtIndex(tensor_index);
  if (!is_feedback_tensor) {
    return std::nullopt;
  }
  if (!feedback_manager_->Contains(tensor_name)) {
    return absl::InternalError(absl::StrCat(
        "Feedback tensor buffer not found for tensor: ", tensor_name));
  }
  // TODO: Perform lookups based on tensor index, not name.
  LITERT_ASSIGN_OR_RETURN(
      const litert::TensorBuffer& feedback_buffer,
      feedback_manager_->GetFeedbackTensorBuffer(tensor_name));
  LITERT_ASSIGN_OR_RETURN(litert::TensorBuffer buffer_dup,
                          feedback_buffer.Duplicate());
  return std::move(buffer_dup);
}

absl::Status InferenceRunnerLiteRt::PrepareInputBuffers(
    const litert::SimpleSignature& signature, const TensorSpan& tensor_span,
    InferenceRunContext& ctx) {
  const std::vector<litert::StringView> input_names = signature.InputNames();
  RET_CHECK_EQ(input_names.size(),
               tensor_span.size() + GetNumberOfFeedbackTensors());
  std::vector<std::optional<litert::TensorBuffer>> litert_inputs(
      input_names.size());

  // When feedback tensors are present then the model has more inputs than MP
  // has tensors, so we populate feedback buffers directly and map the remaining
  // signature input indices to their corresponding MP Tensors.
  int mp_tensor_index = 0;
  struct TensorIndexPair {
    const Tensor* tensor;
    int idx;
  };
  std::vector<TensorIndexPair> mp_input_tensors;
  mp_input_tensors.reserve(input_names.size());
  for (int i = 0; i < input_names.size(); ++i) {
    ABSL_ASSIGN_OR_RETURN(
        std::optional<litert::TensorBuffer> feedback_buffer,
        TryGetFeedbackBuffer(/*is_input=*/true, i, input_names[i]));
    if (feedback_buffer) {
      litert_inputs[i] = std::move(*feedback_buffer);
    } else {
      // We have a MP tensor for this model input tensor.
      mp_input_tensors.push_back(
          {.tensor = &tensor_span[mp_tensor_index++], .idx = i});
    }
  }

  // If two runners run in parallel and acquire input tensor ReadViews in
  // different orders, they may deadlock. See b/575484155 for details. This is
  // because Tensors don't actually acquire read locks, but always full locks.
  // Thus, sort tensors by address to ensure consistent lock acquisition order.
  absl::c_sort(mp_input_tensors,
               [](const TensorIndexPair& a, const TensorIndexPair& b) {
                 return std::less<const Tensor*>{}(a.tensor, b.tensor);
               });

  // Also check if the same tensor is passed multiple times. This would lock
  // the same tensor in multiple places, leading to a deadlock.
  auto adjacent_it = std::adjacent_find(
      mp_input_tensors.begin(), mp_input_tensors.end(),
      [](const TensorIndexPair& a, const TensorIndexPair& b) {
        return a.tensor == b.tensor;
      });
  RET_CHECK(adjacent_it == mp_input_tensors.end())
      << "Tensor '" << input_names[adjacent_it->idx]
      << "' is passed multiple times. This is currently not supported.";

  for (const TensorIndexPair& info : mp_input_tensors) {
    int i = info.idx;

    ABSL_ASSIGN_OR_RETURN(
        litert::TensorBufferType buffer_type,
        ChooseBestBufferType(cached_input_buffer_requirements_[i]));

    LITERT_ASSIGN_OR_RETURN(const litert::RankedTensorType model_tensor_type,
                            signature.InputTensorType(i));


    // Note: CreateInputRankedTensorType() may refresh
    // cached_input_buffer_requirements_[i], so it must be read after this call.
    ABSL_ASSIGN_OR_RETURN(
        const litert::RankedTensorType tensor_type,
        CreateInputRankedTensorType(model_tensor_type, *tensor_to_use, i));

    LITERT_ASSIGN_OR_RETURN(size_t buffer_size,
                            cached_input_buffer_requirements_[i].BufferSize());
    const bool size_mismatch = (buffer_size != tensor_to_use->bytes());
    if (size_mismatch) {
      // Accelerators (e.g. NPU hardware alignment or GPU vec4/PHWC4 padding)
      // may require the physical buffer to be padded, making it larger than the
      // logical tensor size.
      //
      // If there's a mismatch, we allow the LiteRT buffer to be larger and
      // force managed copying because zero-copy is impossible without a
      // matching padding in MP tensors.
      RET_CHECK_GT(buffer_size, tensor_to_use->bytes())
          << "LiteRt input buffer size is smaller than MP input tensor bytes.";
      ABSL_LOG_FIRST_N(WARNING, 1) << absl::StrCat(
          "LiteRT input buffer size (", buffer_size,
          ") differs from MP input tensor size (", tensor_to_use->bytes(),
          "); forcing managed copy fallback.");
    }

    const bool use_managed =
        size_mismatch ||
        absl::c_linear_search(kLiteRtManagedBufferTypes, buffer_type);
    if (use_managed) {
      ABSL_ASSIGN_OR_RETURN(bool needs_realloc,
                            NeedsReallocation(managed_input_buffers_[i],
                                              buffer_size, tensor_type));

      if (needs_realloc) {
        LITERT_ASSIGN_OR_RETURN(
            auto litert_buffer,
            litert::TensorBuffer::CreateManaged(*environment_, buffer_type,
                                                tensor_type, buffer_size));
        managed_input_buffers_[i] = std::move(litert_buffer);
      }
      ABSL_RETURN_IF_ERROR(CopyMpTensorToLiteRtBuffer(
          *tensor_to_use, *managed_input_buffers_[i]));
      LITERT_ASSIGN_OR_RETURN(litert_inputs[i],
                              managed_input_buffers_[i]->Duplicate());
    } else {
      ABSL_ASSIGN_OR_RETURN(litert_inputs[i],
                            CreateInputTensorBufferFromMpTensor(
                                buffer_type, *tensor_to_use, tensor_type, ctx));
    }
  }

  ctx.litert_inputs.reserve(input_names.size());
  for (std::optional<litert::TensorBuffer>& input : litert_inputs) {
    RET_CHECK(input.has_value());
    ctx.litert_inputs.push_back(*std::move(input));
  }

  return absl::OkStatus();
}

absl::StatusOr<std::vector<Tensor>>
InferenceRunnerLiteRt::CreateMpOutputTensors(
    const litert::SimpleSignature& signature,
    const std::vector<litert::Layout>& output_tensor_layouts) {
  const std::vector<litert::StringView> output_names = signature.OutputNames();
  std::vector<Tensor> mp_output_tensors;
  mp_output_tensors.reserve(output_names.size());

  RET_CHECK_EQ(output_names.size(), output_tensor_layouts.size());

  for (int i = 0; i < output_names.size(); ++i) {
    if (feedback_manager_ &&
        feedback_manager_->IsFeedbackOutputTensorAtIndex(i)) {
      continue;
    }
    LITERT_ASSIGN_OR_RETURN(const litert::RankedTensorType model_tensor_type,
                            signature.OutputTensorType(i));
    ABSL_ASSIGN_OR_RETURN(const litert::RankedTensorType tensor_type,
                          CreateOutputRankedTensorType(
                              model_tensor_type, output_tensor_layouts[i]));

    ABSL_ASSIGN_OR_RETURN(
        Tensor mp_output_tensor,
        CreateTensorFromLiteRtRankedTensorType(
            tensor_type, memory_manager_, LITERT_HOST_MEMORY_BUFFER_ALIGNMENT));
    mp_output_tensors.push_back(std::move(mp_output_tensor));
  }
  return mp_output_tensors;
}

absl::StatusOr<litert::TensorBuffer>
InferenceRunnerLiteRt::CreateOutputTensorBufferFromMpTensor(
    litert::TensorBufferType buffer_type, const Tensor& tensor,
    const litert::RankedTensorType& tensor_type, InferenceRunContext& ctx) {
  switch (buffer_type) {
    case litert::TensorBufferType::kHostMemory:
      return CreateCpuOutputTensorBufferFromMpTensor(tensor, tensor_type,
                                                     ctx.active_output_views);
#if MEDIAPIPE_OPENGL_ES_VERSION >= MEDIAPIPE_OPENGL_ES_31
    case litert::TensorBufferType::kGlBuffer:
      return CreateGlOutputTensorBufferFromMpTensor(tensor, tensor_type,
                                                    ctx.active_output_views);
#endif  // MEDIAPIPE_OPENGL_ES_VERSION >= MEDIAPIPE_OPENGL_ES_31
#if defined(__EMSCRIPTEN__)
    case litert::TensorBufferType::kWebGpuTexture:
      if (webgpu_service_ != nullptr) {
        return CreateWebGpuOutputTensorBufferFromMpTensor(
            tensor, tensor_type, ctx.active_output_views);
      } else {
        return absl::InvalidArgumentError(absl::StrCat(
            "Unsupported buffer type to create output tensor: ", buffer_type));
      }
#endif  // defined(__EMSCRIPTEN__)
    default:
      return absl::InvalidArgumentError(absl::StrCat(
          "Unsupported buffer type to create output tensor: ", buffer_type));
  }
}

absl::Status InferenceRunnerLiteRt::PrepareOutputBuffers(
    const litert::SimpleSignature& signature,
    const std::vector<litert::Layout>& output_tensor_layouts,
    const std::vector<Tensor>& mp_output_tensors, InferenceRunContext& ctx) {
  const std::vector<litert::StringView> output_names = signature.OutputNames();
  ctx.litert_outputs.reserve(output_names.size());

  int mp_output_tensor_index = 0;
  for (int i = 0; i < output_names.size(); ++i) {
    ABSL_ASSIGN_OR_RETURN(
        std::optional<litert::TensorBuffer> feedback_buffer,
        TryGetFeedbackBuffer(/*is_input=*/false, i, output_names[i]));
    if (feedback_buffer.has_value()) {
      ctx.litert_outputs.push_back(*std::move(feedback_buffer));
      continue;
    }
    LITERT_ASSIGN_OR_RETURN(const litert::RankedTensorType model_tensor_type,
                            signature.OutputTensorType(i));
    ABSL_ASSIGN_OR_RETURN(const litert::RankedTensorType tensor_type,
                          CreateOutputRankedTensorType(
                              model_tensor_type, output_tensor_layouts[i]));

    LITERT_ASSIGN_OR_RETURN(
        const auto output_buffer_requirements,
        compiled_model_->GetOutputBufferRequirements(signature_index_, i),
        _ << "Failed to get output buffer requirements for output " << i);

    LITERT_ASSIGN_OR_RETURN(size_t buffer_size,
                            output_buffer_requirements.BufferSize());
    const int mp_output_tensor_bytes =
        mp_output_tensors[mp_output_tensor_index].bytes();
    const bool size_mismatch = (buffer_size != mp_output_tensor_bytes);
    if (size_mismatch) {
      // Accelerators may require padded output buffers. If they differ from
      // the logical MP tensor size, we allow it but force managed copying.
      RET_CHECK_GT(buffer_size, mp_output_tensor_bytes)
          << "LiteRt output buffer size is smaller than MP output tensor "
             "bytes.";
      ABSL_LOG_FIRST_N(WARNING, 1) << absl::StrCat(
          "LiteRT output buffer size (", buffer_size,
          ") differs from MP output tensor size (", mp_output_tensor_bytes,
          "); forcing managed copy fallback.");
    }

    ABSL_ASSIGN_OR_RETURN(litert::TensorBufferType buffer_type,
                          ChooseBestBufferType(output_buffer_requirements));

    const bool use_managed =
        size_mismatch ||
        absl::c_linear_search(kLiteRtManagedBufferTypes, buffer_type);
    if (use_managed) {
      ABSL_ASSIGN_OR_RETURN(bool needs_realloc,
                            NeedsReallocation(managed_output_buffers_[i],
                                              buffer_size, tensor_type));

      if (needs_realloc) {
        LITERT_ASSIGN_OR_RETURN(
            auto litert_buffer,
            litert::TensorBuffer::CreateManaged(*environment_, buffer_type,
                                                tensor_type, buffer_size));
        managed_output_buffers_[i] = std::move(litert_buffer);
      }
      LITERT_ASSIGN_OR_RETURN(litert::TensorBuffer duplicated_buffer,
                              managed_output_buffers_[i]->Duplicate());
      ctx.litert_outputs.push_back(std::move(duplicated_buffer));
      mp_output_tensor_index++;
    } else {
      ABSL_ASSIGN_OR_RETURN(
          litert::TensorBuffer output_tensor,
          CreateOutputTensorBufferFromMpTensor(
              buffer_type, mp_output_tensors[mp_output_tensor_index++],
              tensor_type, ctx));
      ctx.litert_outputs.push_back(std::move(output_tensor));
    }
  }
  return absl::OkStatus();
}


absl::StatusOr<std::vector<Tensor>> InferenceRunnerLiteRt::Run(
    CalculatorContext* cc, const TensorSpan& tensor_span) {
  InferenceRunContext ctx;



  ABSL_RETURN_IF_ERROR(PrepareInputBuffers(signature_, tensor_span, ctx));


  // Get final output tensor layouts from the compiled model.
  LITERT_ASSIGN_OR_RETURN(
      std::vector<litert::Layout> output_tensor_layouts,
      compiled_model_->GetOutputTensorLayouts(
          signature_index_, /*update_allocation=*/has_dynamic_dimension_));

  ABSL_ASSIGN_OR_RETURN(
      std::vector<Tensor> mp_output_tensors,
      CreateMpOutputTensors(signature_, output_tensor_layouts));

  ABSL_RETURN_IF_ERROR(PrepareOutputBuffers(signature_, output_tensor_layouts,
                                            mp_output_tensors, ctx));


  // Execute model.
  if (run_async_) {
    return absl::UnimplementedError(
        "Asynchronous LiteRT execution is not supported");
  } else {
    LITERT_RETURN_IF_ERROR(compiled_model_->Run(
        signature_index_, ctx.litert_inputs, ctx.litert_outputs));


    ctx.active_output_views.clear();
    ctx.active_input_views.clear();

    ABSL_RETURN_IF_ERROR(CopyLiteRtManagedOutputBuffersToMpTensors(
        ctx.litert_outputs, mp_output_tensors));
  }
  return std::move(mp_output_tensors);
}

absl::Status InferenceRunnerLiteRt::CopyLiteRtManagedOutputBuffersToMpTensors(
    std::vector<litert::TensorBuffer>& output_buffers,
    std::vector<Tensor>& mp_output_tensors) {
  RET_CHECK_EQ(output_buffers.size(),
               mp_output_tensors.size() + GetNumberOfFeedbackTensors());
  int mp_output_tensor_index = 0;
  for (int i = 0; i < output_buffers.size(); ++i) {
    if (feedback_manager_ &&
        feedback_manager_->IsFeedbackOutputTensorAtIndex(i)) {
      continue;
    }
    LITERT_ASSIGN_OR_RETURN(
        size_t litert_size, output_buffers[i].Size(),
        _ << "Failed to get output buffer size for output " << i);
    LITERT_ASSIGN_OR_RETURN(
        auto buffer_type, output_buffers[i].BufferType(),
        _ << "Failed to get output buffer type for output " << i);

    // If there was a physical/logical size mismatch, we must force copying the
    // output back to the logical MP tensor.
    const bool size_mismatch =
        (litert_size != mp_output_tensors[mp_output_tensor_index].bytes());
    if (size_mismatch ||
        absl::c_linear_search(kLiteRtManagedBufferTypes, buffer_type)) {
      ABSL_RETURN_IF_ERROR(CopyLiteRtBufferToMpTensor(
          output_buffers[i], mp_output_tensors[mp_output_tensor_index]));
    }
    mp_output_tensor_index++;
  }
  return absl::OkStatus();
}

}  // namespace mediapipe
