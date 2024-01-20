#ifndef SERFNATIVE_SERFCOMPRESSOR_H
#define SERFNATIVE_SERFCOMPRESSOR_H

#include "SerfXORCompressor.h"

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
    SerfCompressor(int alpha) {
        static_assert(sizeof(long) == 8);
        fAlpha = 1075 - Elf64Utils::getFAlpha(alpha);
        maxDiff = Elf64Utils::get10iN(alpha);
    }

    bool isInfinite(double value) {
        return std::numeric_limits<double>::infinity() == value || -std::numeric_limits<double>::infinity() == value;
    }

    long doubleToLongBits(double value) {
        long result;
        std::memcpy(&result, &value, sizeof(value));
        return result;
    }

    double longBitsToDouble(long bits) {
        double result;
        std::memcpy(&result, &bits, sizeof(bits));
        return result;
    }

    void addValue(double v) {
        long vPrimeLong = 0;
        numberOfValues++;

        // let current value be the last value, making an XORed value of 0.
        if (std::abs(storedErasedDoubleValue - v) <= maxDiff) {
            vPrimeLong = storedErasedLongValue;
        } else {
            if (isInfinite(v) || std::isnan(v)) {
                vPrimeLong = doubleToLongBits(v);
            } else {
                long vLongs[] = {
                        doubleToLongBits(v + maxDiff),
                        doubleToLongBits(v),
                        doubleToLongBits(v - maxDiff)
                };
                int maxXoredTrailingZerosCount = -1, maxXoredLeadingZerosCount = -1;
                int xoredTrailingZerosCount, xoredLeadingZeroCount;
                long xoredLongValue;
                long vPrimeLongTemp, mask;
                int e, eraseBits;
                for (long vLongTemp : vLongs) {
                    e = ((int) (vLongTemp >> 52)) & 0x7ff;      // e may be different
                    eraseBits = fAlpha - e;
                    mask = 0xffffffffffffffffL << eraseBits;
                    vPrimeLongTemp = mask & vLongTemp;
                    xoredLongValue = vPrimeLongTemp ^ storedErasedLongValue;
                    xoredTrailingZerosCount = xor_compressor.numberOfTrailingZeros(xoredLongValue);
                    xoredLeadingZeroCount = xor_compressor.numberOfLeadingZeros(xoredLongValue);
                    if (xoredTrailingZerosCount >= maxXoredTrailingZerosCount
                        && xoredLeadingZeroCount >= maxXoredLeadingZerosCount
                        && std::abs(longBitsToDouble(vPrimeLongTemp) - v) <= maxDiff) {
                        maxXoredTrailingZerosCount = xoredTrailingZerosCount;
                        maxXoredLeadingZerosCount = xoredLeadingZeroCount;
                        vPrimeLong = vPrimeLongTemp;
                    }
                }
            }
            storedErasedDoubleValue = longBitsToDouble(vPrimeLong);
            storedErasedLongValue = vPrimeLong;
        }

        compressedSizeInBits += xor_compressor.addValue(vPrimeLong);
    }

    long getCompressedSizeInBits() {
        return compressedSizeInBits;
    }

    std::vector<char> getBytes() {
        int byteCount = ceil(compressedSizeInBits / 8.0);
        std::vector<char> result(byteCount);
        std::copy_n(xor_compressor.getOut().begin(), byteCount, result.begin());
        return result;
    }

    void close() {
        double thisCompressionRatio = compressedSizeInBits / (numberOfValues * 64.0);
        if (storedCompressionRatio < thisCompressionRatio) {
            xor_compressor.setDistribution();
        }
        storedCompressionRatio = thisCompressionRatio;
        compressedSizeInBits += xor_compressor.close();
    }

    void refresh() {
        compressedSizeInBits = 0;
        numberOfValues = 0;
        xor_compressor.refresh();        // note this refresh should be at the last
    }
};


#endif //SERFNATIVE_SERFCOMPRESSOR_H
