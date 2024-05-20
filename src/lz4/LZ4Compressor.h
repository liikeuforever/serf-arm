#ifndef LZ4_COMPRESSOR_H
#define LZ4_COMPRESSOR_H

#include <stdexcept>

#include "lz4/lz4frame.h"
#include "serf/utils/array.h"

class LZ4Compressor {
private:
    const int BLOCK_SIZE = 1000;
    LZ4F_compressionContext_t compression_context;
    size_t rc;
    Array<char> compress_frame = Array<char>(static_cast<int>(LZ4F_compressBound(BLOCK_SIZE * sizeof(double), nullptr)));
    size_t written_bytes = 0;

public:
    LZ4Compressor();

    ~LZ4Compressor();

    void addValue(double v);

    void close();

    Array<char> getBytes();

    long getCompressedSizeInBits();
};

#endif // LZ4_COMPRESSOR_H
