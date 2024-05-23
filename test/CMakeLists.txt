# Scan and collect all test file
file(GLOB_RECURSE TEST_SRC *_test.cc)
foreach(F ${TEST_SRC})
    message(STATUS "[Test] Collected source code file: " ${F})
endforeach(F)

include_directories(${PROJECT_SOURCE_DIR}/src)

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
target_link_libraries(PerformanceProgram PRIVATE alp buff chimp deflate elf fpc gorilla lzw lz77 lz4 machete sz serf GTest::gtest_main)
gtest_discover_tests(PerformanceProgram)