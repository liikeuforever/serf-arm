// Fast and lightweight implementation of bit-level output stream

#ifndef SERFNATIVE_OUTPUTBITSTREAM_H
#define SERFNATIVE_OUTPUTBITSTREAM_H

#include <vector>

class OutputBitStream {
public:
    OutputBitStream() : buffer_(0), bitsAvailable_(8) {}
    void writeInt(int data, size_t numBits);
    int writeBit(bool bit);
    void writeLong(long data, size_t numBits);
    std::vector<char> getBuffer();
    void flush();

private:
    unsigned char buffer_;
    size_t bitsAvailable_;
    std::vector<char> output_;
};

#endif //SERFNATIVE_OUTPUTBITSTREAM_H
