// realesrgan implemented with ncnn library

#include "realesrgan.h"

#include "utils/system.h"
#include "utils/logger.h"

// ncnn
#include "net.h"
#include "gpu.h"
#include "layer.h"

#include <algorithm>
#include <vector>
#include <opencv2/imgproc.hpp>
#include "ncnn_engine.h"

namespace pf
{

static const uint32_t realesrgan_preproc_spv_data[] = {   
    #include "realesrgan_preproc.spv.hex.h"
};
static const uint32_t realesrgan_preproc_fp16s_spv_data[] = {
    #include "realesrgan_preproc_fp16s.spv.hex.h"
};
static const uint32_t realesrgan_preproc_int8s_spv_data[] = {
    #include "realesrgan_preproc_int8s.spv.hex.h"
};
static const uint32_t realesrgan_postproc_spv_data[] = {
    #include "realesrgan_postproc.spv.hex.h"
};
static const uint32_t realesrgan_postproc_fp16s_spv_data[] = {
    #include "realesrgan_postproc_fp16s.spv.hex.h"
};
static const uint32_t realesrgan_postproc_int8s_spv_data[] = {
    #include "realesrgan_postproc_int8s.spv.hex.h"
};

static const uint32_t realesrgan_preproc_tta_spv_data[] = {
    #include "realesrgan_preproc_tta.spv.hex.h"
};
static const uint32_t realesrgan_preproc_tta_fp16s_spv_data[] = {
    #include "realesrgan_preproc_tta_fp16s.spv.hex.h"
};
static const uint32_t realesrgan_preproc_tta_int8s_spv_data[] = {
    #include "realesrgan_preproc_tta_int8s.spv.hex.h"
};
static const uint32_t realesrgan_postproc_tta_spv_data[] = {
    #include "realesrgan_postproc_tta.spv.hex.h"
};
static const uint32_t realesrgan_postproc_tta_fp16s_spv_data[] = {
    #include "realesrgan_postproc_tta_fp16s.spv.hex.h"
};
static const uint32_t realesrgan_postproc_tta_int8s_spv_data[] = {
    #include "realesrgan_postproc_tta_int8s.spv.hex.h"
};

class RealESRGANImpl : public RealESRGAN
{
public:
    RealESRGANImpl() = default;
    
    virtual ~RealESRGANImpl() = default;
    
    virtual int init(int _scale) override {
        if (inited_) {
            if (_scale == scale) {
                LOG_WARN("realesrgan, inited. _scale=" << scale);
                return 1;
            }
            uninit();
        }
        std::lock_guard<std::mutex> theGuard(mutex_);

        net.reset(new ncnn::Net());

        useVulkan_ = !NcnnEngine::useCPU();
        
        net->opt.use_vulkan_compute = useVulkan_;
        if (useVulkan_) {
            net->opt.use_fp16_packed = false;
            net->opt.use_fp16_storage = true;
            net->opt.use_fp16_arithmetic = true;
            net->opt.use_int8_storage = false;
            net->opt.use_int8_arithmetic = false;
        }
        else {
            //net->opt.num_threads = 8;
            net->opt.blob_allocator = &blob_pool_allocator_;
            net->opt.workspace_allocator = &workspace_pool_allocator_;
        }

        realesrgan_preproc = nullptr;
        realesrgan_postproc = nullptr;
        bicubic_2x = nullptr;
        bicubic_3x = nullptr;
        bicubic_4x = nullptr;
        tta_mode = false;
        scale = _scale;

        std::string resFolder = getCurrFilePath() + "/Resources/models/";
        LOG_INFO("load models from resource folder:" << resFolder);

        int ret0 = net->load_param((resFolder + "realesr-animevideov3-x" + std::to_string(scale) + ".param").c_str());
        int ret1 = net->load_model((resFolder + "realesr-animevideov3-x" + std::to_string(scale) + ".bin").c_str());
        LOG_INFO("load realsrgan, ret0=" << ret0 << ", ret1=" << ret1 << ", scale=" << scale);
        if (ret0 != 0 || ret1 != 0)
            return -1;
        
        preparePipeline();
        
        inited_ = true;
        return 0;
    }
    
    void preparePipeline() {
        if (useVulkan_) {
            std::vector<ncnn::vk_specialization_type> specializations(1);
    #if _WIN32
            specializations[0].i = 1;
    #else
            specializations[0].i = 0;
    #endif
            realesrgan_preproc = new ncnn::Pipeline(net->vulkan_device());
            realesrgan_preproc->set_optimal_local_size_xyz(32, 32, 3);

            realesrgan_postproc = new ncnn::Pipeline(net->vulkan_device());
            realesrgan_postproc->set_optimal_local_size_xyz(32, 32, 3);

            if (tta_mode) {
                if (net->opt.use_fp16_storage && net->opt.use_int8_storage)
                    realesrgan_preproc->create(realesrgan_preproc_tta_int8s_spv_data, sizeof(realesrgan_preproc_tta_int8s_spv_data), specializations);
                else if (net->opt.use_fp16_storage)
                    realesrgan_preproc->create(realesrgan_preproc_tta_fp16s_spv_data, sizeof(realesrgan_preproc_tta_fp16s_spv_data), specializations);
                else
                    realesrgan_preproc->create(realesrgan_preproc_tta_spv_data, sizeof(realesrgan_preproc_tta_spv_data), specializations);

                if (net->opt.use_fp16_storage && net->opt.use_int8_storage)
                    realesrgan_postproc->create(realesrgan_postproc_tta_int8s_spv_data, sizeof(realesrgan_postproc_tta_int8s_spv_data), specializations);
                else if (net->opt.use_fp16_storage)
                    realesrgan_postproc->create(realesrgan_postproc_tta_fp16s_spv_data, sizeof(realesrgan_postproc_tta_fp16s_spv_data), specializations);
                else
                    realesrgan_postproc->create(realesrgan_postproc_tta_spv_data, sizeof(realesrgan_postproc_tta_spv_data), specializations);
            }
            else {
                if (net->opt.use_fp16_storage && net->opt.use_int8_storage)
                    realesrgan_preproc->create(realesrgan_preproc_int8s_spv_data, sizeof(realesrgan_preproc_int8s_spv_data), specializations);
                else if (net->opt.use_fp16_storage)
                    realesrgan_preproc->create(realesrgan_preproc_fp16s_spv_data, sizeof(realesrgan_preproc_fp16s_spv_data), specializations);
                else
                    realesrgan_preproc->create(realesrgan_preproc_spv_data, sizeof(realesrgan_preproc_spv_data), specializations);

                if (net->opt.use_fp16_storage && net->opt.use_int8_storage)
                    realesrgan_postproc->create(realesrgan_postproc_int8s_spv_data, sizeof(realesrgan_postproc_int8s_spv_data), specializations);
                else if (net->opt.use_fp16_storage)
                    realesrgan_postproc->create(realesrgan_postproc_fp16s_spv_data, sizeof(realesrgan_postproc_fp16s_spv_data), specializations);
                else
                    realesrgan_postproc->create(realesrgan_postproc_spv_data, sizeof(realesrgan_postproc_spv_data), specializations);
            }
        }
        
        auto bicubicOpt = net->opt;
        bicubicOpt.use_vulkan_compute = useVulkan_;
        // bicubic 2x/3x/4x for alpha channel
        {
            bicubic_2x = ncnn::create_layer("Interp");
            if (bicubicOpt.use_vulkan_compute)
                bicubic_2x->vkdev = net->vulkan_device();
            else
                bicubic_2x->support_vulkan = false;

            ncnn::ParamDict pd;
            pd.set(0, 3);// bicubic
            pd.set(1, 2.f);
            pd.set(2, 2.f);
            bicubic_2x->load_param(pd);

            bicubic_2x->create_pipeline(bicubicOpt);
        }
        {
            bicubic_3x = ncnn::create_layer("Interp");
            if (bicubicOpt.use_vulkan_compute)
                bicubic_3x->vkdev = net->vulkan_device();
            else
                bicubic_3x->support_vulkan = false;

            ncnn::ParamDict pd;
            pd.set(0, 3);// bicubic
            pd.set(1, 3.f);
            pd.set(2, 3.f);
            bicubic_3x->load_param(pd);

            bicubic_3x->create_pipeline(bicubicOpt);
        }
        {
            bicubic_4x = ncnn::create_layer("Interp");
            if (bicubicOpt.use_vulkan_compute)
                bicubic_4x->vkdev = net->vulkan_device();
            else
                bicubic_4x->support_vulkan = false;

            ncnn::ParamDict pd;
            pd.set(0, 3);// bicubic
            pd.set(1, 4.f);
            pd.set(2, 4.f);
            bicubic_4x->load_param(pd);

            bicubic_4x->create_pipeline(bicubicOpt);
        }
        
        if (useVulkan_) {
            auto gpu_id = ncnn::get_default_gpu_index();
            uint32_t heap_budget = ncnn::get_gpu_device(gpu_id)->get_heap_budget();

            if (heap_budget > 1900)
                tilesize = 400;
            else if (heap_budget > 550)
                tilesize = 200;
            else if (heap_budget > 190)
                tilesize = 128;
            else
                tilesize = 64;
            
            LOG_INFO("default gpu id:" << gpu_id << ", heap=" << heap_budget << ", title=" << tilesize);
        }
        LOG_INFO("preparePipeline done, use gpu:" << useVulkan_ << ", title=" << tilesize);
    }
    
    cv::Mat inferenceVulkan(const cv::Mat& input) {
        const int w = input.cols;
        const int h = input.rows;
        const int channels = input.channels();
        cv::Mat outimage(h * scale, w * scale, input.flags);
        
        const unsigned char* pixeldata = input.data;
        const int TILE_SIZE_X = tilesize;
        const int TILE_SIZE_Y = tilesize;

        ncnn::VkAllocator* blob_vkallocator = net->vulkan_device()->acquire_blob_allocator();
        ncnn::VkAllocator* staging_vkallocator = net->vulkan_device()->acquire_staging_allocator();

        ncnn::Option opt = net->opt;
        opt.blob_vkallocator = blob_vkallocator;
        opt.workspace_vkallocator = blob_vkallocator;
        opt.staging_vkallocator = staging_vkallocator;

        // each tile 100x100
        const int xtiles = (w + TILE_SIZE_X - 1) / TILE_SIZE_X;
        const int ytiles = (h + TILE_SIZE_Y - 1) / TILE_SIZE_Y;

        const size_t in_out_tile_elemsize = opt.use_fp16_storage ? 2u : 4u;

        //#pragma omp parallel for num_threads(2)
        for (int yi = 0; yi < ytiles; yi++) {
            const int tile_h_nopad = (std::min)((yi + 1) * TILE_SIZE_Y, h) - yi * TILE_SIZE_Y;

            int in_tile_y0 = (std::max)(yi * TILE_SIZE_Y - prepadding, 0);
            int in_tile_y1 = (std::min)((yi + 1) * TILE_SIZE_Y + prepadding, h);

            ncnn::Mat in;
            if (opt.use_fp16_storage && opt.use_int8_storage) {
                in = ncnn::Mat(w, (in_tile_y1 - in_tile_y0), (unsigned char*)pixeldata + in_tile_y0 * w * channels, (size_t)channels, 1);
            }
            else {
                if (channels == 3) {
                    in = ncnn::Mat::from_pixels(pixeldata + in_tile_y0 * w * channels, ncnn::Mat::PIXEL_BGR2RGB, w, (in_tile_y1 - in_tile_y0));
                }
                if (channels == 4) {
                    in = ncnn::Mat::from_pixels(pixeldata + in_tile_y0 * w * channels, ncnn::Mat::PIXEL_BGRA2RGBA, w, (in_tile_y1 - in_tile_y0));
                }
            }

            ncnn::VkCompute cmd(net->vulkan_device());

            // upload
            ncnn::VkMat in_gpu;
            {
                cmd.record_clone(in, in_gpu, opt);

                if (xtiles > 1) {
                    cmd.submit_and_wait();
                    cmd.reset();
                }
            }

            int out_tile_y0 = (std::max)(yi * TILE_SIZE_Y, 0);
            int out_tile_y1 = (std::min)((yi + 1) * TILE_SIZE_Y, h);

            ncnn::VkMat out_gpu;
            if (opt.use_fp16_storage && opt.use_int8_storage) {
                out_gpu.create(w * scale, (out_tile_y1 - out_tile_y0) * scale, (size_t)channels, 1, blob_vkallocator);
            }
            else {
                out_gpu.create(w * scale, (out_tile_y1 - out_tile_y0) * scale, channels, (size_t)4u, 1, blob_vkallocator);
            }

            for (int xi = 0; xi < xtiles; xi++) {
                const int tile_w_nopad = (std::min)((xi + 1) * TILE_SIZE_X, w) - xi * TILE_SIZE_X;

                if (tta_mode) {
                    // preproc
                    ncnn::VkMat in_tile_gpu[8];
                    ncnn::VkMat in_alpha_tile_gpu;
                    {
                        // crop tile
                        int tile_x0 = xi * TILE_SIZE_X - prepadding;
                        int tile_x1 = (std::min)((xi + 1) * TILE_SIZE_X, w) + prepadding;
                        int tile_y0 = yi * TILE_SIZE_Y - prepadding;
                        int tile_y1 = (std::min)((yi + 1) * TILE_SIZE_Y, h) + prepadding;

                        in_tile_gpu[0].create(tile_x1 - tile_x0, tile_y1 - tile_y0, 3, in_out_tile_elemsize, 1, blob_vkallocator);
                        in_tile_gpu[1].create(tile_x1 - tile_x0, tile_y1 - tile_y0, 3, in_out_tile_elemsize, 1, blob_vkallocator);
                        in_tile_gpu[2].create(tile_x1 - tile_x0, tile_y1 - tile_y0, 3, in_out_tile_elemsize, 1, blob_vkallocator);
                        in_tile_gpu[3].create(tile_x1 - tile_x0, tile_y1 - tile_y0, 3, in_out_tile_elemsize, 1, blob_vkallocator);
                        in_tile_gpu[4].create(tile_y1 - tile_y0, tile_x1 - tile_x0, 3, in_out_tile_elemsize, 1, blob_vkallocator);
                        in_tile_gpu[5].create(tile_y1 - tile_y0, tile_x1 - tile_x0, 3, in_out_tile_elemsize, 1, blob_vkallocator);
                        in_tile_gpu[6].create(tile_y1 - tile_y0, tile_x1 - tile_x0, 3, in_out_tile_elemsize, 1, blob_vkallocator);
                        in_tile_gpu[7].create(tile_y1 - tile_y0, tile_x1 - tile_x0, 3, in_out_tile_elemsize, 1, blob_vkallocator);

                        if (channels == 4)
                        {
                            in_alpha_tile_gpu.create(tile_w_nopad, tile_h_nopad, 1, in_out_tile_elemsize, 1, blob_vkallocator);
                        }

                        std::vector<ncnn::VkMat> bindings(10);
                        bindings[0] = in_gpu;
                        bindings[1] = in_tile_gpu[0];
                        bindings[2] = in_tile_gpu[1];
                        bindings[3] = in_tile_gpu[2];
                        bindings[4] = in_tile_gpu[3];
                        bindings[5] = in_tile_gpu[4];
                        bindings[6] = in_tile_gpu[5];
                        bindings[7] = in_tile_gpu[6];
                        bindings[8] = in_tile_gpu[7];
                        bindings[9] = in_alpha_tile_gpu;

                        std::vector<ncnn::vk_constant_type> constants(13);
                        constants[0].i = in_gpu.w;
                        constants[1].i = in_gpu.h;
                        constants[2].i = in_gpu.cstep;
                        constants[3].i = in_tile_gpu[0].w;
                        constants[4].i = in_tile_gpu[0].h;
                        constants[5].i = in_tile_gpu[0].cstep;
                        constants[6].i = prepadding;
                        constants[7].i = prepadding;
                        constants[8].i = xi * TILE_SIZE_X;
                        constants[9].i = (std::min)(yi * TILE_SIZE_Y, prepadding);
                        constants[10].i = channels;
                        constants[11].i = in_alpha_tile_gpu.w;
                        constants[12].i = in_alpha_tile_gpu.h;

                        ncnn::VkMat dispatcher;
                        dispatcher.w = in_tile_gpu[0].w;
                        dispatcher.h = in_tile_gpu[0].h;
                        dispatcher.c = channels;

                        cmd.record_pipeline(realesrgan_preproc, bindings, constants, dispatcher);
                    }

                    // realesrgan
                    ncnn::VkMat out_tile_gpu[8];
                    for (int ti = 0; ti < 8; ti++)
                    {
                        ncnn::Extractor ex = net->create_extractor();

                        ex.set_blob_vkallocator(blob_vkallocator);
                        ex.set_workspace_vkallocator(blob_vkallocator);
                        ex.set_staging_vkallocator(staging_vkallocator);

                        ex.input("data", in_tile_gpu[ti]);

                        ex.extract("output", out_tile_gpu[ti], cmd);

                        {
                            cmd.submit_and_wait();
                            cmd.reset();
                        }
                    }

                    ncnn::VkMat out_alpha_tile_gpu;
                    if (channels == 4)
                    {
                        if (scale == 1)
                        {
                            out_alpha_tile_gpu = in_alpha_tile_gpu;
                        }
                        if (scale == 2)
                        {
                            bicubic_2x->forward(in_alpha_tile_gpu, out_alpha_tile_gpu, cmd, opt);
                        }
                        if (scale == 3)
                        {
                            bicubic_3x->forward(in_alpha_tile_gpu, out_alpha_tile_gpu, cmd, opt);
                        }
                        if (scale == 4)
                        {
                            bicubic_4x->forward(in_alpha_tile_gpu, out_alpha_tile_gpu, cmd, opt);
                        }
                    }

                    // postproc
                    {
                        std::vector<ncnn::VkMat> bindings(10);
                        bindings[0] = out_tile_gpu[0];
                        bindings[1] = out_tile_gpu[1];
                        bindings[2] = out_tile_gpu[2];
                        bindings[3] = out_tile_gpu[3];
                        bindings[4] = out_tile_gpu[4];
                        bindings[5] = out_tile_gpu[5];
                        bindings[6] = out_tile_gpu[6];
                        bindings[7] = out_tile_gpu[7];
                        bindings[8] = out_alpha_tile_gpu;
                        bindings[9] = out_gpu;

                        std::vector<ncnn::vk_constant_type> constants(13);
                        constants[0].i = out_tile_gpu[0].w;
                        constants[1].i = out_tile_gpu[0].h;
                        constants[2].i = out_tile_gpu[0].cstep;
                        constants[3].i = out_gpu.w;
                        constants[4].i = out_gpu.h;
                        constants[5].i = out_gpu.cstep;
                        constants[6].i = xi * TILE_SIZE_X * scale;
                        constants[7].i = (std::min)(TILE_SIZE_X * scale, out_gpu.w - xi * TILE_SIZE_X * scale);
                        constants[8].i = prepadding * scale;
                        constants[9].i = prepadding * scale;
                        constants[10].i = channels;
                        constants[11].i = out_alpha_tile_gpu.w;
                        constants[12].i = out_alpha_tile_gpu.h;

                        ncnn::VkMat dispatcher;
                        dispatcher.w = (std::min)(TILE_SIZE_X * scale, out_gpu.w - xi * TILE_SIZE_X * scale);
                        dispatcher.h = out_gpu.h;
                        dispatcher.c = channels;

                        cmd.record_pipeline(realesrgan_postproc, bindings, constants, dispatcher);
                    }
                }
                else
                {
                    // preproc
                    ncnn::VkMat in_tile_gpu;
                    ncnn::VkMat in_alpha_tile_gpu;
                    {
                        // crop tile
                        int tile_x0 = xi * TILE_SIZE_X - prepadding;
                        int tile_x1 = (std::min)((xi + 1) * TILE_SIZE_X, w) + prepadding;
                        int tile_y0 = yi * TILE_SIZE_Y - prepadding;
                        int tile_y1 = (std::min)((yi + 1) * TILE_SIZE_Y, h) + prepadding;

                        in_tile_gpu.create(tile_x1 - tile_x0, tile_y1 - tile_y0, 3, in_out_tile_elemsize, 1, blob_vkallocator);

                        if (channels == 4)
                        {
                            in_alpha_tile_gpu.create(tile_w_nopad, tile_h_nopad, 1, in_out_tile_elemsize, 1, blob_vkallocator);
                        }

                        std::vector<ncnn::VkMat> bindings(3);
                        bindings[0] = in_gpu;
                        bindings[1] = in_tile_gpu;
                        bindings[2] = in_alpha_tile_gpu;

                        std::vector<ncnn::vk_constant_type> constants(13);
                        constants[0].i = in_gpu.w;
                        constants[1].i = in_gpu.h;
                        constants[2].i = in_gpu.cstep;
                        constants[3].i = in_tile_gpu.w;
                        constants[4].i = in_tile_gpu.h;
                        constants[5].i = in_tile_gpu.cstep;
                        constants[6].i = prepadding;
                        constants[7].i = prepadding;
                        constants[8].i = xi * TILE_SIZE_X;
                        constants[9].i = (std::min)(yi * TILE_SIZE_Y, prepadding);
                        constants[10].i = channels;
                        constants[11].i = in_alpha_tile_gpu.w;
                        constants[12].i = in_alpha_tile_gpu.h;

                        ncnn::VkMat dispatcher;
                        dispatcher.w = in_tile_gpu.w;
                        dispatcher.h = in_tile_gpu.h;
                        dispatcher.c = channels;

                        cmd.record_pipeline(realesrgan_preproc, bindings, constants, dispatcher);
                    }

                    // realesrgan
                    ncnn::VkMat out_tile_gpu;
                    {
                        ncnn::Extractor ex = net->create_extractor();

                        ex.set_blob_vkallocator(blob_vkallocator);
                        ex.set_workspace_vkallocator(blob_vkallocator);
                        ex.set_staging_vkallocator(staging_vkallocator);

                        ex.input("data", in_tile_gpu);

                        ex.extract("output", out_tile_gpu, cmd);
                    }

                    ncnn::VkMat out_alpha_tile_gpu;
                    if (channels == 4)
                    {
                        if (scale == 1)
                        {
                            out_alpha_tile_gpu = in_alpha_tile_gpu;
                        }
                        if (scale == 2)
                        {
                            bicubic_2x->forward(in_alpha_tile_gpu, out_alpha_tile_gpu, cmd, opt);
                        }
                        if (scale == 3)
                        {
                            bicubic_3x->forward(in_alpha_tile_gpu, out_alpha_tile_gpu, cmd, opt);
                        }
                        if (scale == 4)
                        {
                            bicubic_4x->forward(in_alpha_tile_gpu, out_alpha_tile_gpu, cmd, opt);
                        }
                    }

                    // postproc
                    {
                        std::vector<ncnn::VkMat> bindings(3);
                        bindings[0] = out_tile_gpu;
                        bindings[1] = out_alpha_tile_gpu;
                        bindings[2] = out_gpu;

                        std::vector<ncnn::vk_constant_type> constants(13);
                        constants[0].i = out_tile_gpu.w;
                        constants[1].i = out_tile_gpu.h;
                        constants[2].i = out_tile_gpu.cstep;
                        constants[3].i = out_gpu.w;
                        constants[4].i = out_gpu.h;
                        constants[5].i = out_gpu.cstep;
                        constants[6].i = xi * TILE_SIZE_X * scale;
                        constants[7].i = std::min(TILE_SIZE_X * scale, out_gpu.w - xi * TILE_SIZE_X * scale);
                        constants[8].i = prepadding * scale;
                        constants[9].i = prepadding * scale;
                        constants[10].i = channels;
                        constants[11].i = out_alpha_tile_gpu.w;
                        constants[12].i = out_alpha_tile_gpu.h;

                        ncnn::VkMat dispatcher;
                        dispatcher.w = std::min(TILE_SIZE_X * scale, out_gpu.w - xi * TILE_SIZE_X * scale);
                        dispatcher.h = out_gpu.h;
                        dispatcher.c = channels;

                        cmd.record_pipeline(realesrgan_postproc, bindings, constants, dispatcher);
                    }
                }

                if (xtiles > 1)
                {
                    cmd.submit_and_wait();
                    cmd.reset();
                }

                fprintf(stderr, "%.2f%%\n", (float)(yi * xtiles + xi) / (ytiles * xtiles) * 100);
            }

            // download
            {
                ncnn::Mat out;

                if (opt.use_fp16_storage && opt.use_int8_storage)
                {
                    out = ncnn::Mat(out_gpu.w, out_gpu.h, (unsigned char*)outimage.data + yi * scale * TILE_SIZE_Y * w * scale * channels, (size_t)channels, 1);
                }

                cmd.record_clone(out_gpu, out, opt);

                cmd.submit_and_wait();

                if (!(opt.use_fp16_storage && opt.use_int8_storage)) {
                    if (channels == 3) {
                        out.to_pixels((unsigned char*)outimage.data + yi * scale * TILE_SIZE_Y * w * scale * channels, ncnn::Mat::PIXEL_RGB2BGR);
                    }
                    if (channels == 4) {
                        out.to_pixels((unsigned char*)outimage.data + yi * scale * TILE_SIZE_Y * w * scale * channels, ncnn::Mat::PIXEL_RGBA2BGRA);
                    }
                }
            }
        }

        net->vulkan_device()->reclaim_blob_allocator(blob_vkallocator);
        net->vulkan_device()->reclaim_staging_allocator(staging_vkallocator);
        
        return outimage;
    }
    
    cv::Mat inferenceCPU(const cv::Mat& input) {
        const int w = input.cols;
        const int h = input.rows;
        const int channels = input.channels();
        
        cv::Mat outimage(h * scale, w * scale, input.flags);
        
        const unsigned char* pixeldata = input.data;
        const int TILE_SIZE_X = tilesize;
        const int TILE_SIZE_Y = tilesize;

        // each tile 100x100
        const int xtiles = (w + TILE_SIZE_X - 1) / TILE_SIZE_X;
        const int ytiles = (h + TILE_SIZE_Y - 1) / TILE_SIZE_Y;
        
        int pixelType = ncnn::Mat::PIXEL_BGR2RGB;
        int toType = ncnn::Mat::PIXEL_RGB2BGR;
        if (channels == 4) {
            pixelType = ncnn::Mat::PIXEL_BGRA2RGB;
            toType = ncnn::Mat::PIXEL_RGB2BGRA;
        }
        const float normal_val[3] = {1.f / 255, 1.f / 255, 1.f / 255};
        const float mean_val[3] = {0, 0, 0};
        const float denormal_val[3] = {255.f, 255.f, 255.f};

        for (int yi = 0; yi < ytiles; yi++) {
            const int tile_h_nopad = (std::min)((yi + 1) * TILE_SIZE_Y, h) - yi * TILE_SIZE_Y;
            
            for (int xi = 0; xi < xtiles; xi++) {
                const int tile_w_nopad = (std::min)((xi + 1) * TILE_SIZE_X, w) - xi * TILE_SIZE_X;
                
                int tile_x0 = xi * TILE_SIZE_X - prepadding;
                int tile_x1 = (std::min)((xi + 1) * TILE_SIZE_X, w) + prepadding;
                int tile_y0 = yi * TILE_SIZE_Y - prepadding;
                int tile_y1 = (std::min)((yi + 1) * TILE_SIZE_Y, h) + prepadding;

                ncnn::Mat in, out;
                in = ncnn::Mat::from_pixels_roi(pixeldata, pixelType, w, h, tile_x0, tile_y0, tile_x1 - tile_x0, tile_y1 - tile_y0);
                in.substract_mean_normalize(mean_val, normal_val);
                
                ncnn::Extractor ex = net->create_extractor();
                int hr = ex.input("data", in);
                hr = ex.extract("output", out);

                out.substract_mean_normalize(mean_val, denormal_val);
                out.to_pixels(outimage.ptr(tile_y0 * scale, tile_x0 * scale), toType, outimage.step);
                
                LOG_INFO("title processed, tile_x0=" << tile_x0 << ", tile_y0=" << tile_y0 << ", step=" << outimage.step);
            }
        }
        
        if (channels == 4) {
            std::vector<cv::Mat> comps;
            cv::split(input, comps);
                        
            std::vector<cv::Mat> out_comps;
            cv::split(outimage, out_comps);
            
            cv::resize(comps[3], out_comps[3], out_comps[3].size(), 0, 0, cv::INTER_CUBIC);
            cv::merge(out_comps, outimage);
        }
        return outimage;
    }

    virtual cv::Mat inference(const cv::Mat& input) override {
        if (useVulkan_) {
            return inferenceVulkan(input);
        }
        return inferenceCPU(input);
    }
    
    virtual void uninit() override {
        std::lock_guard<std::mutex> theGuard(mutex_);
        if (!inited_)
            return;

        if (realesrgan_preproc) {
            delete realesrgan_preproc;
            delete realesrgan_postproc;
            realesrgan_preproc = nullptr;
            realesrgan_postproc = nullptr;
        }

        if (bicubic_2x) {
            bicubic_2x->destroy_pipeline(net->opt);
            delete bicubic_2x;
            bicubic_2x = nullptr;
        }

        if (bicubic_3x) {
            bicubic_3x->destroy_pipeline(net->opt);
            delete bicubic_3x;
            bicubic_3x = nullptr;
        }

        if (bicubic_4x) {
            bicubic_4x->destroy_pipeline(net->opt);
            delete bicubic_4x;
            bicubic_4x = nullptr;
        }
        net = nullptr;
        inited_ = false;
    }

protected:
    int load(const std::string& parampath, const std::string& modelpath);
    // realesrgan parameters
    int scale = 4;
    int tilesize = 200;
    int prepadding = 0;

private:
    std::mutex mutex_;
    bool inited_ = false;
    bool useVulkan_ = true;

    ncnn::UnlockedPoolAllocator blob_pool_allocator_;
    ncnn::PoolAllocator workspace_pool_allocator_;

    std::unique_ptr<ncnn::Net> net;
    ncnn::Pipeline* realesrgan_preproc = nullptr;
    ncnn::Pipeline* realesrgan_postproc = nullptr;
    ncnn::Layer* bicubic_2x = nullptr;
    ncnn::Layer* bicubic_3x = nullptr;
    ncnn::Layer* bicubic_4x = nullptr;
    bool tta_mode = true;

};

RealESRGAN* RealESRGAN::instance()
{
    static RealESRGANImpl inst;
    
    return &inst;
}

}
