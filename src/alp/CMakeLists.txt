cmake_minimum_required(VERSION 3.24)

set(CMAKE_CXX_STANDARD 17)
project(ALP)

add_compile_options(-fPIC)

#-----------------------------------------------------------------------------------------------------------------------
include(FetchContent)
include(CheckCXXCompilerFlag)
include(CMakePrintHelpers)
include(CTest)

# CMAKE_SOURCE_DIR: ----------------------------------------------------------------------------------------------------
add_compile_definitions(CMAKE_SOURCE_DIR="${CMAKE_SOURCE_DIR}")

# Include --------------------------------------------------------------------------------------------------------------
include_directories(include)

# Src: -------------------------------------------------------------------------------------------------------
add_subdirectory(src)

