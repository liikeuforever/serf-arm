CMAKE_MINIMUM_REQUIRED(VERSION 3.20)

PROJECT(Serf)

# Set C++ standard version
SET(CMAKE_CXX_STANDARD 17)

# -O3 Optimization for release version
SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

# Print project self check info
MESSAGE(STATUS "[SelfCheck] Project Source Dir: " ${PROJECT_SOURCE_DIR})
MESSAGE(STATUS "[SelfCheck] Project Binary Dir: " ${PROJECT_BINARY_DIR})

# Set executable file path and print it
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
MESSAGE(STATUS "[SelfCheck] Project Executable Dir: " ${EXECUTABLE_OUTPUT_PATH})

# Print system info
IF(WIN32)
    MESSAGE(STATUS "[SelfCheck] Host System: Windows (32bit)")
    ADD_DEFINITIONS(-DWIN32)
ELSEIF(WIN64)
    MESSAGE(STATUS "[SelfCheck] Host System: Windows (64bit)")
    ADD_DEFINITIONS(-DWIN64)
ELSEIF(APPLE)
    MESSAGE(STATUS "[SelfCheck] Host System: Apple MacOS")
    ADD_DEFINITIONS(-D__MACH__ )
ELSEIF(UNIX)
    MESSAGE(STATUS "[SelfCheck] Host System: Linux/Unix/*nix")
    ADD_DEFINITIONS(-DUNIX -DLINUX)
ELSE()
    MESSAGE(STATUS "[SelfCheck] Host System: Unknown")
ENDIF(WIN32)

# Set GoogleTest version
SET(GTEST_VER "1.14.0")

# Load FetchContent Module
INCLUDE(FetchContent)

# Auto fetch GoogleTest source code
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v${GTEST_VER}.zip
        DOWNLOAD_EXTRACT_TIMESTAMP true
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
# set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

ADD_SUBDIRECTORY(src/serf)
ADD_SUBDIRECTORY(test)