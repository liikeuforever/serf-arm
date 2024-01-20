#include "OutputBitStream.h"

int OutputBitStream::writeInt(int data, size_t numBits) {
    while (numBits > 0) {
        buffer_ = (buffer_ << 1) | ((data >> (numBits - 1)) & 1);
        bitsAvailable_--;
        numBits--;

        if (bitsAvailable_ == 0) {
            flush();
        }
    }
    return numBits;
}

int OutputBitStream::writeBit(bool bit) {
    buffer_ = (buffer_ << 1) | (bit & 1);
    bitsAvailable_--;

    if (bitsAvailable_ == 0) {
        flush();
    }

    return 1;
}

void OutputBitStream::writeLong(long data, size_t numBits) {
    while (numBits > 0) {
        buffer_ = (buffer_ << 1) | ((data >> (numBits - 1)) & 1);
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

