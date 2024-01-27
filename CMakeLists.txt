CMAKE_MINIMUM_REQUIRED(VERSION 3.15)

PROJECT(SerfNative)

SET(CMAKE_CXX_STANDARD 17)

# SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O3")

# Set include directories
INCLUDE_DIRECTORIES(inc)
INCLUDE_DIRECTORIES(src/compare/include/deflate)
INCLUDE_DIRECTORIES(src/compare/include/lz4)

# Scan all source code files
AUX_SOURCE_DIRECTORY(src SRC1)
AUX_SOURCE_DIRECTORY(src/utils SRC2)
AUX_SOURCE_DIRECTORY(src/compressor SRC3)
AUX_SOURCE_DIRECTORY(src/decompressor SRC4)
AUX_SOURCE_DIRECTORY(src/compare/lib/deflate SRC5)
AUX_SOURCE_DIRECTORY(src/compare/lib/lz4 SRC6)

# Compile Serf as a library
ADD_LIBRARY(SerfNative-lib STATIC ${SRC1} ${SRC2} ${SRC3} ${SRC4} ${SRC5} ${SRC6})

# Compile test source code
ADD_EXECUTABLE(SerfNative test/TestLz4.cpp)
# ADD_EXECUTABLE(SerfNative test/TestDeflate.cpp)
# ADD_EXECUTABLE(SerfNative test/TestSerf.cpp)

# Link Serf to test program
TARGET_LINK_LIBRARIES(SerfNative PRIVATE SerfNative-lib)