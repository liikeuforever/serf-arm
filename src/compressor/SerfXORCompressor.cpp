#include "SerfXORCompressor.h"
#include "Double.h"

int SerfXORCompressor::addValue(b64 value) {
    if (first) {
        if (writePositions) {
            return out->writeBit(true)
                   + PostOfficeSolver::writePositions(leadPositions, out)
                   + PostOfficeSolver::writePositions(trailPositions, out)
                   + writeFirst(value);
        } else {
            return out->writeBit(false) + writeFirst(value);
        }
    } else {
        return compressValue(value);
    }
}

int SerfXORCompressor::close() {
    int thisSize = addValue(Double::doubleToLongBits(NAN));
    out->flush();
    if (updatePositions) {
        // we update distribution using the inner info
        leadPositions = PostOfficeSolver::initRoundAndRepresentation(leadDistribution, leadingRepresentation, leadingRound);
        leadingBitsPerValue = PostOfficeSolver::positionLength2Bits[leadPositions.size()];

        trailPositions = PostOfficeSolver::initRoundAndRepresentation(trailDistribution, trailingRepresentation, trailingRound);
        trailingBitsPerValue = PostOfficeSolver::positionLength2Bits[trailPositions.size()];
    }
    writePositions = updatePositions;
    return thisSize;
}

uint8_t *SerfXORCompressor::getOut() {
    out->flush();
    return out->getBuffer();
}

void SerfXORCompressor::refresh() {
    delete out;
    out = new OutputBitStream((int) (((capacity + 1) * 8 + capacity / 8 + 1) * 1.2));
    first = true;
    updatePositions = false;
    leadDistribution.clear();
    trailDistribution.clear();
}

void SerfXORCompressor::setDistribution() {
    this->updatePositions = true;
}

int SerfXORCompressor::writeFirst(b64 value) {
    first = false;
    storedVal = value;
    int trailingZeros = __builtin_ctzl(value);
    out->writeInt(trailingZeros, 7);
    if (trailingZeros < 64) {
        out->writeLong(storedVal >> (trailingZeros + 1), 63 - trailingZeros);
        return 70 - trailingZeros;
    } else {
        return 7;
    }
}

int SerfXORCompressor::compressValue(b64 value) {
    int thisSize = 0;
    b64 xorResult = storedVal ^ value;

    if (xorResult == 0) {
        // case 01
        out->writeInt(1, 2);
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
                out->writeInt(1, 1);
                out->writeLong(xorResult >> storedTrailingZeros, centerBits);
            } else {
                out->writeLong((1UL << centerBits) | (xorResult >> storedTrailingZeros),
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
                out->writeInt((leadingRepresentation[storedLeadingZeros] << trailingBitsPerValue)
                             | trailingRepresentation[storedTrailingZeros],
                             2 + leadingBitsPerValue + trailingBitsPerValue);
                out->writeLong(static_cast<unsigned long>(xorResult) >> storedTrailingZeros, centerBits);
            } else {
                out->writeLong(
                        (((static_cast<b64>(leadingRepresentation[storedLeadingZeros]) << trailingBitsPerValue) |
                          trailingRepresentation[storedTrailingZeros]) << centerBits) |
                        (xorResult >> storedTrailingZeros),
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
    out = new OutputBitStream((int) (((capacity + 1) * 8 + capacity / 8 + 1) * 1.2));
}

SerfXORCompressor::~SerfXORCompressor() {
    delete out;
}
