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

// This header includes platform-specific headers for OpenGL.

#ifndef MEDIAPIPE_GPU_GL_BASE_H_
#define MEDIAPIPE_GPU_GL_BASE_H_

#if defined(__EMSCRIPTEN__)
#include <emscripten/html5.h>
#endif  // defined(__EMSCRIPTEN__)


#define HAS_EGL 1

#include <EGL/egl.h>
// TODO: b/377324183 - add <EGL/eglext.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <GLES3/gl32.h>

// When using the Linux EGL headers, we may end up pulling a
// "#define Status int" from Xlib.h, which interferes with absl::Status.
#undef Status

// More crud from X
#undef None
#undef Bool
#undef Success


namespace mediapipe {

// Doing this as an inline function allows us to avoid unwanted "pointer will
// never be null" errors on certain platforms and compilers.
template <typename T>
inline bool SymbolAvailable(T* symbol) {
  return symbol != nullptr;
}

}  // namespace mediapipe

#endif  // MEDIAPIPE_GPU_GL_BASE_H_
