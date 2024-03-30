#ifndef TENSOR_XOR_COMPRESS_TRANSACTION_H
#define TENSOR_XOR_COMPRESS_TRANSACTION_H

#include <vector>
#include <thread>
#include <omp.h>

#include "serf/compressor/TorchSerfXORCompressor.h"

class TensorXORCompressTransaction {
private:
    int compressor_array_len;
    TorchSerfXORCompressor **compressor_array;

public:
    TensorXORCompressTransaction(int d1, int d2, int d3, int d4);

    std::vector<std::vector<uint8_t>> compress_tensor(std::vector<double> flat_tensor);
};

#endif //TENSOR_XOR_COMPRESS_TRANSACTION_H
