CMAKE_MINIMUM_REQUIRED(VERSION 3.27)

PROJECT(SerfNative)

SET(CMAKE_CXX_STANDARD 17)

# Set include directories
INCLUDE_DIRECTORIES(inc)

# Scan all source code files
AUX_SOURCE_DIRECTORY(src SRC1)
AUX_SOURCE_DIRECTORY(src/utils SRC2)
AUX_SOURCE_DIRECTORY(src/compressor SRC3)
AUX_SOURCE_DIRECTORY(src/decompressor SRC4)

# Compile Serf as a library
ADD_LIBRARY(SerfNative-lib STATIC ${SRC1} ${SRC2} ${SRC3} ${SRC4})

# Compile test source code
ADD_EXECUTABLE(SerfNative test/TestSerf.cpp)

# Link Serf to test program
TARGET_LINK_LIBRARIES(SerfNative PRIVATE SerfNative-lib)