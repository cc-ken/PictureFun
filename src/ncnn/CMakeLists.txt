project(ncnn_engine)

set(target ${PROJECT_NAME})

include_directories(
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/../../
    $ENV{VULKAN_SDK}/include
)

if (APPLE)
    include_directories(
        "${ncnn_FRAMEWORK_DIRS}/ncnn.framework/Versions/Current/Headers/ncnn")
endif()

file(GLOB NCNN_ENGINE_SRC_FILES ${CMAKE_CURRENT_LIST_DIR}/*.h ${CMAKE_CURRENT_LIST_DIR}/*.cpp)

add_library(${target} STATIC ${NCNN_ENGINE_SRC_FILES})

target_link_libraries(${target} PUBLIC utils)
