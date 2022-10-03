cmake_minimum_required(VERSION 3.13.0)

project(PictureFunWorkspace)

include(cmake/predefines.cmake)

if (APPLE)
set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
endif(APPLE)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/conan/${TARGET_OS}")
set(CMAKE_PREFIX_PATH "${CMAKE_CURRENT_LIST_DIR}/conan/${TARGET_OS}")

find_package(OpenCV REQUIRED)
find_package(Qt6 REQUIRED)
find_package(ncnn REQUIRED)

add_subdirectory(utils)
add_subdirectory(src)
