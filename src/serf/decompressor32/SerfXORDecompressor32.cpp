#include "SerfXORDecompressor32.h"

std::vector<float> SerfXORDecompressor32::decompress(const Array<uint8_t> &bs) {
    in->setBuffer(bs);
    updateFlagAndPositionsIfNeeded();
    std::vector<float> values;
    uint32_t value;
    while ((value = readValue()) != Float::floatToIntBits(Float::NaN)) {
        values.emplace_back(Float::intBitsToFloat(value));
        storedVal = value;
    }
    return values;
}

uint32_t SerfXORDecompressor32::readValue() {
    uint32_t value = storedVal;
    int centerBits;

    if (equalWin) {
        if (in->readInt(1) == 0) {
            if (in->readInt(1) != 1) {
                // case 00
                int leadAndTrail = static_cast<int>(in->readInt(leadingBitsPerValue + trailingBitsPerValue));
                int lead = leadAndTrail >> trailingBitsPerValue;
                int trail = ~(0xffff << trailingBitsPerValue) & leadAndTrail;
                storedLeadingZeros = leadingRepresentation[lead];
                storedTrailingZeros = trailingRepresentation[trail];
            }
            centerBits = 32 - storedLeadingZeros - storedTrailingZeros;
            value = in->readInt(centerBits) << storedTrailingZeros;
            value = storedVal ^ value;
        }
    } else {
        if (in->readInt(1) == 1) {
            // case 1
            centerBits = 32 - storedLeadingZeros - storedTrailingZeros;

            value = in->readInt(centerBits) << storedTrailingZeros;
            value = storedVal ^ value;
        } else if (in->readInt(1) == 0) {
            // case 00
            int leadAndTrail = static_cast<int>(in->readInt(leadingBitsPerValue + trailingBitsPerValue));
            int lead = leadAndTrail >> trailingBitsPerValue;
            int trail = ~(0xffff << trailingBitsPerValue) & leadAndTrail;
            storedLeadingZeros = leadingRepresentation[lead];
            storedTrailingZeros = trailingRepresentation[trail];
            centerBits = 32 - storedLeadingZeros - storedTrailingZeros;

            value = in->readInt(centerBits) << storedTrailingZeros;
            value = storedVal ^ value;
        }
    }
    return value;
}

void SerfXORDecompressor32::updateFlagAndPositionsIfNeeded() {
    equalWin = in->readBit() == 1;
    if (in->readBit() == 1) {
        updateLeadingRepresentation();
        updateTrailingRepresentation();
    }
}

void SerfXORDecompressor32::updateLeadingRepresentation() {
    int num = static_cast<int>(in->readInt(4));
    if (num == 0) {
        num = 16;
    }
    leadingBitsPerValue = PostOfficeSolver32::positionLength2Bits[num];
    leadingRepresentation = Array<int>(num);
    for (int i = 0; i < num; i++) {
        leadingRepresentation[i] = static_cast<int>(in->readInt(5));
    }
}

void SerfXORDecompressor32::updateTrailingRepresentation() {
    int num = static_cast<int>(in->readInt(4));
    if (num == 0) {
        num = 16;
    }
    trailingBitsPerValue = PostOfficeSolver32::positionLength2Bits[num];
    trailingRepresentation = Array<int>(num);
    for (int i = 0; i < num; i++) {
        trailingRepresentation[i] = static_cast<int>(in->readInt(5));
    }
}
