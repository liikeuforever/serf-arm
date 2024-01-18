#ifndef SERFNATIVE_SERFXORCOMPRESSOR_H
#define SERFNATIVE_SERFXORCOMPRESSOR_H

#include <limits>

#include "IntArray.h"

class SerfXORCompressor {
private:
    constexpr static int leadingRepresentation[] = {
            0, 0, 0, 0, 0, 0, 0, 0,
            1, 1, 1, 1, 2, 2, 2, 2,
            3, 3, 4, 4, 5, 5, 6, 6,
            7, 7, 7, 7, 7, 7, 7, 7,
            7, 7, 7, 7, 7, 7, 7, 7,
            7, 7, 7, 7, 7, 7, 7, 7,
            7, 7, 7, 7, 7, 7, 7, 7,
            7, 7, 7, 7, 7, 7, 7, 7
    };
    constexpr static int leadingRound[] = {
            0, 0, 0, 0, 0, 0, 0, 0,
            8, 8, 8, 8, 12, 12, 12, 12,
            16, 16, 18, 18, 20, 20, 22, 22,
            24, 24, 24, 24, 24, 24, 24, 24,
            24, 24, 24, 24, 24, 24, 24, 24,
            24, 24, 24, 24, 24, 24, 24, 24,
            24, 24, 24, 24, 24, 24, 24, 24,
            24, 24, 24, 24, 24, 24, 24, 24
    };
    constexpr static int trailingRepresentation[] = {
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 22, 22,
            22, 22, 22, 22, 28, 28, 28, 28,
            32, 32, 32, 32, 36, 36, 36, 36,
            40, 40, 42, 42, 42, 42, 46, 46,
            46, 46, 46, 46, 46, 46, 46, 46,
            46, 46, 46, 46, 46, 46, 46, 46,
    };
    IntArray leadDistribution = IntArray(64);
    IntArray trailDistribution = IntArray(64);
    int storedLeadingZeros = std::numeric_limits<int>::max();
    int storedTrailingZeros = std::numeric_limits<int>::max();
    long storedVal = 0;
    bool first = true;
    IntArray leadPositions = {0, 8, 12, 16, 18, 20, 22, 24};
    IntArray trailPositions = {0, 22, 28, 32, 36, 40, 42, 46};
    bool updatePositions = false;
    bool writePositions = false;
    int leadingBitsPerValue = 3;
    int trailingBitsPerValue = 3;
    const int capacity = 1000;

    int writeFirst(long value) {
        first = false;
        storedVal = value;
        int trailingZeros = 
    }

public:
    SerfXORCompressor() {
        out = OutputBitStream();
    }

    int addValue(long value) {
        if (first) {
            if (writePositions) {
                return out.writeBit(true)
                       + PostOfficeSolver::writePositions(leadPositions, out)
                       + PostOfficeSolver.writePositions(trailPositions, out)
                       + writeFirst(value);
            } else {
                return out.writeBit(false) + writeFirst(value);
            }
        } else {
            return compressValue(value);
        }
    }


};

#endif //SERFNATIVE_SERFXORCOMPRESSOR_H