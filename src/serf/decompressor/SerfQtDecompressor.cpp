#include "SerfQtDecompressor.h"

std::vector<double> SerfQtDecompressor::Decompress() {
    pre_value_ = 2;
    std::vector<double> decompressedValueList;
    while (block_size_--) {
        double value = NextValue();
        decompressedValueList.emplace_back(value);
    }
    return decompressedValueList;
}

double SerfQtDecompressor::NextValue() {
    double returnValue;
    int64_t decodeValue = ZigZagCodec::decode(EliasDeltaCodec::decode(*input_bit_stream_) - 1);
    double recoverValue = pre_value_ + 2 * max_diff_ * static_cast<double>(decodeValue);
    pre_value_ = recoverValue;
    returnValue = recoverValue;
    return returnValue;
}
