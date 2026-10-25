#include "cv-cuda-pipeline/examples/desktop/cuda_image_ops/filter_ops/filter_kernels.h"

#include "cv-cuda-pipeline/framework/port/opencv_core_inc.h"
#include "cv-cuda-pipeline/framework/port/opencv_imgproc_inc.h"

#include <cuda_runtime.h>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

namespace {

bool CheckCuda(cudaError_t status, const char* action) {
  if (status == cudaSuccess) return true;
  std::cerr << action << " failed: " << cudaGetErrorString(status) << '\n';
  return false;
}

bool Run(const std::string& operation, const cv::Mat& input, cv::Mat* output) {
  *output = cv::Mat(input.size(), CV_8UC3);
  const std::size_t bytes = input.total() * input.elemSize();
  std::uint8_t* device_input = nullptr;
  std::uint8_t* device_output = nullptr;
  if (!CheckCuda(cudaMalloc(reinterpret_cast<void**>(&device_input), bytes),
                 "Allocate test input") ||
      !CheckCuda(cudaMalloc(reinterpret_cast<void**>(&device_output), bytes),
                 "Allocate test output")) {
    if (device_output) cudaFree(device_output);
    if (device_input) cudaFree(device_input);
    return false;
  }
  bool ok = CheckCuda(cudaMemcpy(device_input, input.data, bytes,
                                 cudaMemcpyHostToDevice),
                      "Copy test input");
  if (ok) {
    ok = CheckCuda(RunFilterOperator(operation, device_input, device_output,
                                     input.cols, input.rows, 3),
                   operation.c_str());
  }
  if (ok) {
    ok = CheckCuda(cudaMemcpy(output->data, device_output, bytes,
                              cudaMemcpyDeviceToHost),
                   "Copy test output");
  }
  const cudaError_t free_output = cudaFree(device_output);
  const cudaError_t free_input = cudaFree(device_input);
  ok = CheckCuda(free_output, "Free test output") && ok;
  ok = CheckCuda(free_input, "Free test input") && ok;
  return ok;
}

cv::Mat Synthetic(int width = 40, int height = 32) {
  cv::Mat image(height, width, CV_8UC3);
  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      image.at<cv::Vec3b>(y, x) = cv::Vec3b(
          (x * 7 + y * 3) % 256, (x * 2 + y * 11 + 31) % 256,
          (x * 13 + y * 5 + 73) % 256);
    }
  }
  return image;
}

bool Close(const std::string& name, const cv::Mat& actual,
           const cv::Mat& expected, double tolerance = 2.0) {
  cv::Mat difference;
  cv::absdiff(actual, expected, difference);
  double maximum = 0;
  cv::Point location;
  cv::minMaxLoc(difference.reshape(1), nullptr, &maximum, nullptr, &location);
  if (maximum <= tolerance) return true;
  std::cerr << name << " differs from reference (max error " << maximum
            << ", tolerance " << tolerance << ", at " << location.x << ','
            << location.y << " actual="
            << static_cast<int>(actual.reshape(1).at<std::uint8_t>(location))
            << " expected="
            << static_cast<int>(expected.reshape(1).at<std::uint8_t>(location))
            << ")\n";
  return false;
}

bool TestReference(const std::string& name, const cv::Mat& input,
                   const cv::Mat& expected, double tolerance = 2.0) {
  cv::Mat actual;
  return Run(name, input, &actual) && Close(name, actual, expected, tolerance);
}

bool TestAdaptiveThreshold(const cv::Mat& input, const cv::Mat& expected) {
  cv::Mat actual;
  if (!Run("adaptivethreshold", input, &actual)) return false;
  cv::Mat difference;
  cv::absdiff(actual, expected, difference);
  cv::Mat mismatched;
  cv::threshold(difference.reshape(1), mismatched, 0, 255, cv::THRESH_BINARY);
  const int mismatch_count = cv::countNonZero(mismatched);
  const int allowed = static_cast<int>(input.total() * 3 * 0.04);
  if (mismatch_count <= allowed) return true;
  std::cerr << "adaptivethreshold differs from Gaussian-C reference at "
            << mismatch_count << " channel values (allowed " << allowed
            << ")\n";
  return false;
}

int ReflectCoordinate(int coordinate, int size) {
  if (size <= 1) return 0;
  while (coordinate < 0 || coordinate >= size) {
    coordinate = coordinate < 0 ? -coordinate - 1
                                : 2 * size - coordinate - 1;
  }
  return coordinate;
}

cv::Mat JointBilateralReference(const cv::Mat& image) {
  cv::Mat result(image.size(), image.type());
  constexpr float sigma_color = 75.0f;
  constexpr float sigma_space = 75.0f;
  for (int y = 0; y < image.rows; ++y) {
    for (int x = 0; x < image.cols; ++x) {
      const cv::Vec3b center = image.at<cv::Vec3b>(y, x);
      const float center_gray =
          0.114f * center[0] + 0.587f * center[1] + 0.299f * center[2];
      for (int c = 0; c < 3; ++c) {
        float weighted = 0.0f;
        float weights = 0.0f;
        for (int dy = -4; dy <= 4; ++dy) {
          for (int dx = -4; dx <= 4; ++dx) {
            const cv::Vec3b sample = image.at<cv::Vec3b>(
                ReflectCoordinate(y + dy, image.rows),
                ReflectCoordinate(x + dx, image.cols));
            const float guide =
                0.114f * sample[0] + 0.587f * sample[1] + 0.299f * sample[2];
            const float difference = guide - center_gray;
            const float weight =
                std::exp(-(dx * dx + dy * dy) /
                             (2.0f * sigma_space * sigma_space) -
                         3.0f * difference * difference /
                             (2.0f * sigma_color * sigma_color));
            weighted += weight * sample[c];
            weights += weight;
          }
        }
        result.at<cv::Vec3b>(y, x)[c] =
            cv::saturate_cast<std::uint8_t>(weighted / weights);
      }
    }
  }
  return result;
}

bool TestAll() {
  const cv::Mat input = Synthetic();
  bool passed = true;
  cv::Mat expected;

  cv::blur(input, expected, cv::Size(7, 7), cv::Point(-1, -1),
           cv::BORDER_REFLECT_101);
  passed = TestReference("averageblur", input, expected) && passed;

  cv::GaussianBlur(input, expected, cv::Size(9, 9), 1.5, 1.5,
                   cv::BORDER_REFLECT);
  passed = TestReference("gaussian", input, expected, 3.0) && passed;

  cv::Mat bilateral;
  cv::bilateralFilter(input, bilateral, 9, 75.0, 75.0, cv::BORDER_REFLECT);
  passed = TestReference("bilateral_filter", input, bilateral, 12.0) && passed;
  passed = TestReference("joint_bilateral_filter", input,
                         JointBilateralReference(input), 2.0) &&
           passed;

  cv::Mat box_reference = input.clone();
  const int x0[] = {input.cols / 8, input.cols / 4, input.cols * 3 / 4};
  const int y0[] = {input.rows / 8, input.rows / 4, input.rows * 3 / 4};
  const int widths[] = {input.cols / 5, input.cols / 2, input.cols / 6};
  const int heights[] = {input.rows / 5, input.rows / 2, input.rows / 6};
  const int sizes[] = {21, 45, 11};
  for (int i = 0; i < 3; ++i) {
    cv::Mat blurred;
    cv::blur(input, blurred, cv::Size(sizes[i], sizes[i]), cv::Point(-1, -1),
             cv::BORDER_REFLECT);
    const cv::Rect roi(x0[i], y0[i], widths[i], heights[i]);
    if (roi.x >= 0 && roi.y >= 0 && roi.x + roi.width <= input.cols &&
        roi.y + roi.height <= input.rows) {
      blurred(roi).copyTo(box_reference(roi));
    }
  }
  passed = TestReference("boxblur", input, box_reference) && passed;

  cv::Mat gray;
  cv::cvtColor(input, gray, cv::COLOR_BGR2GRAY);
  cv::Mat adaptive;
  cv::adaptiveThreshold(gray, adaptive, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C,
                        cv::THRESH_BINARY, 11, 2.0);
  cv::Mat adaptive_bgr;
  cv::cvtColor(adaptive, adaptive_bgr, cv::COLOR_GRAY2BGR);
  passed = TestAdaptiveThreshold(input, adaptive_bgr) && passed;

  cv::Mat sharpened;
  const cv::Mat sharpen_kernel =
      (cv::Mat_<float>(3, 3) << 0, -1, 0, -1, 5, -1, 0, -1, 0);
  cv::filter2D(input, sharpened, CV_8U, sharpen_kernel, cv::Point(-1, -1), 0,
               cv::BORDER_REFLECT_101);
  passed = TestReference("conv2d", input, sharpened, 0.0) && passed;

  cv::Mat erased;
  passed = Run("erase", input, &erased) && Close("erase", erased, input, 0.0) &&
           passed;
  const cv::Mat erase_input = Synthetic(600, 520);
  cv::Mat erase_output;
  passed = Run("erase", erase_input, &erase_output) && passed;
  const cv::Vec3b red = erase_output.at<cv::Vec3b>(90, 60);
  const cv::Vec3b green_tint = erase_output.at<cv::Vec3b>(420, 220);
  if (red != cv::Vec3b(0, 0, 255) ||
      green_tint[0] != erase_input.at<cv::Vec3b>(420, 220)[0] ||
      green_tint[1] != 255 ||
      green_tint[2] != erase_input.at<cv::Vec3b>(420, 220)[2]) {
    std::cerr << "erase did not apply sample rectangles and channel mask\n";
    passed = false;
  }

  cv::Mat noise_first;
  cv::Mat noise_second;
  passed = Run("gaussiannoise", input, &noise_first) &&
           Run("gaussiannoise", input, &noise_second) &&
           Close("gaussiannoise deterministic seed", noise_first, noise_second,
                 0.0) &&
           passed;
  if (cv::countNonZero(noise_first.reshape(1) != input.reshape(1)) == 0) {
    std::cerr << "gaussiannoise did not modify the synthetic input\n";
    passed = false;
  }

  cv::Mat equalized;
  cv::equalizeHist(gray, equalized);
  cv::Mat equalized_bgr;
  cv::cvtColor(equalized, equalized_bgr, cv::COLOR_GRAY2BGR);
  passed = TestReference("histogrameq", input, equalized_bgr, 1.0) && passed;

  cv::Mat median;
  cv::medianBlur(input, median, 7);
  passed = TestReference("median_blur", input, median, 0.0) && passed;

  cv::Mat closed;
  const cv::Mat element =
      cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
  cv::morphologyEx(input, closed, cv::MORPH_CLOSE, element, cv::Point(-1, -1),
                   1, cv::BORDER_REPLICATE);
  passed = TestReference("morphology", input, closed, 0.0) && passed;

  cv::Mat laplacian_float;
  cv::Laplacian(input, laplacian_float, CV_32F, 3, 1.0, 0.0,
                cv::BORDER_REPLICATE);
  double low = 0;
  double high = 0;
  cv::minMaxLoc(laplacian_float.reshape(1), &low, &high);
  cv::Mat laplacian_expected(input.size(), CV_8UC3);
  for (int y = 0; y < input.rows; ++y) {
    for (int x = 0; x < input.cols; ++x) {
      for (int c = 0; c < 3; ++c) {
        const float value = laplacian_float.at<cv::Vec3f>(y, x)[c];
        laplacian_expected.at<cv::Vec3b>(y, x)[c] =
            high > low ? cv::saturate_cast<std::uint8_t>(
                             (value - low) * (255.0 / (high - low)))
                       : cv::saturate_cast<std::uint8_t>(value);
      }
    }
  }
  passed = TestReference("laplacian", input, laplacian_expected, 2.0) && passed;

  cv::Mat constant(input.size(), input.type(), cv::Scalar(127, 127, 127));
  cv::Mat clahe_actual;
  passed = Run("clahe", constant, &clahe_actual) && passed;
  if (cv::countNonZero(clahe_actual.reshape(1) !=
                       cv::Mat(constant.size(), constant.type(),
                               cv::Scalar(255, 255, 255))
                           .reshape(1)) != 0) {
    std::cerr << "clahe constant-image output was not fully equalized\n";
    passed = false;
  }

  cv::Mat normalized_expected(input.size(), CV_8UC3);
  const float mean[3] = {103.53f, 116.28f, 123.675f};
  const float deviation[3] = {57.375f, 57.12f, 58.395f};
  for (int y = 0; y < input.rows; ++y) {
    for (int x = 0; x < input.cols; ++x) {
      for (int c = 0; c < 3; ++c) {
        normalized_expected.at<cv::Vec3b>(y, x)[c] =
            cv::saturate_cast<std::uint8_t>(
                (input.at<cv::Vec3b>(y, x)[c] - mean[c]) / deviation[c] *
                    64.0f +
                128.0f);
      }
    }
  }
  passed =
      TestReference("normalize", input, normalized_expected, 0.0) && passed;
  return passed;
}

}  // namespace

int main() {
  int device_count = 0;
  if (!CheckCuda(cudaGetDeviceCount(&device_count), "Detect CUDA device") ||
      device_count == 0) {
    std::cerr << "No CUDA device is available for operator tests\n";
    return 1;
  }
  if (!TestAll()) return 1;
  std::cout << "All 15 filter operator CUDA checks passed\n";
  return 0;
}
