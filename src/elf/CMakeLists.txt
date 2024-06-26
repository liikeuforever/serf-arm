include_directories(${PROJECT_SOURCE_DIR}/src)

file(GLOB_RECURSE LIB_SRC *.cpp *.c)

add_library(elf SHARED ${LIB_SRC})