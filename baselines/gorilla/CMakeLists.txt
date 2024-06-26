include_directories(../../src)

# Scan and collect all source code file
file(GLOB_RECURSE LIB_SRC *.cc *.c)

add_library(gorilla SHARED ${LIB_SRC})

target_link_libraries(gorilla serf)