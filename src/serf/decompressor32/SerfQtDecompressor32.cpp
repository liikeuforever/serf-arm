#include "SerfQtDecompressor32.h"

SerfQtDecompressor32::SerfQtDecompressor32(float maxDiff): maxDiff(maxDiff) {}

std::vector<float> SerfQtDecompressor32::decompress(Array<uint8_t> &bs) {
    preValue = 2;
    in->SetBuffer(bs);
    std::vector<float> decompressedValueList;
    float value;
    while (!std::isnan(value = nextValue())) {
        decompressedValueList.emplace_back(value);
    }
    return decompressedValueList;
}

float SerfQtDecompressor32::nextValue() {
    float returnValue;
    int exceptionFlag = in->ReadInt(1);
    if (exceptionFlag == 0) {
        long decodeValue = ZigZagCodec::decode(EliasDeltaCodec::Decode(in.get()) - 1);
        float recoverValue = preValue + 2 * maxDiff * decodeValue;
        preValue = recoverValue;
        returnValue = recoverValue;
    } else {
        int leadingZeroCount = in->ReadInt(4);
        uint32_t leftBits = in->ReadInt(32 - leadingZeroCount);
        float recoverValue = Float::IntBitsToFloat(
                leftBits ^ Float::FloatToIntBits(preValue));
        preValue = recoverValue;
        returnValue = recoverValue;
    }
    return returnValue;
}
