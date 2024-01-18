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
            0, 0, 0, 0, 0, 0, 22, 22,
            22, 22, 22, 22, 28, 28, 28, 28,
            32, 32, 32, 32, 36, 36, 36, 36,
            40, 40, 42, 42, 42, 42, 46, 46,
            46, 46, 46, 46, 46, 46, 46, 46,
            46, 46, 46, 46, 46, 46, 46, 46,
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

    int numberOfTrailingZeros(long value) {
        if (value == 0) {
            return sizeof(value) * 8;
        }

        int count = 0;
        while ((value & 1) == 0) {
            value >>= 1;
            ++count;
        }

        return count;
    }

    int writeFirst(long value) {
        first = false;
        storedVal = value;
        int trailingZeros = numberOfTrailingZeros(value);
        out.writeInt(trailingZeros, 7);
        if (trailingZeros < 64) {
            out.writeLong(storedVal >> (trailingZeros + 1), 63 - trailingZeros);
            return 70 - trailingZeros;
        } else {
            return 7;
        }
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
                       + PostOfficeSolver::writePositions(trailPositions, out)
                       + writeFirst(value);
            } else {
                return out.writeBit(false) + writeFirst(value);
            }
        } else {
            return compressValue(value);
        }
    }

    int close() {
        int thisSize = addValue(Elf64Utils::END_SIGN);
        out.flush();
        if (updatePositions) {
            // we update distribution using the inner info
            leadPositions = PostOfficeSolver::initRoundAndRepresentation(leadDistribution, std::vector<int>(std::begin(leadingRepresentation), std::end(leadingRepresentation)), std::vector<int>(std::begin(leadingRound), std::end(leadingRound)));
            leadingBitsPerValue = PostOfficeSolver::positionLength2Bits[leadPositions.size()];

            trailPositions = PostOfficeSolver::initRoundAndRepresentation(trailDistribution, std::vector<int>(std::begin(trailingRepresentation), std::end(trailingRepresentation)), std::vector<int>(std::begin(trailingRound), std::end(trailingRound)));
            trailingBitsPerValue = PostOfficeSolver::positionLength2Bits[trailPositions.size()];
        }
        writePositions = updatePositions;
        return thisSize;
    }
};

#endif //SERFNATIVE_SERFXORCOMPRESSOR_H