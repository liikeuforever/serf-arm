#include "SerfQtCompressor.h"

void SerfQtCompressor::addValue(double v) {
    long q = (long) std::round((v - preValue) / (2 * maxDiff));
    double recoverValue = preValue + 2 * maxDiff * q;
    if (__builtin_expect(std::abs(recoverValue - v) > maxDiff || std::isnan(v), false)) {
        // small cases
        compressedBits += out->writeBit(true);
        uint64_t xorResult = Double::doubleToLongBits(v) ^ Double::doubleToLongBits(preValue);
        int leadingZeroCount = std::min(__builtin_clzll(xorResult), 31);
        compressedBits += out->writeInt(static_cast<uint32_t>(leadingZeroCount), 5);
        compressedBits += out->writeLong(xorResult, 64 - leadingZeroCount);
        preValue = v;
    } else {
        compressedBits += out->writeBit(false);
        compressedBits += EliasDeltaCodec::encode(ZigZagCodec::encode(static_cast<int64_t>(q)) + 1, *out);
        preValue = recoverValue;
    }
}

Array<uint8_t> SerfQtCompressor::getBytes() {
    Array<uint8_t> result(static_cast<int>(std::ceil(static_cast<double>(storedCompressedBits) / 8.0)));
    uint8_t *ptr = out->getBuffer();
    for (int i = 0; i < result.length; ++i) {
        result[i] = ptr[i];
    }
    out->refresh();
    return result;
}

void SerfQtCompressor::close() {
    addValue(Double::NaN);
    out->flush();
    preValue = 2;
    storedCompressedBits = compressedBits;
    compressedBits = 0;
}

long SerfQtCompressor::getCompressedSizeInBits() const {
    return storedCompressedBits;
}
