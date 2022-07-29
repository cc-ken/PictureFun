// realesrgan implemented with ncnn library

#ifndef REALESRGAN_H
#define REALESRGAN_H
#include <string>
#include <opencv2/core/core.hpp>


namespace pf
{

class RealESRGAN
{
public:
    virtual ~RealESRGAN() = default;

    static RealESRGAN* instance();
    
    virtual cv::Mat inference(const cv::Mat& input) = 0;
    
    virtual int init(int scale = 4) = 0;
    
    virtual void uninit() = 0;

protected:
    RealESRGAN() = default;
};

}

#endif // REALESRGAN_H
