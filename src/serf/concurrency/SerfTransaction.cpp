#include "SerfTransaction.h"

SerfTransaction::SerfTransaction(int d1, int d2, int d3, int d4): compressor_array_len(d1 * d2 * d3 * d4) {
    auto init = new TorchSerfXORCompressor * [compressor_array_len];
    for (int i = 0; i < compressor_array_len; ++i) {
        init[i] = new TorchSerfXORCompressor(1.0E-4, 0);
    }
    compressor_array = init;
    omp_set_num_threads(static_cast<int>(std::thread::hardware_concurrency()) * 2);
}

//std::vector<std::vector<uint8_t>> SerfTransaction::compress_tensor(std::vector<double> flat_tensor) {
//    std::vector<std::vector<uint8_t>> result(flat_tensor.size());
//    #pragma omp parallel for
//    for (int i = 0; i < compressor_array_len; ++i) {
//        Array<uint8_t> compressed = compressor_array[i]->compress(flat_tensor[i]);
//        std::vector<uint8_t> compressed_v(compressed.length);
//        for (int j = 0; j < compressed.length; ++j) {
//            compressed_v.push_back(compressed[j]);
//        }
//        result[i] = compressed_v;
//    }
//    return result;
//}

std::vector<std::vector<uint8_t>> SerfTransaction::compress_tensor(std::vector<double> flat_tensor) {
    std::vector<std::vector<uint8_t>> result(flat_tensor.size());
    #pragma omp parallel for
    for (int i = 0; i < compressor_array_len; ++i) {
        result[i] = compressor_array[i]->compress_vector(flat_tensor[i]);
    }
    return result;
}
