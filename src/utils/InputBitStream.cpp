#include "InputBitStream.h"

int InputBitStream::readInt(size_t numBits) {
    if (numBits <= 0 || numBits > 32) {
        return -1;
    }
    int result = 0;
    for (int i = 0; i < numBits; ++i) {
        if (bitsAvailable_ == 0) {
            if (index_ >= size_) {
                return -1;
            }

            buffer_ = data_[index_++];
            bitsAvailable_ = 8;
        }

        result = (result << 1) | ((buffer_ >> (bitsAvailable_ - 1)) & 1);
        bitsAvailable_--;
    }
    return result;
}

long InputBitStream::readLong(size_t numBits) {
    if (numBits <= 0 || numBits > 64) {
        return -1;
    }
    long result = 0;
    for (int i = 0; i < numBits; ++i) {
        if (bitsAvailable_ == 0) {
            if (index_ >= size_) {
                return -1;
            }

            buffer_ = data_[index_++];
            bitsAvailable_ = 8;
        }
        result = (result << 1) | ((buffer_ >> (bitsAvailable_ - 1)) & 1);
        bitsAvailable_--;
    }
    return result;
}

int InputBitStream::readBit() {
    return readInt(1);
}
