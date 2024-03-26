#include "NetSerfXORDecompressor.h"

NetSerfXORDecompressor::NetSerfXORDecompressor(long adjustD): adjustD(adjustD) {}

double NetSerfXORDecompressor::decompress(Array<uint8_t> &bs) {
    in->setBuffer(bs);
    return Double::longBitsToDouble(readValue()) - adjustD;
}

uint64_t NetSerfXORDecompressor::readValue() {
    // empty read 4 bits for getting rid of transmit header
    in->readInt(4);
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
        if (in->readInt(1) == 0) {
            if (in->readInt(1) != 1) {
                // case 00
                int leadAndTrail = in->readInt(leadingBitsPerValue + trailingBitsPerValue);
                int lead = leadAndTrail >> trailingBitsPerValue;
                int trail = ~(0xffffffff << trailingBitsPerValue) & leadAndTrail;
                storedLeadingZeros = leadingRepresentation[lead];
                storedTrailingZeros = trailingRepresentation[trail];
            }
            centerBits = 64 - storedLeadingZeros - storedTrailingZeros;
            value = in->readLong(centerBits) << storedTrailingZeros;
            value = storedVal ^ value;
            storedVal = value;
        }
    } else {
        if (in->readInt(1) == 1) {
            // case 1
            centerBits = 64 - storedLeadingZeros - storedTrailingZeros;

            value = in->readLong(centerBits) << storedTrailingZeros;
            value = storedVal ^ value;
            storedVal = value;
        } else if (in->readInt(1) == 0) {
            // case 00
            int leadAndTrail = in->readInt(leadingBitsPerValue + trailingBitsPerValue);
            int lead = leadAndTrail >> trailingBitsPerValue;
            int trail = ~(0xffffffff << trailingBitsPerValue) & leadAndTrail;
            storedLeadingZeros = leadingRepresentation[lead];
            storedTrailingZeros = trailingRepresentation[trail];
            centerBits = 64 - storedLeadingZeros - storedTrailingZeros;

            value = in->readLong(centerBits) << storedTrailingZeros;
            value = storedVal ^ value;
            storedVal = value;
        }
    }
}

void NetSerfXORDecompressor::updateFlagAndPositionsIfNeeded() {
    equalWin = in->readBit() == 1;
    if (in->readBit() == 1) {
        updateLeadingRepresentation();
        updateTrailingRepresentation();
    }
    numberOfValues = 0;
}

void NetSerfXORDecompressor::updateLeadingRepresentation() {
    int num = in->readInt(5);
    if (num == 0) {
        num = 32;
    }
    leadingBitsPerValue = PostOfficeSolver::positionLength2Bits[num];
    leadingRepresentation = Array<int>(num);
    for (int i = 0; i < num; i++) {
        leadingRepresentation[i] = in->readInt(6);
    }
}

void NetSerfXORDecompressor::updateTrailingRepresentation() {
    int num = in->readInt(5);
    if (num == 0) {
        num = 32;
    }
    trailingBitsPerValue = PostOfficeSolver::positionLength2Bits[num];
    trailingRepresentation = Array<int>(num);
    for (int i = 0; i < num; i++) {
        trailingRepresentation[i] = in->readInt(6);
    }
}
