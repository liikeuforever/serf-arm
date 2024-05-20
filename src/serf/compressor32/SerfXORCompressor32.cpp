#include "SerfXORCompressor32.h"
#include "serf/utils/Serf32Utils.h"
#include "serf/utils/PostOfficeSolver32.h"

SerfXORCompressor32::SerfXORCompressor32(int capacity, float maxDiff): maxDiff(maxDiff) {
    this->out = std::make_unique<OutputBitStream>(std::floor(((capacity + 1) * 4 + capacity / 4 + 1) * 1.2));
    this->compressedSizeInBits = out->writeInt(0, 2);
}

void SerfXORCompressor32::addValue(float v) {
    uint32_t thisVal;
    // note we cannot let > maxDiff, because kNan - v > maxDiff is always false
    if (std::abs(Float::intBitsToFloat(storedVal) - v) > maxDiff) {
        // in our implementation, we do not consider special cases and overflow case
        thisVal = Serf32Utils::findAppInt(v - maxDiff, v + maxDiff, v, storedVal, maxDiff);
    } else {
        // let current value be the last value, making an XORed value of 0.
        thisVal = storedVal;
    }

    compressedSizeInBits += compressValue(thisVal);
    storedVal = thisVal;
    ++numberOfValues;
}

long SerfXORCompressor32::getCompressedSizeInBits() const {
    return compressedSizeInBits;
}

Array<uint8_t> SerfXORCompressor32::getBytes() {
    return outBuffer;
}

void SerfXORCompressor32::close() {
    compressedSizeInBits += compressValue(Float::floatToIntBits(Float::NaN));
    out->flush();
    outBuffer = Array<uint8_t> (std::ceil(static_cast<double>(compressedSizeInBits) / 8.0));
    uint8_t *buffer = out->getBuffer();
    for (int i = 0; i < std::ceil(static_cast<double>(compressedSizeInBits) / 8.0); ++i) {
        outBuffer[i] = buffer[i];
    }
    out->refresh();
    storedCompressedSizeInBits = compressedSizeInBits;
    compressedSizeInBits = updateFlagAndPositionsIfNeeded();
}

int SerfXORCompressor32::compressValue(uint32_t value) {
    int thisSize = 0;
    uint32_t xorResult = storedVal ^ value;

    if (xorResult == 0) {
        // case 01
        if (equalWin) {
            thisSize += static_cast<int>(out->writeBit(true));
        } else {
            thisSize += static_cast<int>(out->writeInt(1, 2));
        }
        equalVote++;
    } else {
        int leadingCount = __builtin_clz(xorResult);
        int trailingCount = __builtin_ctz(xorResult);
        int leadingZeros = leadingRound[leadingCount];
        int trailingZeros = trailingRound[trailingCount];
        ++leadDistribution[leadingCount];
        ++trailDistribution[trailingCount];

        if (leadingZeros >= storedLeadingZeros && trailingZeros >= storedTrailingZeros &&
            (leadingZeros - storedLeadingZeros) + (trailingZeros - storedTrailingZeros) < 1 + leadingBitsPerValue + trailingBitsPerValue) {
            // case 1
            int centerBits = 32 - storedLeadingZeros - storedTrailingZeros;
            int len;
            if (equalWin) {
                len = 2 + centerBits;
                if (len > 32) {
                    out->writeInt(1, 2);
                    out->writeInt(xorResult >> storedTrailingZeros, centerBits);
                } else {
                    out->writeInt((1 << centerBits) | (xorResult >> storedTrailingZeros), 2 + centerBits);
                }
            } else {
                len = 1 + centerBits;
                if (len > 32) {
                    out->writeInt(1, 1);
                    out->writeInt(xorResult >> storedTrailingZeros, centerBits);
                } else {
                    out->writeInt((1 << centerBits) | (xorResult >> storedTrailingZeros), 1 + centerBits);
                }
            }
            thisSize += len;
            equalVote--;
        } else {
            storedLeadingZeros = leadingZeros;
            storedTrailingZeros = trailingZeros;
            int centerBits = 32 - storedLeadingZeros - storedTrailingZeros;

            // case 00
            int len = 2 + leadingBitsPerValue + trailingBitsPerValue + centerBits;
            if (len > 32) {
                out->writeInt((leadingRepresentation[storedLeadingZeros] << trailingBitsPerValue)
                             | trailingRepresentation[storedTrailingZeros], 2 + leadingBitsPerValue + trailingBitsPerValue);
                out->writeInt(xorResult >> storedTrailingZeros, centerBits);
            } else {
                out->writeInt((((leadingRepresentation[storedLeadingZeros] << trailingBitsPerValue)
                               | trailingRepresentation[storedTrailingZeros]) << centerBits)
                             | (xorResult >> storedTrailingZeros), len);
            }
            thisSize += len;
        }
    }
    return thisSize;
}

int SerfXORCompressor32::updateFlagAndPositionsIfNeeded() {
    int len;
    equalWin = equalVote > 0;
    double thisCompressionRatio = static_cast<double>(compressedSizeInBits) / (numberOfValues * 32.0);
    if (storedCompressionRatio < thisCompressionRatio) {
        // update positions
        Array<int> leadPositions = PostOfficeSolver32::initRoundAndRepresentation(leadDistribution, leadingRepresentation, leadingRound);
        leadingBitsPerValue = PostOfficeSolver32::positionLength2Bits[leadPositions.length()];
        Array<int> trailPositions = PostOfficeSolver32::initRoundAndRepresentation(trailDistribution, trailingRepresentation, trailingRound);
        trailingBitsPerValue = PostOfficeSolver32::positionLength2Bits[trailPositions.length()];
        len = static_cast<int>(out->writeInt(equalWin ? 3 : 1, 2))
              + PostOfficeSolver32::writePositions(leadPositions, out.get())
              + PostOfficeSolver32::writePositions(trailPositions, out.get());
    } else {
        len = static_cast<int>(out->writeInt(equalWin ? 2 : 0, 2));
    }
    equalVote = 0;
    storedCompressionRatio = thisCompressionRatio;
    numberOfValues = 0;
    for (int i = 0; i < 32; ++i) {
        leadDistribution[i] = 0;
        trailDistribution[i] = 0;
    }
    return len;
}
