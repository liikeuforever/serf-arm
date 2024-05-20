#ifndef SERF_XOR_DECOMPRESSOR_32_H
#define SERF_XOR_DECOMPRESSOR_32_H

#include <limits>
#include <memory>
#include <vector>

#include "serf/utils/Serf32Utils.h"
#include "serf/utils/PostOfficeSolver32.h"
#include "serf/utils/float.h"
#include "serf/utils/input_bit_stream.h"

class SerfXORDecompressor32 {
private:
    uint32_t storedVal = Float::FloatToIntBits(2);
    int storedLeadingZeros = std::numeric_limits<int>::max();
    int storedTrailingZeros = std::numeric_limits<int>::max();
    std::unique_ptr<InputBitStream> in = std::make_unique<InputBitStream>();
    Array<int> leadingRepresentation = {0, 8, 12, 16};
    Array<int> trailingRepresentation = {0, 16};
    int leadingBitsPerValue = 2;
    int trailingBitsPerValue = 1;
    bool equalWin = false;

public:
    SerfXORDecompressor32() = default;

    std::vector<float> decompress(const Array<uint8_t> &bs);

private:
    uint32_t readValue();
    void updateFlagAndPositionsIfNeeded();
    void updateLeadingRepresentation();
    void updateTrailingRepresentation();
};

#endif //SERF_XOR_DECOMPRESSOR_32_H
