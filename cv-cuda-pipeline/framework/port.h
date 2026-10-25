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
//
// Defines symbols and methods related to portable portions of the framework.
//

#ifndef MEDIAPIPE_FRAMEWORK_PORT_H_
#define MEDIAPIPE_FRAMEWORK_PORT_H_

// These platforms do not support OpenGL ES Compute Shaders (v3.1 and up),
// but may or may not still be able to run other OpenGL code.
#if !defined(MEDIAPIPE_DISABLE_GL_COMPUTE) && \
    (MEDIAPIPE_DISABLE_GPU || MEDIAPIPE_USING_LEGACY_SWIFTSHADER)
#define MEDIAPIPE_DISABLE_GL_COMPUTE
#endif

// Compile time target platform definitions.
// Example: #if MEDIAPIPE_OPENGL_ES_VERSION >= MEDIAPIPE_OPENGL_ES_31
#define MEDIAPIPE_OPENGL_ES_20 200
#define MEDIAPIPE_OPENGL_ES_30 300
#define MEDIAPIPE_OPENGL_ES_31 310

// NOTE: MEDIAPIPE_OPENGL_ES_VERSION macro represents the maximum OpenGL ES
// version to build for. Runtime availability is _not_ guaranteed; in
// particular, uses of OpenGL ES 3.1 should be guarded by a runtime check.
// TODO: identify and fix code where macro is used incorrectly.
#if MEDIAPIPE_DISABLE_GPU
#define MEDIAPIPE_OPENGL_ES_VERSION 0
#else
#define MEDIAPIPE_OPENGL_ES_VERSION MEDIAPIPE_OPENGL_ES_31
#endif

#ifndef MEDIAPIPE_HAS_RTTI
// Detect if RTTI is disabled in the compiler.
#if defined(__clang__) && defined(__has_feature)
#define MEDIAPIPE_HAS_RTTI __has_feature(cxx_rtti)
#elif defined(__GNUC__) && !defined(__GXX_RTTI)
#define MEDIAPIPE_HAS_RTTI 0
#elif defined(_MSC_VER) && !defined(_CPPRTTI)
#define MEDIAPIPE_HAS_RTTI 0
#else
#define MEDIAPIPE_HAS_RTTI 1
#endif
#endif  // MEDIAPIPE_HAS_RTTI

#endif  // MEDIAPIPE_FRAMEWORK_PORT_H_
