# Scan and collect all test file
FILE(GLOB_RECURSE TEST_SRC *.cpp *.c)
MESSAGE(STATUS "[Serf-Test] Collected test file: " ${TEST_SRC})

enable_testing()

INCLUDE(GoogleTest)

ADD_EXECUTABLE(serf_test ${TEST_SRC})

target_link_libraries(serf_test PRIVATE serf_lib GTest::gtest_main)

# Discover test
gtest_discover_tests(serf_test)