#ifndef SERF_SERFDECOMPRESSOR_H
#define SERF_SERFDECOMPRESSOR_H

#include <vector>
#include "serf/decompressor/SerfXORDecompressor.h"

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

#endif
