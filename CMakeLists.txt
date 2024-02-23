CMAKE_MINIMUM_REQUIRED(VERSION 3.27)

PROJECT(SerfNative)

# Set C++ standard version
SET(CMAKE_CXX_STANDARD 17)

# -O3 Optimization for release version
SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

# Set GoogleTest version
SET(GTEST_VER "1.14.0")

# Set include directories
INCLUDE_DIRECTORIES(inc)

# Load FetchContent Module
INCLUDE(FetchContent)

# Auto fetch GoogleTest source code
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v${GTEST_VER}.zip
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
# set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

# Scan all source code files
AUX_SOURCE_DIRECTORY(src/utils SRC_UTILS)
AUX_SOURCE_DIRECTORY(src/compressor SRC_COMPRESSOR)
AUX_SOURCE_DIRECTORY(src/decompressor SRC_DECOMPRESSOR)
AUX_SOURCE_DIRECTORY(test SRC_TEST)

# Compile Serf as a library
ADD_LIBRARY(SerfNative-lib STATIC ${SRC_UTILS} ${SRC_COMPRESSOR} ${SRC_DECOMPRESSOR})

# Enable test
enable_testing()

# Load GoogleTest module
INCLUDE(GoogleTest)

# Compile test source code
ADD_EXECUTABLE(TestSerf ${SRC_TEST})

# Link Serf library to test program
TARGET_LINK_LIBRARIES(TestSerf PRIVATE SerfNative-lib GTest::gtest_main)

# Discover test
gtest_discover_tests(TestSerf)
Footer
