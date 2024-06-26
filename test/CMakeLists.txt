cmake_minimum_required(VERSION 3.20)

project(PerfTest)

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

include_directories(${CMAKE_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR})

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

# Scan and collect all test file
file(GLOB_RECURSE TEST_SRC *_test.cc)

enable_testing()

include(GoogleTest)

foreach(F ${TEST_SRC})
    get_filename_component(TEST_PROG ${F} NAME_WE)
    add_executable(${TEST_PROG} ${F})
    string(REPLACE "_test" "" LIB_NAME ${TEST_PROG})
    target_link_libraries(${TEST_PROG} PRIVATE ${LIB_NAME} GTest::gtest_main)
    gtest_discover_tests(${TEST_PROG})
endforeach(F)

add_executable(PerformanceProgram Perf.cc)
target_link_libraries(PerformanceProgram PRIVATE alp buff chimp deflate elf fpc gorilla lzw lz77 lz4 machete sz3c serf
        GTest::gtest_main)
gtest_discover_tests(PerformanceProgram)

add_executable(TransClient transmission/prog/client.cc)
add_executable(TransServer transmission/prog/server.cc)
target_link_libraries(TransClient serf)
target_link_libraries(TransServer serf)