// Fast and lightweight implementation of bit-level input stream

#ifndef SERFNATIVE_INPUTBITSTREAM_H
#define SERFNATIVE_INPUTBITSTREAM_H

#include <fstream>
#include <bitset>

class InputBitStream {

public:
// read `numBits` bits into an integer and return it
int readInt(size_t numBits) {
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

    InputBitStream(const char* data, size_t size) : data_(data), size_(size), index_(0), buffer_(0), bitsAvailable_(0) {}

// read `numBits` bits into a long and return it
long readLong(size_t numBits) {
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

int readBit() {
    return readInt(1);
}

private:
    const char* data_;
    size_t size_;
    size_t index_;
    unsigned char buffer_;
    size_t bitsAvailable_;
};

#endif //SERFNATIVE_INPUTBITSTREAM_H
