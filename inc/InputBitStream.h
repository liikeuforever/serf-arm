// Fast and lightweight implementation of bit-level input stream

#ifndef SERFNATIVE_INPUTBITSTREAM_H
#define SERFNATIVE_INPUTBITSTREAM_H

#include <fstream>

class InputBitStream {
public:
    InputBitStream(const char *data, size_t size) : data_(data), size_(size), index_(0), buffer_(0),
                                                    bitsAvailable_(0) {}

    int readInt(size_t numBits);

    long readLong(size_t numBits);

    int readBit();

private:
    const char *data_;
    size_t size_;
    size_t index_;
    unsigned char buffer_;
    size_t bitsAvailable_;
};

#endif //SERFNATIVE_INPUTBITSTREAM_H
