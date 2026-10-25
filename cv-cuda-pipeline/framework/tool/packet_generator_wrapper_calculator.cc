#include "cv-cuda-pipeline/framework/tool/packet_generator_wrapper_calculator.h"

#include "absl/status/status.h"
#include "cv-cuda-pipeline/framework/calculator_base.h"
#include "cv-cuda-pipeline/framework/calculator_registry.h"
#include "cv-cuda-pipeline/framework/output_side_packet.h"
#include "cv-cuda-pipeline/framework/packet_generator.h"
#include "cv-cuda-pipeline/framework/packet_set.h"
#include "cv-cuda-pipeline/framework/port/status_macros.h"
#include "cv-cuda-pipeline/framework/tool/packet_generator_wrapper_calculator.pb.h"

namespace mediapipe {

absl::Status PacketGeneratorWrapperCalculator::GetContract(
    CalculatorContract* cc) {
  const auto& options =
      cc->Options<::mediapipe::PacketGeneratorWrapperCalculatorOptions>();
  ABSL_ASSIGN_OR_RETURN(auto static_access,
                        mediapipe::internal::StaticAccessToGeneratorRegistry::
                            CreateByNameInNamespace(
                                options.package(), options.packet_generator()));
  ABSL_RETURN_IF_ERROR(
      static_access->FillExpectations(
          options.options(), &cc->InputSidePackets(), &cc->OutputSidePackets()))
          .SetPrepend()
      << options.packet_generator() << "::FillExpectations() failed: ";
  return absl::OkStatus();
}

absl::Status PacketGeneratorWrapperCalculator::Open(CalculatorContext* cc) {
  const auto& options =
      cc->Options<::mediapipe::PacketGeneratorWrapperCalculatorOptions>();
  ABSL_ASSIGN_OR_RETURN(auto static_access,
                        mediapipe::internal::StaticAccessToGeneratorRegistry::
                            CreateByNameInNamespace(
                                options.package(), options.packet_generator()));
  mediapipe::PacketSet output_packets(cc->OutputSidePackets().TagMap());
  ABSL_RETURN_IF_ERROR(static_access->Generate(options.options(),
                                               cc->InputSidePackets(),
                                               &output_packets))
          .SetPrepend()
      << options.packet_generator() << "::Generate() failed: ";
  for (auto id = output_packets.BeginId(); id < output_packets.EndId(); ++id) {
    cc->OutputSidePackets().Get(id).Set(output_packets.Get(id));
  }
  return absl::OkStatus();
}

absl::Status PacketGeneratorWrapperCalculator::Process(CalculatorContext* cc) {
  return absl::OkStatus();
}

REGISTER_CALCULATOR(PacketGeneratorWrapperCalculator);

}  // namespace mediapipe
