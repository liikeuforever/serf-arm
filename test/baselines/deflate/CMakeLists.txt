include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Scan and collect all source code file
file(GLOB_RECURSE LIB_SRC *.cpp *.c *.cc)

add_library(deflate SHARED ${LIB_SRC})
