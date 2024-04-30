#include "ChimpDecompressor.h"

std::vector<double> ChimpDecompressor::decompress(const Array<uint8_t> &bs) {
    input_bit_stream->setBuffer(bs);
    std::vector<double> values;

}
