// Fast and lightweight implementation of bit-level output stream

#ifndef SERFNATIVE_OUTPUTBITSTREAM_H
#define SERFNATIVE_OUTPUTBITSTREAM_H

#include <vector>

class OutputBitStream {
public:
    OutputBitStream() : buffer_(0), bitsAvailable_(8) {}

    void writeInt(int data, size_t numBits) {
        while (numBits > 0) {
            buffer_ = (buffer_ << 1) | ((data >> (numBits - 1)) & 1);
            bitsAvailable_--;
            numBits--;

            if (bitsAvailable_ == 0) {
                flush();
            }
        }
    }

    int writeBit(bool bit) {
        buffer_ = (buffer_ << 1) | (bit & 1);
        bitsAvailable_--;

        if (bitsAvailable_ == 0) {
            flush();
        }

        return 1;
    }

    void writeLong(long data, size_t numBits) {
        while (numBits > 0) {
            buffer_ = (buffer_ << 1) | ((data >> (numBits - 1)) & 1);
            bitsAvailable_--;
            numBits--;

            if (bitsAvailable_ == 0) {
                flush();
            }
        }
    }

    std::vector<char> getBuffer() {
        flush();
        return output_;
    }

    void flush() {
        output_.push_back(static_cast<char>(buffer_));
        buffer_ = 0;
        bitsAvailable_ = 8;
    }

private:
    unsigned char buffer_;
    size_t bitsAvailable_;
    std::vector<char> output_;
};

#endif //SERFNATIVE_OUTPUTBITSTREAM_H
