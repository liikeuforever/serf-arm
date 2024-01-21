#ifndef SERFNATIVE_SERFXORDECOMPRESSOR_H
#define SERFNATIVE_SERFXORDECOMPRESSOR_H

#include <limits>
#include <cstring>

#include "InputBitStream.h"
#include "PostOfficeSolver.h"
#include "Elf64Utils.h"

class SerfXORDecompressor {
private:
    long storedVal = 0;
    int storedLeadingZeros = std::numeric_limits<int>::max();
    int storedTrailingZeros = std::numeric_limits<int>::max();
    bool first = true;
    bool endOfStream = false;
    InputBitStream in = InputBitStream(nullptr, 0);
    std::vector<int> leadingRepresentation = {0, 8, 12, 16, 18, 20, 22, 24};
    std::vector<int> trailingRepresentation = {0, 22, 28, 32, 36, 40, 42, 46};
    int leadingBitsPerValue = 3;
    int trailingBitsPerValue = 3;

public:

    SerfXORDecompressor() {
        in = InputBitStream(nullptr, 0);
    }

    void initLeadingRepresentation() {
        int num = in.readInt(5);
        if (num == 0) {
            num = 32;
        }
        leadingBitsPerValue = PostOfficeSolver::positionLength2Bits[num];
        leadingRepresentation = std::vector<int>(num);
        for (int i = 0; i < num; i++) {
            leadingRepresentation[i] = in.readInt(6);
        }
    }

    void initTrailingRepresentation() {
        int num = in.readInt(5);
        if (num == 0) {
            num = 32;
        }
        trailingBitsPerValue = PostOfficeSolver::positionLength2Bits[num];
        trailingRepresentation = std::vector<int>(num);
        for (int i = 0; i < num; i++) {
            trailingRepresentation[i] = in.readInt(6);
        }
    }

    void setBytes(char *data, size_t data_size) {
        in = InputBitStream(data, data_size);
    }

    void refresh() {
        first = true;
        endOfStream = false;
    }

    void nextValue() {
            long value;
            int centerBits;

            if (in.readInt(1) == 1) {
                // case 1
                centerBits = 64 - storedLeadingZeros - storedTrailingZeros;

                value = in.readLong(centerBits) << storedTrailingZeros;
                value = storedVal ^ value;
                endOfStream = value == Elf64Utils::END_SIGN;
                storedVal = value;
            } else if (in.readInt(1) == 0) {
                // case 00
                int leadAndTrail = in.readInt(leadingBitsPerValue + trailingBitsPerValue);
                int lead = leadAndTrail >> trailingBitsPerValue;
                int trail = ~(0xffffffff << trailingBitsPerValue) & leadAndTrail;
                storedLeadingZeros = leadingRepresentation[lead];
                storedTrailingZeros = trailingRepresentation[trail];
                centerBits = 64 - storedLeadingZeros - storedTrailingZeros;

                value = in.readLong(centerBits) << storedTrailingZeros;
                value = storedVal ^ value;
                endOfStream = value == Elf64Utils::END_SIGN;
                storedVal = value;
            }
    }

    void next() {
        if (first) {
            if (in.readBit() == 1) {
                initLeadingRepresentation();
                initTrailingRepresentation();
            }
            first = false;
            int trailingZeros = in.readInt(7);
            if (trailingZeros < 64) {
                storedVal = ((in.readLong(63 - trailingZeros) << 1) + 1) << trailingZeros;
            } else {
                storedVal = 0;
            }
            endOfStream = storedVal == Elf64Utils::END_SIGN;
        } else {
            nextValue();
        }
    }

    double readValue() {
        next();
        return longBitsToDouble(storedVal);
    }

    bool available() const {
        return (storedVal != Elf64Utils::END_SIGN);
    }

private:
    double longBitsToDouble(long bits) {
        double result;
        std::memcpy(&result, &bits, sizeof(bits));
        return result;
    }
};


#endif //SERFNATIVE_SERFXORDECOMPRESSOR_H
