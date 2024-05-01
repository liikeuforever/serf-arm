message(STATUS "[Gorilla] Source Code Dir: " ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(${PROJECT_SOURCE_DIR}/src)

# Scan and collect all source code file
file(GLOB_RECURSE LIB_SRC *.cc *.c)
foreach(F ${LIB_SRC})
    message(STATUS "[Gorilla] Collected source code file: " ${F})
endforeach(F)

set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

add_library(gorilla SHARED ${LIB_SRC})

target_link_libraries(gorilla serf)