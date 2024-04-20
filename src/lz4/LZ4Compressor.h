#ifndef LZ4_COMPRESSOR_H
#define LZ4_COMPRESSOR_H

#include <stdexcept>

#include "lz4/lz4frame.h"
#include "serf/utils/Array.h"

class LZ4Compressor {
private:
    const int BLOCK_SIZE = 1000;
    LZ4F_compressionContext_t compression_context;
    size_t rc;
    bool first = true;
    Array<char> compress_frame = Array<char>(static_cast<int>(LZ4F_compressBound(BLOCK_SIZE * sizeof(double), nullptr)));
    size_t pos = 0;

public:
    LZ4Compressor();

    ~LZ4Compressor();

    void addValue(double v);

    void close();

    Array<char> getBytes();
};

#endif // LZ4_COMPRESSOR_H
