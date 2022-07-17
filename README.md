## Introduction

This repository is a simple cross-platform AI picture processing software, it uses [NCNN](https://github.com/Tencent/ncnn) as the AI inference engine to get the cross-platform GPU implementation through [vulkan](https://www.lunarg.com/vulkan-sdk/).

## Dependency

Download [vulkan sdk](https://www.lunarg.com/vulkan-sdk/) and export the environment variable `VULKAN_SDK`, like in my computer:

    export VULKAN_SDK=~/VulkanSDK/1.3.216.0/macOS

## Build from source

## Where to download the models

The current using background remove model is from this [repo](https://github.com/FeiGeChuanShu/ncnn_Android_matting.git), `hrnet-w18`, download them and put into `src/models)` folder.
