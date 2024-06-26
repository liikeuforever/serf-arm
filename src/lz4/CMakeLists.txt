include_directories(${PROJECT_SOURCE_DIR}/src)

# Scan and collect all source code file
file(GLOB_RECURSE LIB_SRC *.cpp *.c)

add_library(lz4 SHARED ${LIB_SRC})
