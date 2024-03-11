#include "serf/decompressor/SerfXORDecompressor.h"

SerfXORDecompressor::SerfXORDecompressor() {
    in = nullptr;
}

void SerfXORDecompressor::initLeadingRepresentation() {
    unsigned int num = in->readInt(5);
    if (num == 0) {
        num = 32;
    }
    leadingBitsPerValue = PostOfficeSolver::positionLength2Bits[num];
    leadingRepresentation = std::vector<int>(num);
    for (int i = 0; i < num; i++) {
        leadingRepresentation[i] = static_cast<int>(in->readInt(6));
    }
}

void SerfXORDecompressor::initTrailingRepresentation() {
    uint32_t num = in->readInt(5);
    if (num == 0) {
        num = 32;
    }
    trailingBitsPerValue = PostOfficeSolver::positionLength2Bits[num];
    trailingRepresentation = std::vector<int>(num);
    for (int i = 0; i < num; i++) {
        trailingRepresentation[i] = static_cast<int>(in->readInt(6));
    }
}

void SerfXORDecompressor::setBytes(char *data, size_t data_size) {
    in = new InputBitStream(reinterpret_cast<uint8_t *>(data), data_size);
}

void SerfXORDecompressor::refresh() {
    first = true;
    endOfStream = false;
}

void SerfXORDecompressor::nextValue() {
    b64 value;
    int centerBits;

    if (in->readInt(1) == 1) {
        // case 1
        centerBits = 64 - storedLeadingZeros - storedTrailingZeros;

        value = in->readLong(centerBits) << storedTrailingZeros;
        value = storedVal ^ value;
        endOfStream = (value == Double::doubleToULongBits(NAN));
        storedVal = value;
    } else if (in->readInt(1) == 0) {
        // case 00
        uint32_t leadAndTrail = in->readInt(leadingBitsPerValue + trailingBitsPerValue);
        uint32_t lead = leadAndTrail >> trailingBitsPerValue;
        uint32_t trail = ~(0xffffffff << trailingBitsPerValue) & leadAndTrail;
        storedLeadingZeros = leadingRepresentation[lead];
        storedTrailingZeros = trailingRepresentation[trail];
        centerBits = 64 - storedLeadingZeros - storedTrailingZeros;

        value = in->readLong(centerBits) << storedTrailingZeros;
        value = storedVal ^ value;
        endOfStream = (value == Double::doubleToULongBits(NAN));
        storedVal = value;
    }
}

void SerfXORDecompressor::next() {
    if (first) {
        if (in->readBit() == 1) {
            initLeadingRepresentation();
            initTrailingRepresentation();
        }
        first = false;
        uint32_t trailingZeros = in->readInt(7);
        if (trailingZeros < 64) {
            storedVal = ((in->readLong(63 - trailingZeros) << 1) + 1) << trailingZeros;
        } else {
            storedVal = 0;
        }
        endOfStream = (storedVal == Double::doubleToULongBits(NAN));
    } else {
        nextValue();
    }
}

double SerfXORDecompressor::readValue() {
    next();
    return Double::UlongBitsToDouble(storedVal);
}

bool SerfXORDecompressor::available() const {
    return !endOfStream;
}

SerfXORDecompressor::~SerfXORDecompressor() {
    delete in;
}
