#include "SerfConstructCompressor.h"

void SerfConstructCompressor::addValue(double v) {
    numberOfValues++;

    // note we cannot let > maxDiff, because NaN - v > maxDiff is always false
    if (std::abs(Double::longBitsToDouble(storedAppLongValue) - v) > maxDiff) {
        if (std::isnan(v)) {
            // handle NaN
            storedAppLongValue = Double::doubleToLongBits(v);
        } else {
            // in our implementation, we do not consider the subnormal case and overflow case
            double min = v - maxDiff;
            double max = v + maxDiff;
            if (min >= 0) {
                // both positive
                storedAppLongValue = findAppLong(min, max, 0, v, storedAppLongValue, maxDiff);
            } else if (max <= 0) {
                // both negative
                storedAppLongValue = findAppLong(-max, -min, 0x8000000000000000L, v, storedAppLongValue, maxDiff);
            } else if (storedAppLongValue >= 0) {
                // consider positive part only, to make more leading zeros
                storedAppLongValue = findAppLong(0, max, 0, v, storedAppLongValue, maxDiff);
            } else {
                // consider negative part only, to make more leading zeros
                storedAppLongValue = findAppLong(0, -min, 0x8000000000000000L, v, storedAppLongValue, maxDiff);
            }
        }
    }
    // empty else, let current value be the last value, making an XORed value of 0.

    compressedSizeInBits += xor_compressor.addValue(storedAppLongValue);
}

long SerfConstructCompressor::getCompressedSizeInBits() {
    return compressedSizeInBits;
}

std::vector<char> SerfConstructCompressor::getBytes() {
    int byteCount = ceil(compressedSizeInBits / 8.0);
    std::vector<char> result; result.reserve(byteCount);
    auto bytes = xor_compressor.getOut();
    for (int i = 0; i < byteCount; ++i) {
        result.push_back(static_cast<char>(bytes[i]));
    }
    return std::move(result);
}

void SerfConstructCompressor::close() {
    double thisCompressionRatio = compressedSizeInBits / (numberOfValues * 64.0);
    if (storedCompressionRatio < thisCompressionRatio) {
        xor_compressor.setDistribution();
    }
    storedCompressionRatio = thisCompressionRatio;
    compressedSizeInBits += xor_compressor.close();
}

void SerfConstructCompressor::refresh() {
    compressedSizeInBits = 0;
    numberOfValues = 0;
    xor_compressor.refresh();
}

long SerfConstructCompressor::findAppLong(double minDouble, double maxDouble, long sign, double original, long lastLong,
                                          double maxDiff) {
    long min = Double::doubleToLongBits(minDouble) & 0x7fffffffffffffffL; // may be negative zero
    long max = Double::doubleToLongBits(maxDouble);
    int leadingZeros = __builtin_clzl(min xor max);
    long frontMask = 0xffffffffffffffffL << (64 - leadingZeros);
    long front = frontMask & min;
    long rear = (~frontMask) & lastLong;
    int shift = 63 - leadingZeros;
    long bitMask = 1L << shift;
    long resultLong;

    for (int i = leadingZeros; i < 64; ++i) {
        rear -= ((bitMask & rear) >> shift) * bw[shift];
        front += ((bitMask & min) >> shift) * bw[shift];

        resultLong = rear + front;
        if (resultLong >= min && resultLong <= max) {
            resultLong = resultLong xor sign;
            if (std::abs(Double::longBitsToDouble(resultLong) - original) <= maxDiff) {
                return resultLong;
            }
        }

        resultLong = (rear + front + bw[shift]) & 0x7fffffffffffffffL; // may be overflow
        if (resultLong <= max) {
            resultLong = resultLong ^ sign;
            if (std::abs(Double::longBitsToDouble(resultLong) - original) <= maxDiff) {
                return resultLong;
            }
        }
        bitMask = bitMask >> 1;
        --shift;
    }

    return Double::doubleToLongBits(original);    // we do not find a satisfied value, so we return the original value
}
