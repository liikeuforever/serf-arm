#ifndef DEFLATE_COMPRESSOR_H
#define DEFLATE_COMPRESSOR_H

#include <stdexcept>

#include "deflate/deflate.h"
#include "serf/utils/array.h"

class DeflateCompressor {
private:
    const int BLOCK_SIZE = 1000;
    int ret;
    z_stream strm;
    Array<unsigned char> compress_pack = Array<unsigned char>(BLOCK_SIZE * static_cast<int>(sizeof(double)));

public:
    DeflateCompressor();

    ~DeflateCompressor();

    void addValue(double v);

    void close();

    Array<unsigned char> getBytes();

    long getCompressedSizeInBits();
};

#endif //DEFLATE_COMPRESSOR_H
