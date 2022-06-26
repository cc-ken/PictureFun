if (APPLE)
    if (IOS)
        set(TARGET_OS "iOS")
    else()
        set(TARGET_OS "Macos")
        set(MACOSX ON)
    endif()
elseif (ANDROID)
    set(TARGET_OS "Android")
elseif (MSVC)
    set(TARGET_OS "Windows")
    set(WINDOWS ON)
endif()

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(CMAKE_CXX_STANDARD 17)

if (LINUX)
   set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
   set(CMAKE_BUILD_TYPE "Release")
   set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
   set(CMAKE_INSTALL_RPATH "\$ORIGIN:${CMAKE_INSTALL_RPATH}")
   set(SYSTEM_LIBS X11 CACHE INTERNAL "")
endif()

if (MACOSX)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "11.0")
endif()

if (WINDOWS)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi")
    set(CMAKE_PDB_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/pdbs)
    set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /INCREMENTAL"  CACHE INTERNAL "")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /INCREMENTAL" CACHE INTERNAL "")
    set(SYSTEM_LIBS winmm Shell32 CACHE INTERNAL "")
else()
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -g")
endif()