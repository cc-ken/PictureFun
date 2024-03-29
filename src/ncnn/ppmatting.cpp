#include "ppmatting.h"
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
// ncnn
#include "net.h"
#include "benchmark.h"

#include "utils/system.h"
#include "utils/logger.h"
#include "ncnn_engine.h"

namespace {

static ncnn::UnlockedPoolAllocator g_blob_pool_allocator;
static ncnn::PoolAllocator g_workspace_pool_allocator;

}

namespace pf
{

class PPMattingNCNNImpl : public PPMattingNCNN
{
protected:
    std::unique_ptr<ncnn::Net> bgnet_;
    bool useGPU_ = true;
    std::mutex mutex_;
    bool inited_ = false;

public:
    PPMattingNCNNImpl()
    {
        useGPU_ = true;
    }
    
    int init() override {
        std::lock_guard<std::mutex> theLock(mutex_);
        if (inited_)
            return 1;
        
        ncnn::Option opt;
        opt.lightmode = true;
        //opt.num_threads = 4;
        opt.blob_allocator = &g_blob_pool_allocator;
        opt.workspace_allocator = &g_workspace_pool_allocator;

        // use vulkan compute
        if (!NcnnEngine::useCPU())
            opt.use_vulkan_compute = true;

        std::string resFolder = getCurrFilePath() + "/Resources/models/";
        LOG_INFO("load models from resource folder:" << resFolder);
        
        bgnet_.reset(new ncnn::Net());
        bgnet_->opt = opt;

        int ret0 = bgnet_->load_param((resFolder + "hrnet-w18.param").c_str());
        int ret1 = bgnet_->load_model((resFolder + "hrnet-w18.bin").c_str());
        LOG_INFO("load ppmatting, ret0=" << ret0 << ", ret1=" << ret1);
        if (ret0 != 0 || ret1 != 0)
            return -1;

        inited_ = true;
        return 0;
    }
    
    void uninit() override {
        std::lock_guard<std::mutex> theLock(mutex_);
        bgnet_ = nullptr;
        inited_ = false;
    }
    
    cv::Mat inference(const cv::Mat& input) override
    {
        init();
        
        const int workingWidth = 512;
        const int workingHeight = 512;
        const int width = input.cols;
        const int height = input.rows;
        ncnn::Mat in_resize = ncnn::Mat::from_pixels_resize(input.data, ncnn::Mat::PIXEL_BGR2RGB,
                                    input.cols, input.rows, workingWidth, workingHeight);

        const float meanVals[3] = { 127.5f, 127.5f,  127.5f };
        const float normVals[3] = { 0.0078431f, 0.0078431f, 0.0078431f };
        in_resize.substract_mean_normalize(meanVals, normVals);
        ncnn::Mat out;
        {
            ncnn::Extractor ex = bgnet_->create_extractor();
            ex.set_vulkan_compute(useGPU_);
            ex.input("input", in_resize);
            ex.extract("output", out);
        }

        ncnn::Mat alpha;
        ncnn::resize_bilinear(out, alpha, width, height);
        
        cv::Mat result(cv::Size(width, height), CV_32FC1, alpha.data);
        
        return result.clone();
    }
};

PPMattingNCNN* PPMattingNCNN::instance() {
    static PPMattingNCNNImpl inst;
    
    return &inst;
}

}
