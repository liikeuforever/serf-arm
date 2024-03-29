message(STATUS "[Serf] Source Code Dir: " ${CMAKE_CURRENT_SOURCE_DIR})

find_package(OpenMP REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")

include_directories(${PROJECT_SOURCE_DIR}/src)

# Scan and collect all source code file
file(GLOB_RECURSE LIB_SRC *.cpp *.c)
foreach(F ${LIB_SRC})
    message(STATUS "[Serf] Collected source code file: " ${F})
endforeach(F)

set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

add_library(serf SHARED ${LIB_SRC})
target_link_libraries(serf PRIVATE OpenMP::OpenMP_CXX)