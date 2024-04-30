#include "ChimpCompressor.h"

ChimpCompressor::ChimpCompressor(int capacity) {
    // Preserve 64 bytes to avoid overflow
    output_bit_stream = std::make_unique<OutputBitStream>((capacity + 8) * sizeof(double));
    // Use 32bits to store length information
    size += output_bit_stream->writeInt(capacity, 32);
}

void ChimpCompressor::addValue(double v) {
    if (first) {
        first = false;
        size += output_bit_stream->writeLong(Double::doubleToLongBits(v), 64);
        storedValues[current] = Double::doubleToLongBits(v);
        indices[((int) v) & setLsb] = index;
    } else {
        int key = (int) v & setLsb;
        uint64_t delta;
        int previousIndex;
        int trailingZeros = 0;
        int curIndex = indices[key];
        if ((index - curIndex) < previousValues) {
            delta = Double::doubleToLongBits(v) ^ storedValues[curIndex % previousValues];
            trailingZeros = __builtin_ctzl(delta);
            if (trailingZeros > threshold) {
                previousIndex = curIndex % previousValues;
            } else {
                previousIndex = index % previousValues;
                delta = storedValues[previousIndex] ^ Double::doubleToLongBits(v);
            }
        } else {
            previousIndex = index % previousValues;
            delta = storedValues[previousIndex] ^ Double::doubleToLongBits(v);
        }

        if (delta == 0) {
            size += output_bit_stream->writeInt(previousIndex, flagZeroSize);
            storedLeadingZeros = 65;
        } else {
            int leadingZeros = leadingRnd[__builtin_clzl(delta)];

            if (trailingZeros > threshold) {
                int32_t significantBits = 64 - leadingZeros - trailingZeros;
                size += output_bit_stream->writeInt(((previousValues + previousIndex) << 9) |
                                             (leadingRep[leadingZeros] << 6) |
                                             significantBits, flagOneSize);
                size += output_bit_stream->writeLong(delta >> trailingZeros, significantBits);
                storedLeadingZeros = 65;
            } else if (leadingZeros == storedLeadingZeros) {
                size += output_bit_stream->writeInt(2, 2);
                int32_t significantBits = 64 - leadingZeros;
                size += output_bit_stream->writeLong(delta, significantBits);
            } else {
                storedLeadingZeros = leadingZeros;
                int significantBits = 64 - leadingZeros;
                size += output_bit_stream->writeInt((0x3 << 3) | leadingRep[leadingZeros], 5);
                size += output_bit_stream->writeLong(delta, significantBits);
            }
        }

        current = (current + 1) % previousValues;
        storedValues[current] = Double::doubleToLongBits(v);
        index++;
        indices[key] = index;
    }
}

void ChimpCompressor::close() {
    output_bit_stream->flush();
    uint8_t *data_ptr = output_bit_stream->getBuffer();
    long compress_pack_len_in_bytes = std::ceil(size / 8.0);
    bs = Array<uint8_t>(compress_pack_len_in_bytes);
    for (int i = 0; i < compress_pack_len_in_bytes; ++i) {
        bs[i] = data_ptr[i];
    }
}

long ChimpCompressor::getCompressedSize() {
    return size;
}

Array<uint8_t> ChimpCompressor::getBytes() {
    return bs;
}
