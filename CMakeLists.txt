cmake_minimum_required(VERSION 3.20)

project()

# Print system info
if(WIN32)
    message(STATUS "[SelfCheck] Host System: Windows (32bit)")
    add_definitions(-DWIN32)
elseif(WIN64)
    message(STATUS "[SelfCheck] Host System: Windows (64bit)")
    add_definitions(-DWIN64)
elseif(APPLE)
    message(STATUS "[SelfCheck] Host System: Apple MacOS")
    add_definitions(-D__MACH__)
elseif(UNIX)
    message(STATUS "[SelfCheck] Host System: Linux/Unix/*nix")
    add_definitions(-DUNIX -DLINUX)
else()
    message(STATUS "[SelfCheck] Host System: Unknown")
endif(WIN32)

message(STATUS "[SelfCheck] Loading GoogleTest module")

# Set GoogleTest version
set(GTEST_VER "1.14.0")

# Load FetchContent Module
include(FetchContent)

# Auto fetch GoogleTest source code
FetchContent_Declare(
    googletest
    DOWNLOAD_EXTRACT_TIMESTAMP true
    URL https://github.com/google/googletest/archive/refs/tags/v${GTEST_VER}.zip)
# For Windows: Prevent overriding the parent project's compiler/linker settings
# set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

message(STATUS "[SelfCheck] Loaded GoogleTest module")

add_subdirectory(src/serf)
add_subdirectory(baselines/deflate)
add_subdirectory(baselines/fpc)
add_subdirectory(baselines/lz4)
add_subdirectory(baselines/chimp128)
add_subdirectory(baselines/gorilla)
add_subdirectory(baselines/elf)
add_subdirectory(baselines/machete)
add_subdirectory(baselines/lz77)
add_subdirectory(baselines/sz3)
add_subdirectory(baselines/lzw)
add_subdirectory(baselines/alp)
add_subdirectory(baselines/buff)
add_subdirectory(baselines/sz_adt)
add_subdirectory(test)

# If Python API and JAVA API are needed
# add_subdirectory(pywrapper)
# add_subdirectory(jna)