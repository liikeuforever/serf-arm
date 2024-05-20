#ifndef NET_SERF_XOR_DECOMPRESSOR_H
#define NET_SERF_XOR_DECOMPRESSOR_H

#include <cstdint>
#include <memory>
#include <limits>

#include "serf/utils/double.h"
#include "serf/utils/input_bit_stream.h"
#include "serf/utils/PostOfficeSolver.h"

class NetSerfXORDecompressor {
private:
    const int BLOCK_SIZE = 1000;
    const long adjustD;

    uint64_t storedVal = Double::DoubleToLongBits(2);
    int storedLeadingZeros = std::numeric_limits<int>::max();
    int storedTrailingZeros = std::numeric_limits<int>::max();
    std::unique_ptr<InputBitStream> in = std::make_unique<InputBitStream>();
    Array<int> leadingRepresentation = {0, 8, 12, 16, 18, 20, 22, 24};
    Array<int> trailingRepresentation = {0, 22, 28, 32, 36, 40, 42, 46};
    int leadingBitsPerValue = 3;
    int trailingBitsPerValue = 3;
    int numberOfValues = 0;
    bool equalWin = false;

public:
    explicit NetSerfXORDecompressor(long adjustD);

    double decompress(Array<uint8_t> &bs);

private:
    uint64_t readValue();

    void nextValue();

    void updateFlagAndPositionsIfNeeded();

    void updateLeadingRepresentation();

    void updateTrailingRepresentation();
};

#endif //NET_SERF_XOR_DECOMPRESSOR_H
