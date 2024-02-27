message(STATUS "[Serf] Source Code Dir: " ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Scan and collect all source code file
file(GLOB_RECURSE LIB_SRC *.cpp *.c)
foreach(F ${LIB_SRC})
    message(STATUS "[Serf] Collected source code file: " ${F})
endforeach(F)

set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

add_library(serf STATIC ${LIB_SRC})
