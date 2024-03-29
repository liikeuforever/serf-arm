#ifndef SERF_TRANSACTION_H
#define SERF_TRANSACTION_H

#include <vector>
#include <list>
#include <thread>
#include <omp.h>

#include "serf/compressor/TorchSerfXORCompressor.h"
#include "serf/decompressor/TorchSerfXORDecompressor.h"

class SerfTransaction {
private:
    int compressor_array_len;
    TorchSerfXORCompressor **compressor_array;

public:
    SerfTransaction(int d1, int d2, int d3, int d4);

    std::vector<std::vector<uint8_t>> compress_tensor(std::vector<double> flat_tensor);
};

#endif //SERF_TRANSACTION_H
