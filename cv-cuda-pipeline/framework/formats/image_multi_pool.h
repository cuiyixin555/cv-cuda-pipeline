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

// This class lets calculators allocate GpuBuffers of various sizes, caching
// and reusing them as needed. It does so by automatically creating and using
// platform-specific buffer pools for the requested sizes.
//
// This class is not meant to be used directly by calculators, but is instead
// used by GlCalculatorHelper to allocate buffers.

#ifndef MEDIAPIPE_FRAMEWORK_FORMATS_IMAGE_MULTI_POOL_H_
#define MEDIAPIPE_FRAMEWORK_FORMATS_IMAGE_MULTI_POOL_H_

#include <deque>
#include <limits>
#include <unordered_map>

#include "absl/synchronization/mutex.h"
#include "cv-cuda-pipeline/framework/formats/image.h"
#include "cv-cuda-pipeline/framework/formats/image_frame_pool.h"

#if !MEDIAPIPE_DISABLE_GPU
#include "cv-cuda-pipeline/gpu/gpu_buffer.h"


#include "cv-cuda-pipeline/gpu/gl_texture_buffer_pool.h"
#endif  // !MEDIAPIPE_DISABLE_GPU

namespace mediapipe {

using ImageFrameSharedPtr = std::shared_ptr<ImageFrame>;

// TODO: Update to use new pool eviction policy.
class ImageMultiPool {
 public:
  ImageMultiPool() {}
  explicit ImageMultiPool(void* ignored) {}
  ~ImageMultiPool();

  // Obtains a buffer. May either be reused or created anew.
  Image GetBuffer(int width, int height, bool use_gpu,
                  ImageFormat::Format format /*= ImageFormat::SRGBA*/);


  static std::size_t RotateLeftN(std::size_t x, int n) {
    return (x << n) | (x >> (std::numeric_limits<size_t>::digits - n));
  }

  struct IBufferSpec {
    IBufferSpec(int w, int h, mediapipe::ImageFormat::Format f)
        : width(w), height(h), format(f) {}
    int width;
    int height;
    mediapipe::ImageFormat::Format format;
    // Note: alignment should be added here if ImageFrameBufferPool is changed
    // to allow for customizable alignment sizes (currently fixed at 4 for best
    // compatability with OpenGL).
  };

  struct IBufferSpecHash {
    std::size_t operator()(const IBufferSpec& spec) const {
      // Width and height are expected to be smaller than half the width of
      // size_t. We can combine them into a single integer using std::hash.
      constexpr int kWidth = std::numeric_limits<size_t>::digits;
      return std::hash<std::size_t>{}(
          spec.width ^ RotateLeftN(spec.height, kWidth / 2) ^
          RotateLeftN(static_cast<uint32_t>(spec.format), kWidth / 4));
      // Note: alignment should be added here if ImageFrameBufferPool is changed
      // to allow for customizable alignment sizes (currently fixed at 4 for
      // best compatability with OpenGL).
    }
  };

 private:
#if !MEDIAPIPE_DISABLE_GPU
  typedef std::shared_ptr<mediapipe::GlTextureBufferPool> SimplePoolGpu;
  SimplePoolGpu MakeSimplePoolGpu(IBufferSpec spec);
  Image GetBufferFromSimplePool(IBufferSpec spec, const SimplePoolGpu& pool);

  absl::Mutex mutex_gpu_;
  std::unordered_map<IBufferSpec, SimplePoolGpu, IBufferSpecHash> pools_gpu_
      ABSL_GUARDED_BY(mutex_gpu_);
  // A queue of IBufferSpecs to keep track of the age of each IBufferSpec added
  // to the pool.
  std::deque<IBufferSpec> buffer_specs_gpu_;
#endif  // !MEDIAPIPE_DISABLE_GPU

  typedef std::shared_ptr<ImageFramePool> SimplePoolCpu;
  SimplePoolCpu MakeSimplePoolCpu(IBufferSpec spec);
  Image GetBufferFromSimplePool(IBufferSpec spec, const SimplePoolCpu& pool);

  absl::Mutex mutex_cpu_;
  std::unordered_map<IBufferSpec, SimplePoolCpu, IBufferSpecHash> pools_cpu_
      ABSL_GUARDED_BY(mutex_cpu_);
  // A queue of IBufferSpecs to keep track of the age of each IBufferSpec added
  // to the pool.
  std::deque<IBufferSpec> buffer_specs_cpu_;

};

// IBufferSpec equality operators
inline bool operator==(const ImageMultiPool::IBufferSpec& lhs,
                       const ImageMultiPool::IBufferSpec& rhs) {
  return lhs.width == rhs.width && lhs.height == rhs.height &&
         lhs.format == rhs.format;
}
inline bool operator!=(const ImageMultiPool::IBufferSpec& lhs,
                       const ImageMultiPool::IBufferSpec& rhs) {
  return !operator==(lhs, rhs);
}

}  // namespace mediapipe

#endif  // MEDIAPIPE_FRAMEWORK_FORMATS_IMAGE_MULTI_POOL_H_
