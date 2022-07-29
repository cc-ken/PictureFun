## Introduction

This repository is a simple cross-platform AI picture processing software, it uses [NCNN](https://github.com/Tencent/ncnn) as the AI inference engine to get the cross-platform GPU implementation through [vulkan](https://www.lunarg.com/vulkan-sdk/).

## Dependency

Download [vulkan sdk](https://www.lunarg.com/vulkan-sdk/) and export the environment variable `VULKAN_SDK`, like in my computer:

    export VULKAN_SDK=~/VulkanSDK/1.3.216.0/macOS

## Build from source

## AI models we used

1. The current using background remove model is from this [ncnn_Android_matting](https://github.com/FeiGeChuanShu/ncnn_Android_matting.git), `hrnet-w18`, download them and put into `src/models)` folder.

2. We integrate the [Real-ESRGAN](https://github.com/xinntao/Real-ESRGAN-ncnn-vulkan) for super resolution.
The model can be downloaded from [animevideov3](https://github.com/xinntao/Real-ESRGAN/releases/download/v0.2.5.0/realesr-animevideov3.pth)
