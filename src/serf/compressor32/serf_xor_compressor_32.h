#ifndef SERF_XOR_COMPRESSOR_32_H
#define SERF_XOR_COMPRESSOR_32_H

#include <cmath>
#include <cstdint>
#include <memory>

#include "serf/utils/output_bit_stream.h"
#include "serf/utils/float.h"
#include "serf/utils/array.h"

class SerfXORCompressor32 {
private:
    const float maxDiff;
    uint32_t storedVal = Float::FloatToIntBits(2);

    long compressedSizeInBits;
    long storedCompressedSizeInBits = 0;

    std::unique_ptr<OutputBitStream> out;
    Array<uint8_t> outBuffer = Array<uint8_t>(0);

    int numberOfValues = 0;
    double storedCompressionRatio = 0;

    int equalVote = 0;
    bool equalWin = false;

    Array<int> leadingRepresentation = {
            0, 0, 0, 0, 0, 0, 0, 0,
            1, 1, 1, 1, 2, 2, 2, 2,
            3, 3, 3, 3, 3, 3, 3, 3,
            3, 3, 3, 3, 3, 3, 3, 3,
    };
    Array<int> leadingRound = {
            0, 0, 0, 0, 0, 0, 0, 0,
            8, 8, 8, 8, 12, 12, 12, 12,
            16, 16, 16, 16, 16, 16, 16, 16,
            16, 16, 16, 16, 16, 16, 16, 16
    };
    Array<int> trailingRepresentation = {
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1
    };
    Array<int> trailingRound = {
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            16, 16, 16, 16, 16, 16, 16, 16,
            16, 16, 16, 16, 16, 16, 16, 16
    };

    int leadingBitsPerValue = 2;
    int trailingBitsPerValue = 1;
    Array<int> leadDistribution = Array<int>(32);
    Array<int> trailDistribution = Array<int>(32);
    int storedLeadingZeros = std::numeric_limits<int>::max();
    int storedTrailingZeros = std::numeric_limits<int>::max();

public:
    SerfXORCompressor32(int capacity, float maxDiff);

    void addValue(float v);

    long getCompressedSizeInBits() const;

    Array<uint8_t> getBytes();

    void close();

private:
    int compressValue(uint32_t value);

    int updateFlagAndPositionsIfNeeded();
};

#endif //SERF_XOR_COMPRESSOR_32_H