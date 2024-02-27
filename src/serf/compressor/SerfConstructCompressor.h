#ifndef SERF_SERFCONSTRUCTCOMPRESSOR_H
#define SERF_SERFCONSTRUCTCOMPRESSOR_H

#include <vector>
#include <cmath>

#include "compressor/SerfXORCompressor.h"
#include "utils/Double.h"

class SerfConstructCompressor {
private:
    int compressedSizeInBits = 0;
    int numberOfValues = 0;
    double storedCompressionRatio = 0;
    double maxDiff;
    long storedAppLongValue = Double::doubleToLongBits(NAN);
    long bw[64]{};
    SerfXORCompressor *xor_compressor = nullptr;

public:
    explicit SerfConstructCompressor(double maxDiff);
    ~SerfConstructCompressor();
    void addValue(double v);
    long getCompressedSizeInBits() const;
    std::vector<char> getBytes();
    void close();
    void refresh();

private:
    long findAppLong(double minDouble, double maxDouble, long sign, double original, long lastLong);
};


#endif
