include_directories(../../src)

# Scan and collect all source code file
file(GLOB_RECURSE LIB_SRC *.cpp *.c)

add_library(deflate SHARED ${LIB_SRC})
