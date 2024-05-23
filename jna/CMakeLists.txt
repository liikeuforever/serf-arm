message(STATUS "[JNA-Serf] Source Code Dir: " ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(${PROJECT_SOURCE_DIR}/src)

set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

add_library(javaSerf SHARED serf_jna_agent.cc)

target_link_libraries(javaSerf PRIVATE serf)