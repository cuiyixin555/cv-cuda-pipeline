#include "cv-cuda-pipeline/util/sync_wait.h"

#include <fcntl.h>

#include <utility>

#include "absl/log/absl_check.h"
#include "absl/status/status.h"
#include "absl/time/time.h"
#include "cv-cuda-pipeline/framework/formats/shared_fd.h"
#include "cv-cuda-pipeline/framework/formats/unique_fd.h"
#include "cv-cuda-pipeline/framework/port/benchmark.h"
#include "cv-cuda-pipeline/framework/port/gmock.h"
#include "cv-cuda-pipeline/framework/port/gtest.h"
#include "cv-cuda-pipeline/framework/port/status_matchers.h"

#include <sys/timerfd.h>

namespace mediapipe {
namespace {

struct TestTimer {
  UniqueFd fd;
};

TestTimer CreateTestTimer(absl::Duration duration) {
  const int fd = timerfd_create(CLOCK_MONOTONIC, /*flags*/ 0);
  ABSL_CHECK_NE(fd, -1);
  TestTimer timer = {UniqueFd(fd)};

  struct itimerspec new_value;
  new_value.it_value = absl::ToTimespec(duration);
  new_value.it_interval.tv_sec = 0;
  new_value.it_interval.tv_nsec = 0;

  ABSL_CHECK_NE(timerfd_settime(timer.fd.Get(), /*flags=*/0, &new_value,
                                /*oldtimer=*/nullptr),
                -1);

  return timer;
}

TEST(SyncWait, WorksWithIndefiniteTimeout) {
  TestTimer timer = CreateTestTimer(absl::Milliseconds(2));
  MP_EXPECT_OK(SyncWait(timer.fd, absl::InfiniteDuration()));
}

TEST(SyncWait, WorksWithSharedFd) {
  TestTimer timer = CreateTestTimer(absl::Milliseconds(2));
  SharedFd fd(std::move(timer).fd);
  MP_EXPECT_OK(SyncWait(fd, absl::InfiniteDuration()));
}

TEST(SyncWait, WorksWithDefiniteTimeout) {
  TestTimer timer = CreateTestTimer(absl::Milliseconds(5));
  MP_EXPECT_OK(SyncWait(timer.fd, absl::Milliseconds(10)));
}

TEST(SyncWait, WorksWithReadyFd) {
  TestTimer timer = CreateTestTimer(absl::Milliseconds(5));
  // timer.fd is not available for read
  MP_EXPECT_OK(SyncWait(timer.fd, absl::InfiniteDuration()));

  // timer.fd is available for read
  MP_EXPECT_OK(SyncWait(timer.fd, absl::InfiniteDuration()));
  MP_EXPECT_OK(SyncWait(timer.fd, absl::Milliseconds(1)));
}

TEST(SyncWait, ReportsTimeout) {
  TestTimer timer = CreateTestTimer(absl::Milliseconds(100));
  EXPECT_THAT(SyncWait(timer.fd, absl::Milliseconds(5)),
              StatusIs(absl::StatusCode::kDeadlineExceeded));
}

TEST(SyncWait, ReportsInvalidFd) {
  const int fd = -1;
  EXPECT_THAT(SyncWait(fd, absl::InfiniteDuration()),
              StatusIs(absl::StatusCode::kInternal));
}

TEST(SyncWait, IsSignaledWorks) {
  TestTimer timer = CreateTestTimer(absl::Milliseconds(100));
  MP_ASSERT_OK_AND_ASSIGN(bool is_signaled, IsSignaled(timer.fd));
  EXPECT_FALSE(is_signaled);

  MP_ASSERT_OK(SyncWait(timer.fd, absl::InfiniteDuration()));

  MP_ASSERT_OK_AND_ASSIGN(is_signaled, IsSignaled(timer.fd));
  EXPECT_TRUE(is_signaled);
}

TEST(SyncWait, IsSignaledWorksWithSharedFd) {
  TestTimer timer = CreateTestTimer(absl::Milliseconds(100));
  SharedFd fd(std::move(timer).fd);
  MP_ASSERT_OK_AND_ASSIGN(bool is_signaled, IsSignaled(fd));
  EXPECT_FALSE(is_signaled);

  MP_ASSERT_OK(SyncWait(fd, absl::InfiniteDuration()));

  MP_ASSERT_OK_AND_ASSIGN(is_signaled, IsSignaled(fd));
  EXPECT_TRUE(is_signaled);
}

TEST(SyncWait, IsSignaledReportsInvalidFd) {
  const int fd = -1;
  EXPECT_THAT(IsSignaled(fd), StatusIs(absl::StatusCode::kInternal));
}

void BM_SyncWaitZeroTimeout(benchmark::State& state) {
  // Non blocking waits will be used and timer canceled automatically after
  // benchmark completion.
  TestTimer timer = CreateTestTimer(absl::Minutes(1));
  for (auto s : state) {
    ABSL_CHECK_EQ(SyncWait(timer.fd, absl::ZeroDuration()).code(),
                  absl::StatusCode::kDeadlineExceeded);
  }
}
BENCHMARK(BM_SyncWaitZeroTimeout);

}  // namespace
}  // namespace mediapipe
