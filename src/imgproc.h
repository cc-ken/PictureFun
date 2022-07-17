#pragma once

#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>

namespace pf
{
    cv::Mat blend(const cv::Mat& in, const cv::Mat& alpha, cv::Scalar bgColor);
}
