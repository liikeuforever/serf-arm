#ifndef TENSOR_XOR_DECOMPRESS_TRANSACTION_H
#define TENSOR_XOR_DECOMPRESS_TRANSACTION_H

#include <vector>
#include <thread>
#include <omp.h>

#include "serf/decompressor/TorchSerfXORDecompressor.h"

class TensorXORDecompressTransaction {
private:
    int decompressor_array_len;
    TorchSerfXORDecompressor **decompressor_array;

public:
    TensorXORDecompressTransaction(int d1, int d2, int d3, int d4);

    std::vector<double> decompress_tensor(std::vector<std::vector<uint8_t>> compress_pack);
};

#endif //TENSOR_XOR_DECOMPRESS_TRANSACTION_H
