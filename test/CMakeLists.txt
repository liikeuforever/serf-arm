# Scan and collect all test file
file(GLOB_RECURSE TEST_SRC *.cpp *.c)
foreach(F ${TEST_SRC})
    message(STATUS "[Serf-Test] Collected source code file: " ${F})
endforeach(F)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)

enable_testing()

include(GoogleTest)

add_executable(serf_test ${TEST_SRC})

target_link_libraries(serf_test PRIVATE serf GTest::gtest_main)

# Discover test
gtest_discover_tests(serf_test)
