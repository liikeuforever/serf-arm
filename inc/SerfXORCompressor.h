#ifndef SERFNATIVE_SERFXORCOMPRESSOR_H
#define SERFNATIVE_SERFXORCOMPRESSOR_H

#include <vector>
#include <limits>
#include <cfloat>
#include <cmath>

#include "OutputBitStream.h"
#include "PostOfficeSolver.h"

typedef unsigned long b64;

class SerfXORCompressor {
private:
    std::vector<int> leadingRepresentation = {
            0, 0, 0, 0, 0, 0, 0, 0,
            1, 1, 1, 1, 2, 2, 2, 2,
            3, 3, 4, 4, 5, 5, 6, 6,
            7, 7, 7, 7, 7, 7, 7, 7,
            7, 7, 7, 7, 7, 7, 7, 7,
            7, 7, 7, 7, 7, 7, 7, 7,
            7, 7, 7, 7, 7, 7, 7, 7,
            7, 7, 7, 7, 7, 7, 7, 7
    };
    std::vector<int> leadingRound = {
            0, 0, 0, 0, 0, 0, 0, 0,
            8, 8, 8, 8, 12, 12, 12, 12,
            16, 16, 18, 18, 20, 20, 22, 22,
            24, 24, 24, 24, 24, 24, 24, 24,
            24, 24, 24, 24, 24, 24, 24, 24,
            24, 24, 24, 24, 24, 24, 24, 24,
            24, 24, 24, 24, 24, 24, 24, 24,
            24, 24, 24, 24, 24, 24, 24, 24
    };
    std::vector<int> trailingRepresentation = {
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 1, 1,
            1, 1, 1, 1, 2, 2, 2, 2,
            3, 3, 3, 3, 4, 4, 4, 4,
            5, 5, 6, 6, 6, 6, 7, 7,
            7, 7, 7, 7, 7, 7, 7, 7,
            7, 7, 7, 7, 7, 7, 7, 7,
    };
    std::vector<int> trailingRound = {
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 22, 22,
            22, 22, 22, 22, 28, 28, 28, 28,
            32, 32, 32, 32, 36, 36, 36, 36,
            40, 40, 42, 42, 42, 42, 46, 46,
            46, 46, 46, 46, 46, 46, 46, 46,
            46, 46, 46, 46, 46, 46, 46, 46
    };
    std::vector<int> leadDistribution = std::vector<int>(64);
    std::vector<int> trailDistribution = std::vector<int>(64);
    int storedLeadingZeros = std::numeric_limits<int>::max();
    int storedTrailingZeros = std::numeric_limits<int>::max();
    b64 storedVal = 0;
    bool first = true;
    std::vector<int> leadPositions = {0, 8, 12, 16, 18, 20, 22, 24};
    std::vector<int> trailPositions = {0, 22, 28, 32, 36, 40, 42, 46};
    bool updatePositions = false;
    bool writePositions = false;
    int leadingBitsPerValue = 3;
    int trailingBitsPerValue = 3;
    const int capacity = 1000;
    OutputBitStream *out = nullptr;

public:
    SerfXORCompressor();

    ~SerfXORCompressor();

    int addValue(b64 value);

    int close();

    uint8_t * getOut();

    void refresh();

    void setDistribution();

    int writeFirst(b64 value);

    int compressValue(b64 value);
};

#endif //SERFNATIVE_SERFXORCOMPRESSOR_H