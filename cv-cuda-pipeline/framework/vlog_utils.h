#ifndef MEDIAPIPE_FRAMEWORK_VLOG_UTILS_H_
#define MEDIAPIPE_FRAMEWORK_VLOG_UTILS_H_

#include "absl/strings/string_view.h"

namespace mediapipe {

// Splits long messages by new lines before logging each line separately. To
// ensure the message is only generated when VLOG is enabled, use this function
// in a VLOG_IS_ON() block:
// if (VLOG_IS_ON(1)) {
//   VlogLargeMessage(
//       /*verbose_level=*/1, GenerateDebugString());
// }
void VlogLargeMessage(int verbose_level, absl::string_view message);

}  // namespace mediapipe

#endif  // MEDIAPIPE_FRAMEWORK_VLOG_UTILS_H_
