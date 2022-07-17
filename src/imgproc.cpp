#include "imgproc.h"

namespace pf
{

cv::Mat blend(const cv::Mat& in, const cv::Mat& alpha, cv::Scalar bgColor)
{
    CV_Assert(in.size() == alpha.size());
    CV_Assert(alpha.type() == CV_32F);
    cv::Mat blendImg = cv::Mat::zeros(in.size(), CV_8UC3);

    for (int i = 0; i < in.rows; i++) {
        float* alpha_row = (float*)alpha.ptr(i);
        for (int j = 0; j < in.cols; j++) {
            float alpha_val = alpha_row[j];
            blendImg.at<cv::Vec3b>(i, j)[0] = in.at<cv::Vec3b>(i, j)[0] * alpha_val + (1 - alpha_val) * bgColor[2];
            blendImg.at<cv::Vec3b>(i, j)[1] = in.at<cv::Vec3b>(i, j)[1] * alpha_val + (1 - alpha_val) * bgColor[1];
            blendImg.at<cv::Vec3b>(i, j)[2] = in.at<cv::Vec3b>(i, j)[2] * alpha_val + (1 - alpha_val) * bgColor[0];
        }
    }

    return blendImg;
}

}
