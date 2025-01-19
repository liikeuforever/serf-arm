cmake_minimum_required(VERSION 3.20)

project(Serf-All)

# Print system info
if(WIN32)
    message(STATUS "[SelfCheck] Host System: Windows (32bit)")
    add_definitions(-DWIN32)
elseif(WIN64)
    message(STATUS "[SelfCheck] Host System: Windows (64bit)")
    add_definitions(-DWIN64)
elseif(APPLE)
    message(STATUS "[SelfCheck] Host System: Apple MacOS")
    add_definitions(-D__MACH__)
elseif(UNIX)
    message(STATUS "[SelfCheck] Host System: Linux/Unix/*nix")
    add_definitions(-DUNIX -DLINUX)
else()
    message(STATUS "[SelfCheck] Host System: Unknown")
endif(WIN32)

add_subdirectory(src)
# To run perf programs and unit tests
add_subdirectory(test)

# If Python API and JAVA API are needed
add_subdirectory(pywrapper)
# add_subdirectory(jna)