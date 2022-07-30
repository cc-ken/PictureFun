project(ncnn_engine)

set(target ${PROJECT_NAME})

include_directories(
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/../../
    $ENV{VULKAN_SDK}/include
    ${CMAKE_BINARY_DIR}
)

if (MSVC)
    add_definitions(-DNOMINMAX)
endif()

file(GLOB GSLANG_COMP_FILES ${CMAKE_CURRENT_LIST_DIR}/*.comp)
foreach(COMP_FILE ${GSLANG_COMP_FILES})
    get_filename_component(COMP_FILENAME ${COMP_FILE} NAME_WLE)
    execute_process(COMMAND $ENV{VULKAN_SDK}/bin/glslc
        -mfmt=num -g
        "${COMP_FILE}"
        -o "${CMAKE_BINARY_DIR}/${COMP_FILENAME}.spv.hex.h"
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    )
    execute_process(COMMAND $ENV{VULKAN_SDK}/bin/glslc
        -mfmt=num -DNCNN_int8_storage=1 -g
        "${COMP_FILE}"
        -o "${CMAKE_BINARY_DIR}/${COMP_FILENAME}_int8s.spv.hex.h"
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    )
    execute_process(COMMAND $ENV{VULKAN_SDK}/bin/glslc
        -mfmt=num -DNCNN_fp16_storage=1 -g
        "${COMP_FILE}"
        -o "${CMAKE_BINARY_DIR}/${COMP_FILENAME}_fp16s.spv.hex.h"
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    )
endforeach()

#$VULKAN_SDK/bin/glslc -DNCNN_int8_storage=1 realesrgan_preproc.comp -mfmt=num
if (APPLE)
    include_directories(
        "${ncnn_FRAMEWORK_DIRS}/ncnn.framework/Versions/Current/Headers/ncnn")
endif()

file(GLOB NCNN_ENGINE_SRC_FILES ${CMAKE_CURRENT_LIST_DIR}/*.h ${CMAKE_CURRENT_LIST_DIR}/*.cpp)

add_library(${target} STATIC ${NCNN_ENGINE_SRC_FILES})

target_link_libraries(${target} PUBLIC utils ncnn::ncnn opencv::opencv)
