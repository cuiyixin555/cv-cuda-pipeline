#include "cv-cuda-pipeline/tasks/c/audio/core/running_mode_converter.h"

#include "cv-cuda-pipeline/framework/port/gtest.h"
#include "cv-cuda-pipeline/tasks/c/audio/core/common.h"
#include "cv-cuda-pipeline/tasks/cc/audio/core/running_mode.h"

namespace {

using ::mediapipe::tasks::audio::core::RunningMode;
using ::mediapipe::tasks::c::audio::core::CppConvertToRunningMode;

TEST(RunningModeConverterTest, CppConvertToRunningModeTest) {
  EXPECT_EQ(*CppConvertToRunningMode(kMpAudioRunningModeAudioClips),
            RunningMode::AUDIO_CLIPS);
  EXPECT_EQ(*CppConvertToRunningMode(kMpAudioRunningModeAudioStream),
            RunningMode::AUDIO_STREAM);
}

}  // namespace
