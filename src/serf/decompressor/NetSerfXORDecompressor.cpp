#include "NetSerfXORDecompressor.h"

NetSerfXORDecompressor::NetSerfXORDecompressor(long adjustD): adjustD(adjustD) {}

double NetSerfXORDecompressor::decompress(Array<uint8_t> &bs) {
    in->SetBuffer(bs);
    return Double::LongBitsToDouble(readValue()) - adjustD;
}

uint64_t NetSerfXORDecompressor::readValue() {
    // empty read 4 bits for getting rid of transmit header
    in->ReadInt(4);
    if (numberOfValues >= BLOCK_SIZE) {
        updateFlagAndPositionsIfNeeded();
    }
    nextValue();
    ++numberOfValues;
    return storedVal;
}

void NetSerfXORDecompressor::nextValue() {
    uint64_t value;
    int centerBits;

    if (equalWin) {
        if (in->ReadInt(1) == 0) {
            if (in->ReadInt(1) != 1) {
                // case 00
                int leadAndTrail = in->ReadInt(
                        leadingBitsPerValue + trailingBitsPerValue);
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
            int leadAndTrail = in->ReadInt(
                    leadingBitsPerValue + trailingBitsPerValue);
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

void NetSerfXORDecompressor::updateFlagAndPositionsIfNeeded() {
    equalWin = in->ReadBit() == 1;
    if (in->ReadBit() == 1) {
        updateLeadingRepresentation();
        updateTrailingRepresentation();
    }
    numberOfValues = 0;
}

void NetSerfXORDecompressor::updateLeadingRepresentation() {
    int num = in->ReadInt(5);
    if (num == 0) {
        num = 32;
    }
    leadingBitsPerValue = PostOfficeSolver::kPositionLength2Bits[num];
    leadingRepresentation = Array<int>(num);
    for (int i = 0; i < num; i++) {
        leadingRepresentation[i] = in->ReadInt(6);
    }
}

void NetSerfXORDecompressor::updateTrailingRepresentation() {
    int num = in->ReadInt(5);
    if (num == 0) {
        num = 32;
    }
    trailingBitsPerValue = PostOfficeSolver::kPositionLength2Bits[num];
    trailingRepresentation = Array<int>(num);
    for (int i = 0; i < num; i++) {
        trailingRepresentation[i] = in->ReadInt(6);
    }
}
