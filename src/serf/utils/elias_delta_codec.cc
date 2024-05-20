#include "elias_delta_codec.h"

int elias_delta_codec::Encode(int64_t number, OutputBitStream *output_bit_stream_ptr) {
    int compressed_size_in_bits = 0;
    int32_t len;
    int32_t length_of_len;
    if (number <= 11) {
        len = 1 + (int) std::floor(kLogTable[number] / kLogTable[2]);
    } else {
        len = 1 + (int) std::floor(std::log(number) / kLogTable[2]);
    }
    if (len <= 11) {
        length_of_len = (int) std::floor(kLogTable[len] / kLogTable[2]);
    } else {
        length_of_len = (int) std::floor(std::log(len) / kLogTable[2]);
    }
    int total_len = length_of_len + length_of_len + len;
    if (total_len <= 64) {
        compressed_size_in_bits += static_cast<int>(output_bit_stream_ptr->writeLong(((int64_t) len << (len - 1)) |
                                                                                     (number & ~(0xffffffffffffffffL << (len - 1))),
                                                                                     total_len));
    } else {
        compressed_size_in_bits += static_cast<int>(output_bit_stream_ptr->writeInt(0, length_of_len));
        compressed_size_in_bits += static_cast<int>(output_bit_stream_ptr->writeInt(len, length_of_len + 1));
        compressed_size_in_bits += static_cast<int>(output_bit_stream_ptr->writeLong(number, len - 1));
    }
    return compressed_size_in_bits;
}

int64_t elias_delta_codec::Decode(InputBitStream *input_bit_stream_ptr) {
    uint64_t num = 1;
    int32_t len = 1;
    int32_t length_of_len = 0;
    while (input_bit_stream_ptr->readBit() == 0)
        length_of_len++;
    len <<= length_of_len;
    len |= static_cast<int32_t>(input_bit_stream_ptr->readInt(length_of_len));
    num <<= (len - 1);
    num |= input_bit_stream_ptr->readLong(len - 1);
    return *reinterpret_cast<int64_t *>(&num);
}
