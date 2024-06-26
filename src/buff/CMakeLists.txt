include_directories(${PROJECT_SOURCE_DIR}/src)

file(GLOB_RECURSE LIB_SRC *.cc *.c)

add_library(buff SHARED ${LIB_SRC})
