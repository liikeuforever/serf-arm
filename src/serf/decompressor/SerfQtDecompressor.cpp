#include "SerfQtDecompressor.h"

std::vector<double> SerfQtDecompressor::Decompress() {
    pre_value_ = 2;
    std::vector<double> decompressed_value_list;
    while (block_size_--) decompressed_value_list.emplace_back(NextValue());
    return decompressed_value_list;
}

double SerfQtDecompressor::NextValue() {
    int64_t decodeValue = ZigZagCodec::decode(EliasDeltaCodec::decode(input_bit_stream_.get()) - 1);
    double recoverValue = pre_value_ + 2 * max_diff_ * static_cast<double>(decodeValue);
    pre_value_ = recoverValue;
    return recoverValue;
}
