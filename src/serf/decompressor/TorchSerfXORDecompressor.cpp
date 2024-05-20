#include "TorchSerfXORDecompressor.h"

double TorchSerfXORDecompressor::decompress(const Array<uint8_t>& input) {
    in->SetBuffer(input);
    return Double::LongBitsToDouble(readValue()) - adjustD;
}

double TorchSerfXORDecompressor::decompress(const std::vector<uint8_t> &input) {
    in->SetBuffer(input);
    return Double::LongBitsToDouble(readValue()) - adjustD;
}

uint64_t TorchSerfXORDecompressor::readValue() {
    if (numberOfValues >= BLOCK_SIZE) {
        updateFlagAndPositionsIfNeeded();
    }
    nextValue();
    ++numberOfValues;
    return storedVal;
}

void TorchSerfXORDecompressor::nextValue() {
    uint64_t value;
    int centerBits;

    if (equalWin) {
        if (in->ReadInt(1) == 0) {
            if (in->ReadInt(1) != 1) {
                // case 00
                int leadAndTrail = static_cast<int>(in->ReadInt(
                        leadingBitsPerValue + trailingBitsPerValue));
                int lead = leadAndTrail >> trailingBitsPerValue;
                int trail = ~(0xffffffff << trailingBitsPerValue) & leadAndTrail;
                storedLeadingZeros = leadingRepresentation[lead];
                storedTrailingZeros = trailingRepresentation[trail];
            }
            centerBits = 64 - storedLeadingZeros - storedTrailingZeros;
            value = in->ReadLong(centerBits) << storedTrailingZeros;
            value = storedVal ^ value;
            storedVal = value;
        }
    } else {
        if (in->ReadInt(1) == 1) {
            // case 1
            centerBits = 64 - storedLeadingZeros - storedTrailingZeros;

            value = in->ReadLong(centerBits) << storedTrailingZeros;
            value = storedVal ^ value;
            storedVal = value;
        } else if (in->ReadInt(1) == 0) {
            // case 00
            int leadAndTrail = static_cast<int>(in->ReadInt(
                    leadingBitsPerValue + trailingBitsPerValue));
            int lead = leadAndTrail >> trailingBitsPerValue;
            int trail = ~(0xffffffff << trailingBitsPerValue) & leadAndTrail;
            storedLeadingZeros = leadingRepresentation[lead];
            storedTrailingZeros = trailingRepresentation[trail];
            centerBits = 64 - storedLeadingZeros - storedTrailingZeros;

            value = in->ReadLong(centerBits) << storedTrailingZeros;
            value = storedVal ^ value;
            storedVal = value;
        }
    }
}

void TorchSerfXORDecompressor::updateFlagAndPositionsIfNeeded() {
    equalWin = in->ReadBit() == 1;
    if (in->ReadBit() == 1) {
        updateLeadingRepresentation();
        updateTrailingRepresentation();
    }
    numberOfValues = 0;
}

void TorchSerfXORDecompressor::updateLeadingRepresentation() {
    int num = static_cast<int>(in->ReadInt(5));
    if (num == 0) {
        num = 32;
    }
    leadingBitsPerValue = PostOfficeSolver::positionLength2Bits[num];
    leadingRepresentation = Array<int>(num);
    for (int i = 0; i < num; i++) {
        leadingRepresentation[i] = static_cast<int>(in->ReadInt(6));
    }
}

void TorchSerfXORDecompressor::updateTrailingRepresentation() {
    int num = static_cast<int>(in->ReadInt(5));
    if (num == 0) {
        num = 32;
    }
    trailingBitsPerValue = PostOfficeSolver::positionLength2Bits[num];
    trailingRepresentation = Array<int>(num);
    for (int i = 0; i < num; i++) {
        trailingRepresentation[i] = static_cast<int>(in->ReadInt(6));
    }
}
