cmake_minimum_required(VERSION 3.27)
project(SerfNative)

set(CMAKE_CXX_STANDARD 17)

include_directories(inc)

add_library(SerfNative-lib src/utils/Elf64Utils.cpp)

add_executable(SerfNative main.cpp)
