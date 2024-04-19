message(STATUS "[LZ4] Source Code Dir: " ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(${PROJECT_SOURCE_DIR}/src)

# Scan and collect all source code file
file(GLOB_RECURSE LIB_SRC *.cpp *.c)
foreach(F ${LIB_SRC})
    message(STATUS "[LZ4] Collected source code file: " ${F})
endforeach(F)

set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

add_library(lz4 SHARED ${LIB_SRC})