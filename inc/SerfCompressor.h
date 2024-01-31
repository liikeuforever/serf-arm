#ifndef SERFNATIVE_SERFCOMPRESSOR_H
#define SERFNATIVE_SERFCOMPRESSOR_H

#include "SerfXORCompressor.h"

#include <cfloat>
#include <iostream>
#include <cstring>

#include "Double.h"

typedef unsigned long b64;

class SerfCompressor {
private:
    SerfXORCompressor *xor_compressor = new SerfXORCompressor();
    int compressedSizeInBits = 0;
    int numberOfValues = 0;
    double storedCompressionRatio = 0;
    int fAlpha;
    double maxDiff;
    double storedErasedDoubleValue = NAN;
    b64 storedErasedLongValue = Double::doubleToULongBits(NAN);

public:
    explicit SerfCompressor(int alpha);

    ~SerfCompressor();

    void addValue(double v);

    long getCompressedSizeInBits() const;

    std::vector<char> getBytes();

    void close();

    void refresh();
};


#endif //SERFNATIVE_SERFCOMPRESSOR_H
