// MIT License
//
// Copyright (c) 2026 CUI Xin (崔 欣)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "cv-cuda-pipeline/calculators/image_process/cuda/random_resized_crop.h"

#include <algorithm>
#include <cmath>
#include <random>

namespace cv_cuda_pipeline {
bool SelectRandomResizedCrop(int image_width, int image_height, float min_scale,
                             float max_scale, float min_ratio, float max_ratio,
                             std::uint64_t seed, CropRectangle* crop) {
  if (!crop || image_width <= 0 || image_height <= 0 ||
      !std::isfinite(min_scale) || !std::isfinite(max_scale) ||
      !std::isfinite(min_ratio) || !std::isfinite(max_ratio) ||
      min_scale <= 0.0f || max_scale < min_scale || max_scale > 1.0f ||
      min_ratio <= 0.0f || max_ratio < min_ratio)
    return false;

  std::mt19937_64 generator(seed);
  std::uniform_real_distribution<double> scale_distribution(min_scale,
                                                            max_scale);
  std::uniform_real_distribution<double> ratio_distribution(
      std::log(static_cast<double>(min_ratio)),
      std::log(static_cast<double>(max_ratio)));
  const double area = static_cast<double>(image_width) * image_height;
  for (int attempt = 0; attempt < 10; ++attempt) {
    const double target_area = area * scale_distribution(generator);
    const double aspect_ratio = std::exp(ratio_distribution(generator));
    const double sampled_width =
        std::round(std::sqrt(target_area * aspect_ratio));
    const double sampled_height =
        std::round(std::sqrt(target_area / aspect_ratio));
    if (sampled_width > 0.0 && sampled_width <= image_width &&
        sampled_height > 0.0 && sampled_height <= image_height) {
      const int width = static_cast<int>(sampled_width);
      const int height = static_cast<int>(sampled_height);
      std::uniform_int_distribution<int> x_distribution(0, image_width - width);
      std::uniform_int_distribution<int> y_distribution(0,
                                                        image_height - height);
      *crop = {x_distribution(generator), y_distribution(generator), width,
               height};
      return true;
    }
  }

  const double input_ratio = static_cast<double>(image_width) / image_height;
  int width = image_width, height = image_height;
  if (input_ratio < min_ratio) {
    height = std::clamp(static_cast<int>(std::round(
                            image_width / static_cast<double>(min_ratio))),
                        1, image_height);
  } else if (input_ratio > max_ratio) {
    width = std::clamp(static_cast<int>(std::round(
                           image_height * static_cast<double>(max_ratio))),
                       1, image_width);
  }
  *crop = {(image_width - width) / 2, (image_height - height) / 2, width,
           height};
  return true;
}
}  // namespace cv_cuda_pipeline
