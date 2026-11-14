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

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <exception>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "cv-cuda-pipeline/framework/port/opencv_core_inc.h"
#include "cv-cuda-pipeline/framework/port/opencv_imgcodecs_inc.h"
#include "cv-cuda-pipeline/framework/port/opencv_imgproc_inc.h"

namespace {
struct Options {
  std::string input;
  std::string output = "cat_osd.jpg";
};
Options Parse(int argc, char** argv) {
  Options options;
  for (int i = 1; i < argc; ++i) {
    const std::string arg = argv[i];
    if (arg == "--help" || arg == "-h") {
      std::cout << "Usage: " << argv[0] << " --input IMAGE [--output IMAGE]\n";
      std::exit(0);
    }
    if (arg != "--input" && arg != "--output")
      throw std::invalid_argument("Unknown option: " + arg);
    if (++i == argc) throw std::invalid_argument("Missing value for " + arg);
    if (arg == "--input")
      options.input = argv[i];
    else
      options.output = argv[i];
  }
  if (options.input.empty() || options.output.empty())
    throw std::invalid_argument("--input and --output must not be empty");
  return options;
}
void Blend(cv::Mat& image, const cv::Mat& overlay, double alpha) {
  cv::addWeighted(overlay, alpha, image, 1.0 - alpha, 0.0, image);
}
void Run(const Options& options) {
  const cv::Mat input = cv::imread(options.input, cv::IMREAD_COLOR);
  if (input.empty())
    throw std::runtime_error("Cannot read input: " + options.input);
  cv::Mat output = input.clone();
  const int width = input.cols, height = input.rows;
  const int box_x = width / 8, box_y = height / 8;
  const int box_width = std::max(1, width / 4);
  const int box_height = std::max(1, height / 4);
  const auto start = std::chrono::steady_clock::now();

  cv::Mat overlay = output.clone();
  cv::rectangle(overlay, cv::Rect(box_x, box_y, box_width, box_height),
                cv::Scalar(255, 128, 0), cv::FILLED);
  Blend(output, overlay, 64.0 / 255.0);
  cv::rectangle(output, cv::Rect(box_x, box_y, box_width, box_height),
                cv::Scalar(0, 255, 255), 3);

  const std::string label = "CV-CUDA OSD";
  constexpr double kFontScale = 0.65;
  constexpr int kFont = cv::FONT_HERSHEY_SIMPLEX;
  int baseline = 0;
  const cv::Size text_size =
      cv::getTextSize(label, kFont, kFontScale, 1, &baseline);
  const int text_x = box_x;
  const int text_y = box_y >= text_size.height + 8
                         ? box_y - 6
                         : std::min(height - baseline - 1,
                                    box_y + box_height + text_size.height + 6);
  cv::rectangle(output,
                cv::Rect(text_x, text_y - text_size.height - 4,
                         text_size.width + 8, text_size.height + baseline + 8),
                cv::Scalar(0, 0, 0), cv::FILLED);
  cv::putText(output, label, cv::Point(text_x + 4, text_y), kFont, kFontScale,
              cv::Scalar(255, 255, 255), 1, cv::LINE_AA);

  cv::line(output, cv::Point(0, 0), cv::Point(width - 1, height - 1),
           cv::Scalar(0, 255, 0), 2, cv::LINE_AA);

  overlay = output.clone();
  const int radius = std::max(1, std::min(width, height) / 8);
  cv::circle(overlay, cv::Point(width / 2, height / 2), radius,
             cv::Scalar(0, 128, 255), cv::FILLED, cv::LINE_AA);
  Blend(output, overlay, 48.0 / 255.0);
  cv::circle(output, cv::Point(width / 2, height / 2), radius,
             cv::Scalar(0, 128, 255), 2, cv::LINE_AA);

  cv::arrowedLine(output, cv::Point(width - 1, height / 2),
                  cv::Point(width * 3 / 4, height / 2), cv::Scalar(255, 0, 128),
                  2, cv::LINE_AA, 0, 0.25);

  overlay = output.clone();
  std::vector<cv::Point> diamond = {{width / 2, height / 4},
                                    {width * 3 / 4, height / 2},
                                    {width / 2, height * 3 / 4},
                                    {width / 4, height / 2}};
  cv::fillPoly(overlay, std::vector<std::vector<cv::Point>>{diamond},
               cv::Scalar(0, 255, 255), cv::LINE_AA);
  Blend(output, overlay, 32.0 / 255.0);
  cv::polylines(output, diamond, true, cv::Scalar(0, 255, 255), 2, cv::LINE_AA);

  const std::time_t epoch = 0;
  const std::tm* utc = std::gmtime(&epoch);
  if (utc == nullptr) throw std::runtime_error("Cannot format fixed timestamp");
  std::ostringstream clock_text;
  clock_text << std::put_time(utc, "%y%m%d_%H%M%S");
  cv::putText(output, clock_text.str(),
              cv::Point(10, height > 40 ? height - 10 : 10), kFont, 0.45,
              cv::Scalar(0, 255, 255), 1, cv::LINE_AA);

  const auto stop = std::chrono::steady_clock::now();
  if (!cv::imwrite(options.output, output))
    throw std::runtime_error("Cannot write output: " + options.output);
  const auto elapsed =
      std::chrono::duration<double, std::milli>(stop - start).count();
  std::cout << "Saved OpenCV OSD overlay to " << options.output
            << "; draw time " << elapsed << " ms\n";
}
}  // namespace

int main(int argc, char** argv) {
  try {
    Run(Parse(argc, argv));
    return 0;
  } catch (const std::exception& e) {
    std::cerr << "osd_demo: " << e.what() << '\n';
    return 1;
  }
}
