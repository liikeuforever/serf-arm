include_directories(${PROJECT_SOURCE_DIR}/src)

# Scan and collect all source code file
file(GLOB_RECURSE LIB_SRC *.cc *.c)

add_library(lz77 SHARED ${LIB_SRC})
