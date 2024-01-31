#ifndef SERFNATIVE_SERFCONSTRUCTCOMPRESSOR_H
#define SERFNATIVE_SERFCONSTRUCTCOMPRESSOR_H

#include <vector>
#include <cmath>

#include "SerfXORCompressor.h"
#include "Double.h"

class SerfConstructCompressor {
private:
    int compressedSizeInBits = 0;
    int numberOfValues = 0;
    double storedCompressionRatio = 0;
    double maxDiff;
    unsigned long storedAppLongValue = Double::doubleToLongBits(NAN);
    long bw[64]{};
    SerfXORCompressor xor_compressor;

public:
    explicit SerfConstructCompressor(double maxDiff) {
        this->maxDiff = maxDiff;

        bw[0] = 1;
        for (int i = 1; i < 64; i++) {
            bw[i] = bw[i - 1] << 1;
        }
    }

    void addValue(double v);
    long getCompressedSizeInBits();
    std::vector<char> getBytes();
    void close();
    void refresh();

private:
    long findAppLong(double minDouble, double maxDouble, long sign, double original, long lastLong, double maxDiff);

};


#endif //SERFNATIVE_SERFCONSTRUCTCOMPRESSOR_H
