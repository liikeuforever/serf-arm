cmake_minimum_required(VERSION 3.20)

project(Buff)

# Set C++ standard version
set(CMAKE_CXX_STANDARD 17)

# -O3 Optimization for release version
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

# Set parallel compilation level as 4
set(CMAKE_BUILD_PARALLEL_LEVEL 4)

# Find Boost
find_package(Boost REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${Boost_INCLUDE_DIRS})

file(GLOB_RECURSE LIB_SRC *.cc *.c)

add_library(buff SHARED ${LIB_SRC})

# Link Boost libraries
target_link_libraries(buff ${Boost_LIBRARIES})
