cmake_minimum_required(VERSION 3.1)
project(test_package)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

find_package(OpenCV REQUIRED)

set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
option(built_with_ade "Enabled if opencv is built with ade" OFF)
if(built_with_ade)
    add_definitions(-DBUILT_WITH_ADE)
endif()

option(built_with_ffmpeg "Enabled if opencv is built with ffmpeg" OFF)
if(built_with_ffmpeg)
    add_definitions(-DBUILT_WITH_FFMPEG)
endif()

option(built_contrib_sfm "Enabled if opencv is built contrib sfm" OFF)
if(built_contrib_sfm)
    add_definitions(-DBUILT_CONTRIB_SFM)
endif()

add_executable(${PROJECT_NAME} test_package.cpp)
target_link_libraries(${PROJECT_NAME} opencv::opencv)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 11)
