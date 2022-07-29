#include "ncnn_engine.h"
#include "net.h"
#include <vulkan/vulkan.h>
#include "utils/logger.h"

namespace pf {

NcnnEngine& NcnnEngine::instance()
{
    static NcnnEngine engine_;
    return engine_;
}

int NcnnEngine::init()
{
    ncnn::create_gpu_instance();
    return 0;
}

int NcnnEngine::uninit()
{
    PPMattingNCNN::instance()->uninit();
    RealESRGAN::instance()->uninit();

    ncnn::destroy_gpu_instance();
    return 0;
}

}
