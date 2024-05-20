#include "SerfQtCompressor32.h"

SerfQtCompressor32::SerfQtCompressor32(float maxDiff): maxDiff(maxDiff) {}

void SerfQtCompressor32::addValue(float v) {
    long q = std::round((v - preValue) / (2 * maxDiff));
    float recoverValue = preValue + 2 * maxDiff * q;
    if (std::abs(recoverValue - v) > maxDiff || std::isnan(v)) {
        // small cases
        compressedBits += out->writeBit(true);
        uint32_t xorResult = Float::floatToIntBits(v) ^ Float::floatToIntBits(preValue);
        int leadingZeroCount = std::min(__builtin_clz(xorResult), 15);
        compressedBits += out->writeInt(leadingZeroCount, 4);
        compressedBits += out->writeInt(xorResult, 32 - leadingZeroCount);
        preValue = v;
    } else {
        compressedBits += out->writeBit(false);
        compressedBits += EliasDeltaCodec::encode(ZigZagCodec::encode(q) + 1, out.get());
        preValue = recoverValue;
    }
}

Array<uint8_t> SerfQtCompressor32::getBytes() {
    uint8_t *data_ptr = out->getBuffer();
    Array<uint8_t> compressedBytes(std::ceil(storedCompressedBits / 8.0));
    for (int i = 0; i < compressedBytes.length(); ++i) {
        compressedBytes[i] = data_ptr[i];
    }
    out->refresh();
    return compressedBytes;
}

void SerfQtCompressor32::close() {
    addValue(Float::NaN);
    out->flush();
    preValue = 2;
    storedCompressedBits = compressedBits;
    compressedBits = 0;
}

long SerfQtCompressor32::getCompressedSizeInBits() {
    return compressedBits;
}