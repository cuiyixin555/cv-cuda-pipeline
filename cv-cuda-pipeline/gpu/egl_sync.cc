#include "cv-cuda-pipeline/gpu/egl_sync.h"

#include <cstring>
#include <utility>

#include "absl/log/absl_log.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "cv-cuda-pipeline/framework/deps/no_destructor.h"
#include "cv-cuda-pipeline/framework/port/ret_check.h"
#include "cv-cuda-pipeline/framework/port/status_macros.h"
#include "cv-cuda-pipeline/gpu/egl_base.h"
#include "cv-cuda-pipeline/gpu/egl_errors.h"

namespace mediapipe {

namespace {

PFNEGLCREATESYNCKHRPROC eglCreateSyncKHR;
PFNEGLWAITSYNCKHRPROC eglWaitSyncKHR;
PFNEGLCLIENTWAITSYNCKHRPROC eglClientWaitSyncKHR;
PFNEGLDESTROYSYNCKHRPROC eglDestroySyncKHR;
PFNEGLGETSYNCATTRIBKHRPROC eglGetSyncAttribKHR;

bool HasExtension(EGLDisplay display, const char* extension) {
  const char* extensions = eglQueryString(display, EGL_EXTENSIONS);
  return extensions && std::strstr(extensions, extension);
}

absl::Status CheckEglFenceSyncSupported(EGLDisplay display) {
  static bool supported = HasExtension(display, "EGL_KHR_fence_sync");
  if (supported) {
    return absl::OkStatus();
  }
  return absl::UnavailableError("EGL_KHR_fence_sync unavailable.");
}

absl::Status CheckEglWaitSyncSupported(EGLDisplay display) {
  static bool supported = HasExtension(display, "EGL_KHR_wait_sync");
  if (supported) {
    return absl::OkStatus();
  }
  return absl::UnavailableError("EGL_KHR_wait_sync unavailable.");
}

absl::Status CheckEglSyncSupported(EGLDisplay egl_display) {
  static NoDestructor<absl::Status> support_status([&]() -> absl::Status {
    ABSL_RETURN_IF_ERROR(CheckEglFenceSyncSupported(egl_display));
    ABSL_RETURN_IF_ERROR(CheckEglWaitSyncSupported(egl_display));

    RET_CHECK(eglCreateSyncKHR = reinterpret_cast<PFNEGLCREATESYNCKHRPROC>(
                  eglGetProcAddress("eglCreateSyncKHR")));
    RET_CHECK(eglWaitSyncKHR = reinterpret_cast<PFNEGLWAITSYNCKHRPROC>(
                  eglGetProcAddress("eglWaitSyncKHR")));
    RET_CHECK(eglClientWaitSyncKHR =
                  reinterpret_cast<PFNEGLCLIENTWAITSYNCKHRPROC>(
                      eglGetProcAddress("eglClientWaitSyncKHR")));
    RET_CHECK(eglDestroySyncKHR = reinterpret_cast<PFNEGLDESTROYSYNCKHRPROC>(
                  eglGetProcAddress("eglDestroySyncKHR")));
    RET_CHECK(eglGetSyncAttribKHR =
                  reinterpret_cast<PFNEGLGETSYNCATTRIBKHRPROC>(
                      eglGetProcAddress("eglGetSyncAttribKHR")));
    return absl::OkStatus();
  }());
  return *support_status;
}

}  // namespace

absl::StatusOr<EglSync> EglSync::Create(EGLDisplay display) {
  ABSL_RETURN_IF_ERROR(CheckEglSyncSupported(display));

  const EGLSyncKHR egl_sync =
      eglCreateSyncKHR(display, EGL_SYNC_FENCE_KHR, nullptr);
  RET_CHECK_NE(egl_sync, EGL_NO_SYNC_KHR)
      << "Create/eglCreateSyncKHR failed: " << GetEglError();
  return EglSync(display, egl_sync);
}

bool EglSync::IsSupported(EGLDisplay display) {
  return CheckEglSyncSupported(display).ok();
}

EglSync::EglSync(EglSync&& sync) { *this = std::move(sync); }

EglSync& EglSync::operator=(EglSync&& sync) {
  if (this != &sync) {
    Invalidate();

    using std::swap;
    sync_ = std::exchange(sync.sync_, EGL_NO_SYNC_KHR);
    display_ = std::exchange(sync.display_, EGL_NO_DISPLAY);
  }
  return *this;
}

void EglSync::Invalidate() {
  if (sync_ == EGL_NO_SYNC_KHR || display_ == EGL_NO_DISPLAY) {
    return;
  }

  const absl::Status egl_sync_support = CheckEglSyncSupported(display_);
  if (!egl_sync_support.ok()) {
    ABSL_LOG(DFATAL) << "Attempt to destroy an EGL sync: " << egl_sync_support;
    return;
  }

  // Needs extension: EGL_KHR_fence_sync (EGL) / GL_OES_EGL_sync (OpenGL ES).
  // Note: we're doing nothing when the function pointer is nullptr, or the
  // call returns EGL_FALSE.
  const EGLBoolean result = eglDestroySyncKHR(display_, sync_);
  if (result == EGL_FALSE) {
    ABSL_LOG(DFATAL) << "eglDestroySyncKHR failed: " << GetEglError();
  }
  sync_ = EGL_NO_SYNC_KHR;
}

absl::Status EglSync::WaitOnGpu() {
  ABSL_RETURN_IF_ERROR(CheckEglSyncSupported(display_));

  const EGLint result = eglWaitSyncKHR(display_, sync_, 0);
  RET_CHECK_EQ(result, EGL_TRUE) << "eglWaitSyncKHR failed: " << GetEglError();
  return absl::OkStatus();
}

absl::Status EglSync::Wait() {
  ABSL_RETURN_IF_ERROR(CheckEglSyncSupported(display_));

  const EGLint result = eglClientWaitSyncKHR(
      display_, sync_, EGL_SYNC_FLUSH_COMMANDS_BIT_KHR, EGL_FOREVER_KHR);
  RET_CHECK_EQ(result, EGL_CONDITION_SATISFIED_KHR)
      << "eglClientWaitSyncKHR failed: " << GetEglError();
  return absl::OkStatus();
}

absl::StatusOr<bool> EglSync::IsSignaled() {
  EGLint status;
  const EGLBoolean success =
      eglGetSyncAttribKHR(display_, sync_, EGL_SYNC_STATUS_KHR, &status);
  RET_CHECK_EQ(success, EGL_TRUE)
      << "eglGetSyncAttribKHR failed: " << GetEglError();
  return status == EGL_SIGNALED_KHR;
}

}  // namespace mediapipe
