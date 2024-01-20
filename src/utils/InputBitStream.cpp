#include "InputBitStream.h"

int InputBitStream::readInt(size_t numBits) {
    int result = 0;
    while (numBits > 0) {
        // Load more data into buffer when bits in buffer are less than `numBits`
        if (bitsAvailable_ == 0) {
            if (index_ < size_) {
                buffer_ = static_cast<unsigned char>(data_[index_]);
                bitsAvailable_ = 8;
                index_++;
            } else {
                // Input data runs out
                break;
            }
        }

        // read one bit from buffer
        int bit = (buffer_ >> (bitsAvailable_ - 1)) & 1;
        // add this bit to result
        result = (result << 1) | bit;
        // update self status
        bitsAvailable_--;
        numBits--;
    }
    return result;
}

long InputBitStream::readLong(size_t numBits) {
    int result = 0;
    while (numBits > 0) {
        // Load more data into buffer when bits in buffer are less than `numBits`
        if (bitsAvailable_ == 0) {
            if (index_ < size_) {
                buffer_ = static_cast<unsigned char>(data_[index_]);
                bitsAvailable_ = 8;
                index_++;
            } else {
                // Input data runs out
                break;
            }
        }

        // read one bit from buffer
        int bit = (buffer_ >> (bitsAvailable_ - 1)) & 1;
        // add this bit to result
        result = (result << 1) | bit;
        // update self status
        bitsAvailable_--;
        numBits--;
    }
    return result;
}

int InputBitStream::readBit() {
    return readInt(1);
}
