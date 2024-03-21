#include "SerfXORCompressor.h"

SerfXORCompressor::SerfXORCompressor(int capacity, double maxDiff, uint64_t adjustD) : maxDiff(maxDiff),
                                                                                       adjustD(adjustD) {
    this->out = new OutputBitStream(((capacity + 1) * 8 + capacity / 8 + 1) * 1.2);
    this->compressedSizeInBits = out->writeInt(0, 2);
}

SerfXORCompressor::~SerfXORCompressor() {
    delete out;
}

void SerfXORCompressor::addValue(double v) {
    uint64_t thisVal;
    // note we cannot let > maxDiff, because NaN - v > maxDiff is always false
    if (abs(Double::longBitsToDouble(storedVal) - adjustD - v) > maxDiff) {
        // in our implementation, we do not consider special cases and overflow case
        double adjustValue = v + adjustD;
        thisVal = Serf64Utils::findAppLong(adjustValue - maxDiff, adjustValue + maxDiff, v, storedVal, maxDiff,
                                           adjustD);
    } else {
        // let current value be the last value, making an XORed value of 0.
        thisVal = storedVal;
    }

    compressedSizeInBits += compressValue(thisVal);
    storedVal = thisVal;
    ++numberOfValues;
}

long SerfXORCompressor::getCompressedSizeInBits() {
    return storedCompressedSizeInBits;
}

const char *SerfXORCompressor::getBytes() {
    return outBuffer;
}

void SerfXORCompressor::close() {
    compressedSizeInBits += compressValue(Serf64Utils::END_SIGN);
    out->flush();
    outBuffer = Arrays.copyOf(out->getBuffer(), (int) ceil(compressedSizeInBits / 8.0));
    out->refresh();
    storedCompressedSizeInBits = compressedSizeInBits;
    compressedSizeInBits = updateFlagAndPositionsIfNeeded();
}

int SerfXORCompressor::compressValue(uint64_t value) {
    int thisSize = 0;
    uint64_t xorResult = storedVal ^ value;

    if (xorResult == 0) {
        // case 01
        if (equalWin) {
            thisSize += out->writeBit(true);
        } else {
            thisSize += out->writeInt(1, 2);
        }
        equalVote++;
    } else {
        int leadingCount = __builtin_clzll(xorResult);
        int trailingCount = __builtin_ctzll(xorResult);
        int leadingZeros = leadingRound[leadingCount];
        int trailingZeros = trailingRound[trailingCount];
        ++leadDistribution[leadingCount];
        ++trailDistribution[trailingCount];

        if (leadingZeros >= storedLeadingZeros && trailingZeros >= storedTrailingZeros &&
            (leadingZeros - storedLeadingZeros) + (trailingZeros - storedTrailingZeros) <
            1 + leadingBitsPerValue + trailingBitsPerValue) {
            // case 1
            int centerBits = 64 - storedLeadingZeros - storedTrailingZeros;
            int len;
            if (equalWin) {
                len = 2 + centerBits;
                if (len > 64) {
                    out->writeInt(1, 2);
                    out->writeLong(xorResult >> storedTrailingZeros, centerBits);
                } else {
                    out->writeLong((1ULL << centerBits) | (xorResult >> storedTrailingZeros), 2 + centerBits);
                }
            } else {
                len = 1 + centerBits;
                if (len > 64) {
                    out->writeInt(1, 1);
                    out->writeLong(xorResult >> storedTrailingZeros, centerBits);
                } else {
                    out->writeLong((1ULL << centerBits) | (xorResult >> storedTrailingZeros), 1 + centerBits);
                }
            }
            thisSize += len;
            equalVote--;
        } else {
            storedLeadingZeros = leadingZeros;
            storedTrailingZeros = trailingZeros;
            int centerBits = 64 - storedLeadingZeros - storedTrailingZeros;

            // case 00
            int len = 2 + leadingBitsPerValue + trailingBitsPerValue + centerBits;
            if (len > 64) {
                out->writeInt((leadingRepresentation[storedLeadingZeros] << trailingBitsPerValue)
                              | trailingRepresentation[storedTrailingZeros],
                              2 + leadingBitsPerValue + trailingBitsPerValue);
                out->writeLong(xorResult >> storedTrailingZeros, centerBits);
            } else {
                out->writeLong(
                        ((((uint64_t) leadingRepresentation[storedLeadingZeros] << trailingBitsPerValue) |
                          trailingRepresentation[storedTrailingZeros]) << centerBits) |
                        (xorResult >> storedTrailingZeros),
                        len
                );
            }
            thisSize += len;
        }
    }
    return thisSize;
}

int SerfXORCompressor::updateFlagAndPositionsIfNeeded() {
    int len;
    equalWin = equalVote > 0;
    double thisCompressionRatio = compressedSizeInBits / (numberOfValues * 64.0);
    if (storedCompressionRatio < thisCompressionRatio) {
        // update positions
        int []
        leadPositions = PostOfficeSolver::initRoundAndRepresentation(leadDistribution, leadingRepresentation,
                                                                     leadingRound);
        leadingBitsPerValue = PostOfficeSolver::positionLength2Bits[leadPositions.length];
        int []
        trailPositions = PostOfficeSolver.initRoundAndRepresentation(trailDistribution, trailingRepresentation,
                                                                     trailingRound);
        trailingBitsPerValue = PostOfficeSolver.positionLength2Bits[trailPositions.length];
        len = out->writeInt(equalWin ? 3 : 1, 2)
              + PostOfficeSolver.writePositions(leadPositions, out)
              + PostOfficeSolver.writePositions(trailPositions, out);
    } else {
        len = out->writeInt(equalWin ? 2 : 0, 2);
    }
    equalVote = 0;
    storedCompressionRatio = thisCompressionRatio;
    numberOfValues = 0;
    Arrays.fill(leadDistribution, 0);
    Arrays.fill(trailDistribution, 0);
    return len;
}
