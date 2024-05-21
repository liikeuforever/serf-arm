#include "SerfQtCompressor32.h"

SerfQtCompressor32::SerfQtCompressor32(float maxDiff): maxDiff(maxDiff) {}

void SerfQtCompressor32::addValue(float v) {
    long q = std::round((v - preValue) / (2 * maxDiff));
    float recoverValue = preValue + 2 * maxDiff * q;
    if (std::abs(recoverValue - v) > maxDiff || std::isnan(v)) {
        // small cases
        compressedBits += out->WriteBit(true);
        uint32_t xorResult = Float::FloatToIntBits(v) ^
                             Float::FloatToIntBits(preValue);
        int leadingZeroCount = std::min(__builtin_clz(xorResult), 15);
        compressedBits += out->WriteInt(leadingZeroCount, 4);
        compressedBits += out->WriteInt(xorResult, 32 - leadingZeroCount);
        preValue = v;
    } else {
        compressedBits += out->WriteBit(false);
        compressedBits += EliasDeltaCodec::Encode(ZigZagCodec::encode(q) + 1, out.get());
        preValue = recoverValue;
    }
}

Array<uint8_t> SerfQtCompressor32::getBytes() {
    Array<uint8_t> result = out->GetBuffer(std::ceil(storedCompressedBits / 8.0));
    out->Refresh();
    return result;
}

void SerfQtCompressor32::close() {
    addValue(Float::kNan);
    out->Flush();
    preValue = 2;
    storedCompressedBits = compressedBits;
    compressedBits = 0;
}

long SerfQtCompressor32::getCompressedSizeInBits() {
    return compressedBits;
}