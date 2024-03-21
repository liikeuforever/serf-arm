#ifndef SERF_XOR_COMPRESSOR_H
#define SERF_XOR_COMPRESSOR_H

#include <cstdint>
#include <cmath>
#include "serf/utils/Double.h"
#include "serf/utils/OutputBitStream.h"
#include "serf/compressor/ICompressor.h"
#include "serf/utils/Serf64Utils.h"

class SerfXORCompressor : ICompressor {
public:
    SerfXORCompressor(int capacity, double maxDiff, uint64_t adjustD);

    ~SerfXORCompressor();

    void addValue(double v) override;

    long getCompressedSizeInBits() override;

    const char *getBytes() override;

    void close() override;

private:
    const double maxDiff;
    uint64_t storedVal = Double::doubleToLongBits(2);
    long compressedSizeInBits;
    long storedCompressedSizeInBits = 0;
    OutputBitStream *out;
    char *outBuffer;
    int numberOfValues = 0;
    double storedCompressionRatio = 0;
    int equalVote = 0;
    bool equalWin = false;
    const uint64_t adjustD;
    int leadingRepresentation[64] = {
            0, 0, 0, 0, 0, 0, 0, 0,
            1, 1, 1, 1, 2, 2, 2, 2,
            3, 3, 4, 4, 5, 5, 6, 6,
            7, 7, 7, 7, 7, 7, 7, 7,
            7, 7, 7, 7, 7, 7, 7, 7,
            7, 7, 7, 7, 7, 7, 7, 7,
            7, 7, 7, 7, 7, 7, 7, 7,
            7, 7, 7, 7, 7, 7, 7, 7
    };
    int leadingRound[64] = {
            0, 0, 0, 0, 0, 0, 0, 0,
            8, 8, 8, 8, 12, 12, 12, 12,
            16, 16, 18, 18, 20, 20, 22, 22,
            24, 24, 24, 24, 24, 24, 24, 24,
            24, 24, 24, 24, 24, 24, 24, 24,
            24, 24, 24, 24, 24, 24, 24, 24,
            24, 24, 24, 24, 24, 24, 24, 24,
            24, 24, 24, 24, 24, 24, 24, 24
    };
    int trailingRepresentation[64] = {
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 1, 1,
            1, 1, 1, 1, 2, 2, 2, 2,
            3, 3, 3, 3, 4, 4, 4, 4,
            5, 5, 6, 6, 6, 6, 7, 7,
            7, 7, 7, 7, 7, 7, 7, 7,
            7, 7, 7, 7, 7, 7, 7, 7,
    };
    int trailingRound[64] = {
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 22, 22,
            22, 22, 22, 22, 28, 28, 28, 28,
            32, 32, 32, 32, 36, 36, 36, 36,
            40, 40, 42, 42, 42, 42, 46, 46,
            46, 46, 46, 46, 46, 46, 46, 46,
            46, 46, 46, 46, 46, 46, 46, 46,
    };
    int leadingBitsPerValue = 3;
    int trailingBitsPerValue = 3;
    std::array<int, 64> leadDistribution;
    std::array<int, 64> trailDistribution;
    int storedLeadingZeros = std::numeric_limits<int>::max();
    int storedTrailingZeros = std::numeric_limits<int>::max();

    int compressValue(uint64_t value);

    int updateFlagAndPositionsIfNeeded();
};

#endif //SERF_XOR_COMPRESSOR_H