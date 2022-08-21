#include "ncnn_engine.h"
#include "net.h"
#include <vulkan/vulkan.h>
#include "utils/logger.h"
#include "utils/preference.h"

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

bool NcnnEngine::useCPU() {
    auto comp = preference::i().get("comp", "GPU");
    return (comp != "GPU") || (ncnn::get_gpu_count() == 0);
}

}
