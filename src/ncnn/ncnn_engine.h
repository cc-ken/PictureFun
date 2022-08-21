#pragma once

#include "ppmatting.h"
#include "realesrgan.h"

namespace pf {

class NcnnEngine {
public:
    ~NcnnEngine() = default;

    static NcnnEngine& instance();

    int init();

    int uninit();
    
    static bool useCPU();
    
protected:
    NcnnEngine() = default;
};

}
