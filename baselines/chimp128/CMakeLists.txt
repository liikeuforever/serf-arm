include_directories(../../src)

# Scan and collect all source code file
file(GLOB_RECURSE LIB_SRC *.cpp *.c)

add_library(chimp SHARED ${LIB_SRC})

target_link_libraries(chimp serf)
