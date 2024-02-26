MESSAGE(STATUS [Serf] "Source Code Dir: " ${CMAKE_CURRENT_SOURCE_DIR})

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

# Scan and collect all source code file
FILE(GLOB_RECURSE ALL_SRC *.cpp *.c)

