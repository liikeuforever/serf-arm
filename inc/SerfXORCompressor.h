#ifndef SERFNATIVE_SERFXORCOMPRESSOR_H
#define SERFNATIVE_SERFXORCOMPRESSOR_H

#include <vector>
#include <limits>
#include "PostOfficeSolver.h"
#include "Elf64Utils.h"

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
            0, 0, 0, 0, 0, 0, 1, 1,
            1, 1, 1, 1, 2, 2, 2, 2,
            3, 3, 3, 3, 4, 4, 4, 4,
            5, 5, 6, 6, 6, 6, 7, 7,
            7, 7, 7, 7, 7, 7, 7, 7,
            7, 7, 7, 7, 7, 7, 7, 7,
    };
    constexpr static int trailingRound[] = {
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
    long storedVal = 0;
    bool first = true;
    std::vector<int> leadPositions = {0, 8, 12, 16, 18, 20, 22, 24};
    std::vector<int> trailPositions = {0, 22, 28, 32, 36, 40, 42, 46};
    bool updatePositions = false;
    bool writePositions = false;
    int leadingBitsPerValue = 3;
    int trailingBitsPerValue = 3;
    const int capacity = 1000;
    OutputBitStream out = (int) (((capacity + 1) * 8 + capacity / 8 + 1) * 1.2);

public:
    SerfXORCompressor();

    int addValue(long value);

    int close();

    std::vector<char> getOut();

    void refresh();

    void setDistribution();

    int numberOfTrailingZeros(long value);

    int numberOfLeadingZeros(int i);

    int numberOfLeadingZeros(long num);

    int writeFirst(long value);

    int compressValue(long value);
};

#endif //SERFNATIVE_SERFXORCOMPRESSOR_H