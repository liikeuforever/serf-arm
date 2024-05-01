#include "ChimpDecompressor.h"

std::vector<double> ChimpDecompressor::decompress(const Array<uint8_t> &bs) {
    input_bit_stream->setBuffer(bs);
    uint32_t double_count = input_bit_stream->readInt(32);
    std::vector<double> values(double_count);
    storedValues[0] = input_bit_stream->readLong(64);
    values.push_back(Double::longBitsToDouble(storedValues[0]));
    for (int i = 1; i < double_count; ++i) {
        uint32_t flag = input_bit_stream->readInt(2);
        uint32_t tmp, fill, index, significantBits;
        switch (flag) {
            case 3:
                tmp = input_bit_stream->readInt(3);
                storedLeadingZeros = leadingRep[tmp];
                delta = input_bit_stream->readLong(64 - storedLeadingZeros);
                values.push_back(values[i - 1]);
                break;
            case 2:
                break;
            case 1:
                break;
        }
    }
    return values;
}
