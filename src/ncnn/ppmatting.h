#pragma once

#include <memory>
#include <opencv2/core/core.hpp>
#include <mutex>

namespace pf {

class PPMattingNCNN {
public:
    PPMattingNCNN() = default;
    virtual ~PPMattingNCNN() = default;

public:
    static PPMattingNCNN* instance();
    
    virtual cv::Mat inference(const cv::Mat& input) = 0;
    
    virtual int init() = 0;
    
    virtual void uninit() = 0;
};

}
