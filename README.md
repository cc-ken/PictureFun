## Introduction

This repository is a simple cross-platform AI picture processing software, it uses [NCNN](https://github.com/Tencent/ncnn) as the AI inference engine to get the cross-platform GPU implementation through [vulkan](https://www.lunarg.com/vulkan-sdk/).

## Dependency

Download [vulkan sdk](https://www.lunarg.com/vulkan-sdk/) and export the environment variable `VULKAN_SDK`, like in my computer:

    export VULKAN_SDK=~/VulkanSDK/1.3.216.0/macOS

Install conan by pip: `pip3 install conan`

## Build from source

### Macos

    # build conan dependency
    #cd into conan/recipes to create following modules (universal, conan center doesn't have universal lib)
    conan create ./ ncnn/1.0.0@pf/stable -s os=Macos
    conan create ./ spdlog/1.10.0@pf/stable -s os=Macos
    conan create ./ tinyxml2/9.0.0@pf/stable -s os=Macos -k
    conan create ./ opencv/4.5.2@pf/stable -s os=Macos -k
    conan create ./ qt/6.2.4@pf/stable -s os=Macos -k

    # back to the 
    mkdir Macos
    cd Macos
    conan install ../pf -s os=Macos

    # create Xcode projects
    cd ../..
    mkdir build
    cd build
    cmake .. -G Xcode

    #cmake ../qt6 -GNinja -DCMAKE_INSTALL_PREFIX=./install -DCMAKE_OSX_DEPLOYMENT_TARGET="10.15" -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64" -DFEATURE_framework=OFF -DQT_BUILD_EXAMPLES=OFF -DQT_BUILD_TESTS=OFF 

### Windows

    # build ncnn conan dependency
    cd conan/recipes/ncnn/all
    conan create ./ ncnn/1.0.0@pf/stable -s compiler="Visual Studio" -s arch=x86_64

    # install opencv/qt dependency
    cd ../../../
    mkdir Windows
    cd Windows
    conan install ../ -s compiler="Visual Studio" -s compiler.runtime=MDd -s build_type=Debug -s arch=x86_64 --build=missing
    conan install ../ -s compiler="Visual Studio" -s compiler.runtime=MD -s build_type=Release -s arch=x86_64 --build=missing

    # create visual studio projects
    cd ../..
    mkdir build
    cd build
    cmake .. -G"Visual Studio 17 2022"
    
## AI models we used

1. The current using background remove model is from this [ncnn_Android_matting](https://github.com/FeiGeChuanShu/ncnn_Android_matting.git), `hrnet-w18`, download them and put into `src/models)` folder.

2. We integrate the [Real-ESRGAN](https://github.com/xinntao/Real-ESRGAN-ncnn-vulkan) for super resolution.
The model can be downloaded from [animevideov3](https://github.com/xinntao/Real-ESRGAN/releases/download/v0.2.5.0/realesr-animevideov3.pth)
