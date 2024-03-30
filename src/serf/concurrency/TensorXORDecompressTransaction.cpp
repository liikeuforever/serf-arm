#include "TensorXORDecompressTransaction.h"

TensorXORDecompressTransaction::TensorXORDecompressTransaction(int d1, int d2, int d3, int d4): decompressor_array_len(d1 * d2 * d3 * d4) {
    auto init = new TorchSerfXORDecompressor * [decompressor_array_len];
    for (int i = 0; i < decompressor_array_len; ++i) {
        init[i] = new TorchSerfXORDecompressor(0);
    }
    decompressor_array = init;
    omp_set_num_threads(static_cast<int>(std::thread::hardware_concurrency()) * 2);
}

std::vector<double> TensorXORDecompressTransaction::decompress_tensor(std::vector<std::vector<uint8_t>> compress_pack) {
    std::vector<double> result(compress_pack.size());
    #pragma omp parallel for
    for (int i = 0; i < decompressor_array_len; ++i) {
        result[i] = decompressor_array[i]->decompress(compress_pack[i]);
    }
    return result;
}
