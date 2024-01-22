#include "SerfXORCompressor.h"

int SerfXORCompressor::addValue(long value) {
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

int SerfXORCompressor::close() {
    int thisSize = addValue(Elf64Utils::END_SIGN);
    out.flush();
    if (updatePositions) {
        // we update distribution using the inner info
        leadPositions = PostOfficeSolver::initRoundAndRepresentation(leadDistribution,
                                                                     std::vector<int>(std::begin(leadingRepresentation),
                                                                                      std::end(leadingRepresentation)),
                                                                     std::vector<int>(std::begin(leadingRound),
                                                                                      std::end(leadingRound)));
        leadingBitsPerValue = PostOfficeSolver::positionLength2Bits[leadPositions.size()];

        trailPositions = PostOfficeSolver::initRoundAndRepresentation(trailDistribution, std::vector<int>(
                                                                              std::begin(trailingRepresentation), std::end(trailingRepresentation)),
                                                                      std::vector<int>(std::begin(trailingRound),
                                                                                       std::end(trailingRound)));
        trailingBitsPerValue = PostOfficeSolver::positionLength2Bits[trailPositions.size()];
    }
    writePositions = updatePositions;
    return thisSize;
}

uint8_t * SerfXORCompressor::getOut() {
    return out.getBuffer();
}

void SerfXORCompressor::refresh() {
    out = NewOutputBitStream((int) (((capacity + 1) * 8 + capacity / 8 + 1) * 1.2));
    first = true;
    updatePositions = false;
    leadDistribution.clear();
    trailDistribution.clear();
}

void SerfXORCompressor::setDistribution() {
    this->updatePositions = true;
}

int SerfXORCompressor::numberOfTrailingZeros(long value) {
    int x, y;
    if (value == 0) return 64;
    int n = 63;
    y = (int) value;
    if (y != 0) {
        n = n - 32;
        x = y;
    } else x = (int) (static_cast<unsigned long>(value) >> 32);
    y = x << 16;
    if (y != 0) {
        n = n - 16;
        x = y;
    }
    y = x << 8;
    if (y != 0) {
        n = n - 8;
        x = y;
    }
    y = x << 4;
    if (y != 0) {
        n = n - 4;
        x = y;
    }
    y = x << 2;
    if (y != 0) {
        n = n - 2;
        x = y;
    }
    return n - (static_cast<unsigned int>((x << 1)) >> 31);
}

int SerfXORCompressor::numberOfLeadingZeros(int i) {
    // HD, Count leading 0's
    if (i <= 0)
        return i == 0 ? 32 : 0;
    int n = 31;
    if (i >= 1 << 16) {
        n -= 16;
        i >>= 16;
    }
    if (i >= 1 << 8) {
        n -= 8;
        i >>= 8;
    }
    if (i >= 1 << 4) {
        n -= 4;
        i >>= 4;
    }
    if (i >= 1 << 2) {
        n -= 2;
        i >>= 2;
    }
    return n - (static_cast<unsigned int>(i) >> 1);
}

int SerfXORCompressor::numberOfLeadingZeros(long num) {
    int x = (int) (static_cast<unsigned long>(num) >> 32);
    return x == 0 ? 32 + numberOfLeadingZeros((int) num)
                  : numberOfLeadingZeros(x);
}

int SerfXORCompressor::writeFirst(long value) {
    first = false;
    storedVal = value;
    int trailingZeros = numberOfTrailingZeros(value);
    out.writeInt(trailingZeros, 7);
    if (trailingZeros < 64) {
        out.writeLong(static_cast<unsigned long>(storedVal) >> (trailingZeros + 1), 63 - trailingZeros);
        return 70 - trailingZeros;
    } else {
        return 7;
    }
}

int SerfXORCompressor::compressValue(long value) {
    int thisSize = 0;
    long xorResult = storedVal xor value;

    if (xorResult == 0) {
        // case 01
        out.writeInt(1, 2);
        thisSize += 2;
    } else {
        int leadingCount = __builtin_clzl(xorResult);
        int trailingCount = __builtin_ctzl(xorResult);
        int leadingZeros = leadingRound[leadingCount];
        int trailingZeros = trailingRound[trailingCount];
        ++leadDistribution[leadingCount];
        ++trailDistribution[trailingCount];

        if (leadingZeros >= storedLeadingZeros && trailingZeros >= storedTrailingZeros &&
            (leadingZeros - storedLeadingZeros) + (trailingZeros - storedTrailingZeros) <
            1 + leadingBitsPerValue + trailingBitsPerValue) {
            // case 1
            int centerBits = 64 - storedLeadingZeros - storedTrailingZeros;
            int len = 1 + centerBits;
            if (len > 64) {
                out.writeInt(1, 1);
                out.writeLong(static_cast<unsigned long>(xorResult) >> storedTrailingZeros, centerBits);
            } else {
                out.writeLong((1L << centerBits) | (static_cast<unsigned long>(xorResult) >> storedTrailingZeros),
                              1 + centerBits);
            }
            thisSize += len;
        } else {
            storedLeadingZeros = leadingZeros;
            storedTrailingZeros = trailingZeros;
            int centerBits = 64 - storedLeadingZeros - storedTrailingZeros;

            // case 00
            int len = 2 + leadingBitsPerValue + trailingBitsPerValue + centerBits;
            if (len > 64) {
                out.writeInt((leadingRepresentation[storedLeadingZeros] << trailingBitsPerValue)
                             | trailingRepresentation[storedTrailingZeros],
                             2 + leadingBitsPerValue + trailingBitsPerValue);
                out.writeLong(static_cast<unsigned long>(xorResult) >> storedTrailingZeros, centerBits);
            } else {
                out.writeLong(
                        ((((long) leadingRepresentation[storedLeadingZeros] << trailingBitsPerValue) |
                          trailingRepresentation[storedTrailingZeros]) << centerBits) |
                        (static_cast<unsigned long>(xorResult) >> storedTrailingZeros),
                        len
                );
            }
            thisSize += len;
        }
        storedVal = value;
    }
    return thisSize;
}

SerfXORCompressor::SerfXORCompressor() {

}
