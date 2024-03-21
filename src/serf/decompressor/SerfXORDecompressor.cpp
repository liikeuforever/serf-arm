#include "SerfXORDecompressor.h"
#include "serf/utils/PostOfficeSolver.h"

std::vector<double> SerfXORDecompressor::decompress(const Array<uint8_t> &bs) {
    in->setBuffer(bs);
    updateFlagAndPositionsIfNeeded();
    std::vector<double> values(1024);
    uint64_t value;
    while ((value = readValue()) != Serf64Utils::END_SIGN) {
        values.push_back(Double::longBitsToDouble(value) - static_cast<double>(adjustD));
        storedVal = value;
    }
    return values;
}

uint64_t SerfXORDecompressor::readValue() {
    uint64_t value = storedVal;
    uint32_t centerBits;

    if (equalWin) {
        if (in->readInt(1) == 0) {
            if (in->readInt(1) != 1) {
                // case 00
                uint32_t leadAndTrail = in->readInt(leadingBitsPerValue + trailingBitsPerValue);
                uint32_t lead = leadAndTrail >> trailingBitsPerValue;
                uint32_t trail = ~(0xffffffffU << trailingBitsPerValue) & leadAndTrail;
                storedLeadingZeros = leadingRepresentation[static_cast<int>(lead)];
                storedTrailingZeros = trailingRepresentation[static_cast<int>(trail)];
            }
            centerBits = 64 - storedLeadingZeros - storedTrailingZeros;
            value = in->readLong(centerBits) << storedTrailingZeros;
            value = storedVal ^ value;
        }
    } else {
        if (in->readInt(1) == 1) {
            // case 1
            centerBits = 64 - storedLeadingZeros - storedTrailingZeros;

            value = in->readLong(centerBits) << storedTrailingZeros;
            value = storedVal ^ value;
        } else if (in->readInt(1) == 0) {
            // case 00
            uint32_t leadAndTrail = in->readInt(leadingBitsPerValue + trailingBitsPerValue);
            uint32_t lead = leadAndTrail >> trailingBitsPerValue;
            uint32_t trail = ~(0xffffffffU << trailingBitsPerValue) & leadAndTrail;
            storedLeadingZeros = leadingRepresentation[static_cast<int>(lead)];
            storedTrailingZeros = trailingRepresentation[static_cast<int>(trail)];
            centerBits = 64 - storedLeadingZeros - storedTrailingZeros;

            value = in->readLong(centerBits) << storedTrailingZeros;
            value = storedVal ^ value;
        }
    }
    return value;
}

void SerfXORDecompressor::updateFlagAndPositionsIfNeeded() {
    equalWin = in->readBit() == 1;
    if (in->readBit() == 1) {
        updateLeadingRepresentation();
        updateTrailingRepresentation();
    }
}

void SerfXORDecompressor::updateLeadingRepresentation() {
    int num = static_cast<int>(in->readInt(5));
    if (num == 0) {
        num = 32;
    }
    leadingBitsPerValue = PostOfficeSolver::positionLength2Bits[num];
    leadingRepresentation = Array<int>(num);
    for (int i = 0; i < num; i++) {
        leadingRepresentation[i] = static_cast<int>(in->readInt(6));
    }
}

void SerfXORDecompressor::updateTrailingRepresentation() {
    int num = static_cast<int>(in->readInt(5));
    if (num == 0) {
        num = 32;
    }
    trailingBitsPerValue = PostOfficeSolver::positionLength2Bits[num];
    trailingRepresentation = Array<int>(num);
    for (int i = 0; i < num; i++) {
        trailingRepresentation[i] = static_cast<int>(in->readInt(6));
    }
}
