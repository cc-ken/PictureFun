project(utils)

set(target ${PROJECT_NAME})

include_directories(../)

find_package(tinyxml2 REQUIRED)
find_package(OpenCV REQUIRED)
find_package(spdlog REQUIRED)

file(GLOB UTILS_SRC_FILES ${CMAKE_CURRENT_LIST_DIR}/*.h ${CMAKE_CURRENT_LIST_DIR}/*.cpp)

if (APPLE)
    file(GLOB UTILS_COCOA_FILES ${CMAKE_CURRENT_LIST_DIR}/*.mm)
endif()

add_library(${target} STATIC ${UTILS_SRC_FILES} ${UTILS_COCOA_FILES})

target_link_libraries(${target} PUBLIC opencv::opencv spdlog::spdlog tinyxml2::tinyxml2)
