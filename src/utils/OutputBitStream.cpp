#include "OutputBitStream.h"

int OutputBitStream::writeInt(int data, size_t numBits) {
    int count = static_cast<int>(numBits);
    while (numBits > 0) {
        buffer_ |= (((data >> (numBits - 1)) & 1) << (bitsAvailable_ - 1));
        bitsAvailable_--;
        numBits--;

        if (bitsAvailable_ == 0) {
            flush();
        }
    }
    return count;
}

int OutputBitStream::writeBit(bool bit) {
    buffer_ |= ((bit & 1) << (bitsAvailable_ - 1));
    bitsAvailable_--;

    if (bitsAvailable_ == 0) {
        flush();
    }

    return 1;
}

void OutputBitStream::writeLong(long data, size_t numBits) {
    unsigned long data_tmp = static_cast<unsigned long>(data);
    while (numBits > 0) {
        buffer_ |= (((data_tmp >> (numBits - 1)) & 1) << (bitsAvailable_ - 1));
        bitsAvailable_--;
        numBits--;

        if (bitsAvailable_ == 0) {
            flush();
        }
    }
}

std::vector<char> OutputBitStream::getBuffer() {
    flush();
    return output_;
}

void OutputBitStream::flush() {
    output_.push_back(static_cast<char>(buffer_));
    buffer_ = 0;
    bitsAvailable_ = 8;
}

