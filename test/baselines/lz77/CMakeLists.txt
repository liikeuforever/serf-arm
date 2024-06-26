include_directories(../../src)

# Scan and collect all source code file
file(GLOB_RECURSE LIB_SRC *.cc *.c)

add_library(lz77 SHARED ${LIB_SRC})
