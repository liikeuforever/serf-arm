#include "SerfCompressor.h"

SerfCompressor::SerfCompressor(int alpha) {
    static_assert(sizeof(unsigned long) == 8);
    fAlpha = 1075 - Elf64Utils::getFAlpha(alpha);
    maxDiff = Elf64Utils::get10iN(alpha);
}

SerfCompressor::~SerfCompressor() {
    delete xor_compressor;
}

void SerfCompressor::addValue(double v) {
    b64 vPrimeLong = 0;
    numberOfValues++;

    // let current value be the last value, making an XORed value of 0.
    if (std::abs(storedErasedDoubleValue - v) <= maxDiff) {
        vPrimeLong = storedErasedLongValue;
    } else {
        if (std::isinf(v) || std::isnan(v)) {
            vPrimeLong = Double::doubleToULongBits(v);
        } else {
            b64 vLongs[] = {
                    Double::doubleToULongBits(v + maxDiff),
                    Double::doubleToULongBits(v),
                    Double::doubleToULongBits(v - maxDiff)
            };
            int maxXoredTrailingZerosCount = -1, maxXoredLeadingZerosCount = -1;
            int xoredTrailingZerosCount, xoredLeadingZeroCount;
            b64 xoredLongValue;
            b64 vPrimeLongTemp, mask;
            int e, eraseBits;
            for (b64 vLongTemp: vLongs) {
                e = (static_cast<int>(vLongTemp >> 52)) & 0x7ff;      // e may be different
                eraseBits = fAlpha - e;
                mask = 0xffffffffffffffffL << eraseBits;
                vPrimeLongTemp = mask & vLongTemp;
                xoredLongValue = vPrimeLongTemp ^ storedErasedLongValue;
                xoredTrailingZerosCount = __builtin_ctzl(xoredLongValue);
                xoredLeadingZeroCount = __builtin_clzl(xoredLongValue);
                if (xoredTrailingZerosCount >= maxXoredTrailingZerosCount
                    && xoredLeadingZeroCount >= maxXoredLeadingZerosCount
                    && std::abs(Double::UlongBitsToDouble(vPrimeLongTemp) - v) <= maxDiff) {
                    maxXoredTrailingZerosCount = xoredTrailingZerosCount;
                    maxXoredLeadingZerosCount = xoredLeadingZeroCount;
                    vPrimeLong = vPrimeLongTemp;
                }
            }
        }
        storedErasedDoubleValue = Double::UlongBitsToDouble(vPrimeLong);
        storedErasedLongValue = vPrimeLong;
    }

    compressedSizeInBits += xor_compressor->addValue(vPrimeLong);
}

long SerfCompressor::getCompressedSizeInBits() const {
    return compressedSizeInBits;
}

std::vector<char> SerfCompressor::getBytes() {
    int byteCount = ceil(compressedSizeInBits / 8.0);
    std::vector<char> result; result.reserve(byteCount);
    auto bytes = xor_compressor->getOut();
    for (int i = 0; i < byteCount; ++i) {
        result.push_back(static_cast<char>(bytes[i]));
    }
    return std::move(result);
}

void SerfCompressor::close() {
    double thisCompressionRatio = compressedSizeInBits / (numberOfValues * 64.0);
    if (storedCompressionRatio < thisCompressionRatio) {
        xor_compressor->setDistribution();
    }
    storedCompressionRatio = thisCompressionRatio;
    compressedSizeInBits += xor_compressor->close();
}

void SerfCompressor::refresh() {
    compressedSizeInBits = 0;
    numberOfValues = 0;
    xor_compressor->refresh();        // note this refresh should be at the last
}
