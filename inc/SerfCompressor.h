#ifndef SERFNATIVE_SERFCOMPRESSOR_H
#define SERFNATIVE_SERFCOMPRESSOR_H

#include "SerfXORCompressor.h"

#include <iostream>
#include <cstring>

class SerfCompressor {
private:
    SerfXORCompressor xor_compressor;
    int compressedSizeInBits = 0;
    int numberOfValues = 0;
    double storedCompressionRatio = 0;
    int fAlpha;
    double maxDiff;
    double storedErasedDoubleValue = longBitsToDouble(0x7ff8000000000000L);
    long storedErasedLongValue = 0x7ff8000000000000L;

public:
    SerfCompressor(int alpha);

    long doubleToLongBits(double value);

    double longBitsToDouble(long bits);

    void addValue(double v);

    long getCompressedSizeInBits();

    std::vector<char> getBytes();

    void close();

    void refresh();
};


#endif //SERFNATIVE_SERFCOMPRESSOR_H
