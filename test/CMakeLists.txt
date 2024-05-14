# Scan and collect all test file
file(GLOB_RECURSE TEST_SRC *.cpp *.cc *.c)
foreach(F ${TEST_SRC})
    message(STATUS "[Serf-Test] Collected source code file: " ${F})
endforeach(F)

include_directories(${PROJECT_SOURCE_DIR}/src)

enable_testing()

include(GoogleTest)

foreach(F ${TEST_SRC})
    get_filename_component(prjName ${F} NAME_WE)
    add_executable(${prjName} ${F})
    target_link_libraries(${prjName} PRIVATE serf fpc lz4 deflate gorilla chimp elf machete ALP buff lz77 LZW SZ GTest::gtest_main)
    gtest_discover_tests(${prjName})
endforeach(F)