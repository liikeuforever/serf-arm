include_directories(../../src)

file(GLOB_RECURSE LIB_SRC *.cc *.c)

add_library(buff SHARED ${LIB_SRC})
