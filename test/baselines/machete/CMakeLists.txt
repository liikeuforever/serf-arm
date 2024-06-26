include_directories(../../src)

file(GLOB_RECURSE LIB_SRC *.cpp *.c)

set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

add_library(machete SHARED ${LIB_SRC})
