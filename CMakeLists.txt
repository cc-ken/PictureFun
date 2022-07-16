cmake_minimum_required(VERSION 3.13.0)

project(PictureFunWorkspace)

include(cmake/predefines.cmake)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/conan/${TARGET_OS}")

find_package(OpenCV REQUIRED)
find_package(Qt6 REQUIRED)
find_package(ncnn REQUIRED)

add_subdirectory(utils)
add_subdirectory(src)
