cmake_minimum_required(VERSION 3.20)

project(Serf)

# Set C++ standard version
set(CMAKE_CXX_STANDARD 17)

# -O3 Optimization for release version
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

# Set parallel compilation level as 4
set(CMAKE_BUILD_PARALLEL_LEVEL 4)

# Print project self check info
message(STATUS "[SelfCheck] Project Source Dir: " ${PROJECT_SOURCE_DIR})
message(STATUS "[SelfCheck] Project Binary Dir: " ${PROJECT_BINARY_DIR})

# Set executable file path and print it
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
message(STATUS "[SelfCheck] Project Executable Dir: " ${EXECUTABLE_OUTPUT_PATH})

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
add_subdirectory(src/deflate)
add_subdirectory(src/fpc)
add_subdirectory(src/lz4)
add_subdirectory(src/chimp)
add_subdirectory(test)
add_subdirectory(pywrapper)