# Temporarily disable sprintz due to compilation errors
# TODO: Fix syntax errors in sprintz_xff_rle.cpp

# cmake_minimum_required(VERSION 3.20)

# project(Sprintz)

# # Set C++ standard version
# set(CMAKE_CXX_STANDARD 11)

# # -O3 Optimization for release version
# set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

# # Set parallel compilation level as 4
# set(CMAKE_BUILD_PARALLEL_LEVEL 4)

# include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# # Scan and collect all source code file
# file(GLOB_RECURSE LIB_SRC *.cpp *.cc)

# add_library(sprintz STATIC ${LIB_SRC})

# # Architecture-specific compile options
# if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64" OR CMAKE_SYSTEM_PROCESSOR MATCHES "amd64")
#     target_compile_options(sprintz PRIVATE -march=native -mavx2 -mbmi -mbmi2)
#     target_compile_definitions(sprintz PRIVATE USE_AVX2=1)
# elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "arm64" OR CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64")
#     # Use ARM NEON instead of AVX2
#     target_compile_options(sprintz PRIVATE -march=native)
#     target_compile_definitions(sprintz PRIVATE USE_NEON=1)
# else()
#     # Fallback to portable code
#     target_compile_definitions(sprintz PRIVATE USE_PORTABLE=1)
# endif()
