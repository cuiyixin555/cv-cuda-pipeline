// Copyright 2020 The MediaPipe Authors.
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

#ifndef MEDIAPIPE_FRAMEWORK_FORMATS_TENSOR_H_
#define MEDIAPIPE_FRAMEWORK_FORMATS_TENSOR_H_

#include <algorithm>
#include <cstdint>
#include <initializer_list>
#include <memory>
#include <numeric>
#include <ostream>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include "absl/base/thread_annotations.h"
#include "absl/functional/any_invocable.h"
#include "absl/status/status.h"
#include "absl/strings/string_view.h"
#include "absl/synchronization/mutex.h"
#include "cv-cuda-pipeline/framework/formats/tensor/internal.h"
#include "cv-cuda-pipeline/framework/memory_manager.h"
#include "cv-cuda-pipeline/framework/port.h"

#if MEDIAPIPE_OPENGL_ES_VERSION >= MEDIAPIPE_OPENGL_ES_30
#include "cv-cuda-pipeline/gpu/gl_base.h"
#include "cv-cuda-pipeline/gpu/gl_context.h"
#endif  // MEDIAPIPE_OPENGL_ES_VERSION >= MEDIAPIPE_OPENGL_ES_30

namespace mediapipe {
// Tensor is a container of multi-dimensional data that supports sharing the
// content across different backends and APIs: CPU and OpenGL.
// Texture2DView is limited to 4 dimensions.
// The content is accessible through requesting device specific views.
// Acquiring a view guarantees that the content is not changed by another thread
// until the view is released.
//
// For OpenGL the code below must be called by a thread with valid OpenGL ES
// context bound:
// auto view = tensor.GetOpenGlBufferWriteView();
// GLuint buffer = view.buffer();
// Then the buffer can be bound to the GPU command buffer.
// ...binding the buffer to the command buffer...
// ...committing command buffer and releasing the view...
//
// The following request for the CPU view will be blocked until the GPU view is
// released and the GPU task is finished.
//
// auto view = tensor.GetCpuReadView();
// float* pointer = view.buffer<float>();
// ...reading the cpu memory...

class Tensor {
  class View {
   public:
    // Non-copyable.
    View(const View&) = delete;
    View& operator=(const View&) = delete;

   protected:
    explicit View(std::unique_ptr<absl::MutexLock>&& lock)
        : lock_(std::move(lock)) {}
    std::unique_ptr<absl::MutexLock> lock_;
  };

 public:
  // No resources are allocated here.
  enum class ElementType {
    kNone,
    kFloat16,
    kFloat32,
    kUInt8,
    kInt8,
    kInt32,
    kInt64,
    kChar,
    kBool
  };

  static absl::string_view ElementTypeName(ElementType element_type);

  struct Shape {
    Shape() = default;
    Shape(std::initializer_list<int> dimensions) : dims(dimensions) {}
    Shape(const std::vector<int>& dimensions) : dims(dimensions) {}
    Shape(std::initializer_list<int> dimensions, bool is_dynamic)
        : dims(dimensions), is_dynamic(is_dynamic) {}
    Shape(const std::vector<int>& dimensions, bool is_dynamic)
        : dims(dimensions), is_dynamic(is_dynamic) {}
    int num_elements() const {
      return std::accumulate(dims.begin(), dims.end(), 1,
                             std::multiplies<int>());
    }
    std::vector<int> dims;
    // The Tensor has dynamic rather than static shape so the TFLite interpreter
    // needs to be reallocated. Only relevant for CPU.
    bool is_dynamic = false;
  };
  // Quantization parameters corresponding to the zero_point and scale value
  // made available by TfLite quantized (uint8/int8) tensors.
  struct QuantizationParameters {
    QuantizationParameters() = default;
    QuantizationParameters(float scale, int zero_point)
        : scale(scale), zero_point(zero_point) {}
    float scale = 1.0f;
    int zero_point = 0;
  };

  // memory_alignment is an optional argument to tell the API to allocate
  // a buffer that is padded to multiples of memory_alignment bytes.
  // memory_alignment must be power of 2, i.e. 2, 4, 8, 16, 64, etc.
  // If memory_alignment is 0, then the buffer will not be padded.
  // Note that memory_alignment is only applied to CPU storage.
  Tensor(ElementType element_type, const Shape& shape,
         MemoryManager* memory_manager = nullptr, int memory_alignment = 0);
  Tensor(ElementType element_type, const Shape& shape,
         const QuantizationParameters& quantization_parameters,
         MemoryManager* memory_manager = nullptr, int memory_alignment = 0);

  // Non-copyable.
  Tensor(const Tensor&) = delete;
  Tensor& operator=(const Tensor&) = delete;
  // Move-only.
  Tensor(Tensor&& src);
  Tensor& operator=(Tensor&&);
  ~Tensor();

  // Returns a numpy-style string representation of the tensor. If the tensor
  // has more than max_num_elements elements, all dimensions larger than 8 are
  // shortened to x1 x2 x3 ... xn-2, xn-1, xn.
  // Example: Tensor<Float32> [2 2] =
  // [[-0.21845226  0.5312876 ]
  //  [-0.1596979   0.40760058]]
  std::string DebugString(int max_num_elements = 1024) const;

  friend std::ostream& operator<<(std::ostream& stream, const Tensor& tensor) {
    return stream << tensor.DebugString();
  }

  template <typename T>
  class CpuView : public View {
   public:
    template <typename P>
    auto buffer() const {
      // const and non-const return  type selection.
      return static_cast<typename std::tuple_element<
          std::is_const<T>::value, std::tuple<P*, const P*>>::type>(buffer_);
    }
    CpuView(CpuView&& src) : View(std::move(src.lock_)) {
      buffer_ = std::exchange(src.buffer_, nullptr);
      release_callback_ = std::exchange(src.release_callback_, nullptr);
    }
    ~CpuView() {
      if (release_callback_) release_callback_();
    }

   protected:
    friend class Tensor;
    CpuView(T* buffer, std::unique_ptr<absl::MutexLock>&& lock,
            absl::AnyInvocable<void()> release_callback = nullptr)
        : View(std::move(lock)),
          buffer_(buffer),
          release_callback_(std::move(release_callback)) {}
    T* buffer_;
    absl::AnyInvocable<void()> release_callback_;
  };
  using CpuReadView = CpuView<const void>;
  CpuReadView GetCpuReadView() const;
  using CpuWriteView = CpuView<void>;
  CpuWriteView GetCpuWriteView() const;

#if MEDIAPIPE_OPENGL_ES_VERSION >= MEDIAPIPE_OPENGL_ES_30
  // TODO: Use GlTextureView instead.
  // Only float32 textures are supported with 1/2/3/4 depths.
  // OpenGlTexture2dView currently only supports BHWC memory layout.
  class OpenGlTexture2dView : public View {
   public:
    GLuint name() const { return name_; }
    OpenGlTexture2dView(OpenGlTexture2dView&& src)
        : View(std::move(src.lock_)) {
      name_ = std::exchange(src.name_, GL_INVALID_INDEX);
    }
    // To fit a tensor into a texture two layouts are used:
    // 1. Aligned. Width of the texture = tensor_width * num_slices, where slice
    //    is a group of 4 depth values. Tensor depth is padded to 4.
    // 2. Linearized. If texture width or height with the layout 1. is greater
    //    than the GPU supports then all tensor values are packed into a texture
    //    with fixed width calculated by this method.
    // Must be called with the valid GL context bound to the current thread.
    enum class Layout { kAligned, kLinearized };
    static Layout GetLayoutDimensions(const Tensor::Shape& shape, int* width,
                                      int* height);

   protected:
    friend class Tensor;
    OpenGlTexture2dView(GLuint name, std::unique_ptr<absl::MutexLock>&& lock)
        : View(std::move(lock)), name_(name) {}
    GLuint name_;
  };
  // A valid OpenGL context must be bound to the calling thread due to possible
  // GPU resource allocation.
  OpenGlTexture2dView GetOpenGlTexture2dReadView() const;
  OpenGlTexture2dView GetOpenGlTexture2dWriteView() const;
#endif  // MEDIAPIPE_OPENGL_ES_VERSION >= MEDIAPIPE_OPENGL_ES_30

#if MEDIAPIPE_OPENGL_ES_VERSION >= MEDIAPIPE_OPENGL_ES_31
  class OpenGlBufferView : public View {
   public:
    GLuint name() const { return name_; }

    OpenGlBufferView(OpenGlBufferView&& src) : View(std::move(src.lock_)) {
      is_write_view_ = src.is_write_view_;
      name_ = std::exchange(src.name_, GL_INVALID_INDEX);
      gl_context_ = std::exchange(src.gl_context_, nullptr);
      gl_write_read_sync_ = std::exchange(src.gl_write_read_sync_, nullptr);
    }

    ~OpenGlBufferView() {
      if (is_write_view_) {
        if (gl_write_read_sync_ != nullptr && gl_context_ != nullptr) {
          // Ensure that all GPU memory modifications made by compute shaders or
          // rendering commands to this buffer are visible across all GL
          // pipeline resources and contexts before generating the
          // synchronization token.
          gl_context_->Run([] { glMemoryBarrier(GL_ALL_BARRIER_BITS); });
          *gl_write_read_sync_ = gl_context_->CreateSyncToken();
        }
      }
    }

   protected:
    friend class Tensor;

    // NOTE: Update move constructor if adding params.
    OpenGlBufferView(bool is_write_view, GLuint name,
                     std::unique_ptr<absl::MutexLock>&& lock,
                     GlContext* gl_context,
                     std::shared_ptr<GlSyncPoint>* gl_write_read_sync)
        : View(std::move(lock)),
          is_write_view_(is_write_view),
          name_(name),
          gl_context_(gl_context),
          gl_write_read_sync_(gl_write_read_sync) {
      if (!is_write_view) {
        MaybeWaitForWrites();
      }
    }

    void MaybeWaitForWrites() {
      if (gl_context_->IsCurrent()) {
        // Sync is not needed if the view is requested on the same context where
        // the write view was requested.
        return;
      }
      if (GlContext::IsAnyContextCurrent() && *gl_write_read_sync_ != nullptr) {
        // In case the read view is requested on a different context than the
        // one where the write view was requested, we need to wait for the
        // write sync point to be reached.
        (*gl_write_read_sync_)->WaitOnGpu();
        gl_write_read_sync_->reset();
      }
    }

    bool is_write_view_;
    GLuint name_;
    GlContext* gl_context_;
    std::shared_ptr<GlSyncPoint>* gl_write_read_sync_;
  };
  // A valid OpenGL context must be bound to the calling thread due to possible
  // GPU resource allocation.
  // Notes on (multi-context) GL synchronization:
  // 1. GetOpenGlBufferWriteView returns a view that creates a GlSync fence
  //    object during its destruction.
  // 2. If the read view is requested on the same context where the write view
  //    was requested, no GL fence synchronization is needed and the write
  //    fence object is ignored.
  // 3. If the read view is requested on a different context than the one where
  //    the write view was requested, GetOpenGLBufferReadView will wait (on GPU)
  //    for the sync point created during write view destruction.
  // 4. A memory barrier is needed when operating on GL buffers to ensure that
  //    the write operations are visible to subsequent read operations (even on
  //    the same context) - GL fence synchronization is not enough. GL buffer
  //    memory barriers are currentlyh NOT manged by the Tensor class and must
  //    be handled externally.
  OpenGlBufferView GetOpenGlBufferReadView() const;
  OpenGlBufferView GetOpenGlBufferWriteView() const;
#endif  // MEDIAPIPE_OPENGL_ES_VERSION >= MEDIAPIPE_OPENGL_ES_31

  const Shape& shape() const { return shape_; }
  ElementType element_type() const { return element_type_; }
  const QuantizationParameters& quantization_parameters() const {
    return quantization_parameters_;
  }
  int element_size() const {
    switch (element_type_) {
      case ElementType::kNone:
        return 0;
      case ElementType::kFloat16:
        return 2;
      case ElementType::kFloat32:
        return sizeof(float);
      case ElementType::kUInt8:
        return 1;
      case ElementType::kInt8:
        return 1;
      case ElementType::kInt32:
        return sizeof(int32_t);
      case ElementType::kInt64:
        return sizeof(int64_t);
      case ElementType::kChar:
        return sizeof(char);
      case ElementType::kBool:
        return sizeof(bool);
    }
  }
  int bytes() const { return shape_.num_elements() * element_size(); }

  bool ready_on_cpu() const { return valid_ & kValidCpu; }
  bool ready_on_gpu() const {
    return valid_ & (kValidOpenGlBuffer | kValidOpenGlTexture2d);
  }
  bool ready_as_opengl_buffer() const { return valid_ & kValidOpenGlBuffer; }
  bool ready_as_opengl_texture_2d() const {
    return valid_ & kValidOpenGlTexture2d;
  }

 private:
  // RAII object to own CPU buffer and release callback. It is intended to be
  // used internally to access tensor CPU data when lock is already in place.
  template <typename T>
  class CpuBufferHandle {
   public:
    explicit CpuBufferHandle(T* buffer,
                             absl::AnyInvocable<void()> release_callback)
        : buffer_(buffer), release_callback_(std::move(release_callback)) {}
    CpuBufferHandle(const CpuBufferHandle&) = delete;
    CpuBufferHandle& operator=(const CpuBufferHandle&) = delete;
    CpuBufferHandle(CpuBufferHandle&&) = delete;
    CpuBufferHandle& operator=(CpuBufferHandle&&) = delete;

    ~CpuBufferHandle() {
      if (release_callback_) release_callback_();
    }
    template <typename P>
    auto buffer() const {
      // const and non-const return type selection.
      return static_cast<typename std::tuple_element<
          std::is_const<T>::value, std::tuple<P*, const P*>>::type>(buffer_);
    }
    CpuView<T> ToCpuView(std::unique_ptr<absl::MutexLock> lock) && {
      return CpuView<T>(buffer_, std::move(lock), std::move(release_callback_));
    }

   private:
    T* buffer_;
    absl::AnyInvocable<void()> release_callback_;
  };

  void Move(Tensor*);
  absl::Status Invalidate();
  absl::Status ReadBackGpuToCpu() const;
  // Returns a CPU buffer handle of the tensor. view_mutex_ must be held during
  // the function call and the lifetime of the returned handle.
  CpuBufferHandle<const void> AcquireCpuBufferHandle() const;

  ElementType element_type_;
  Shape shape_;
  QuantizationParameters quantization_parameters_;
  int memory_alignment_ = 0;

  // The flags describe the current source of truth resource type.
  enum {
    kValidNone = 0,
    kValidCpu = 1 << 0,
    kValidOpenGlBuffer = 1 << 2,
    kValidOpenGlTexture2d = 1 << 3,
  };
  // A list of resource which are currently allocated and synchronized between
  // each-other: valid_ = kValidCpu | kValidOpenGlBuffer;
  mutable int valid_ = 0;
  // The mutex is locked by Get*View and is kept by all Views.
  mutable absl::Mutex view_mutex_;

  mutable void* cpu_buffer_ = nullptr;
  absl::Status AllocateCpuBuffer() const;
  void FreeCpuBuffer() const;

#if MEDIAPIPE_OPENGL_ES_VERSION >= MEDIAPIPE_OPENGL_ES_30
  mutable std::shared_ptr<mediapipe::GlContext> gl_context_;
  mutable GLuint opengl_texture2d_ = GL_INVALID_INDEX;
  mutable GLuint frame_buffer_ = GL_INVALID_INDEX;
  mutable int texture_width_;
  mutable int texture_height_;

#ifdef __EMSCRIPTEN__
  mutable bool texture_is_half_float_ = false;
#endif  // __EMSCRIPTEN__

  void AllocateOpenGlTexture2d() const;

#if MEDIAPIPE_OPENGL_ES_VERSION >= MEDIAPIPE_OPENGL_ES_31
  mutable GLuint opengl_buffer_ = GL_INVALID_INDEX;
  void AllocateOpenGlBuffer() const;
  mutable std::shared_ptr<GlSyncPoint> gl_write_read_sync_;
#endif  // MEDIAPIPE_OPENGL_ES_VERSION >= MEDIAPIPE_OPENGL_ES_31

  bool NeedsHalfFloatRenderTarget() const;

#endif  // MEDIAPIPE_OPENGL_ES_VERSION >= MEDIAPIPE_OPENGL_ES_30
};

int BhwcBatchFromShape(const Tensor::Shape& shape);
int BhwcHeightFromShape(const Tensor::Shape& shape);
int BhwcWidthFromShape(const Tensor::Shape& shape);
int BhwcDepthFromShape(const Tensor::Shape& shape);

template <typename Sink>
void AbslStringify(Sink& sink, Tensor::ElementType e) {
  sink.Append(Tensor::ElementTypeName(e));
}
}  // namespace mediapipe

#endif  // MEDIAPIPE_FRAMEWORK_FORMATS_TENSOR_H_
