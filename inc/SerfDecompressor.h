#ifndef SERFNATIVE_SERFDECOMPRESSOR_H
#define SERFNATIVE_SERFDECOMPRESSOR_H

#include "SerfXORDecompressor.h"

#include <vector>

class SerfDecompressor {
public:
    SerfXORDecompressor *xor_decompressor = nullptr;

public:
    SerfDecompressor();

    ~SerfDecompressor();

    std::vector<double> decompress();

    double nextValue();

    void setBytes(char *data, size_t data_size);

    void refresh();

    bool available();
};

#endif //SERFNATIVE_SERFDECOMPRESSOR_H
