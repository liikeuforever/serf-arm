#include "SerfQtDecompressor32.h"

SerfQtDecompressor32::SerfQtDecompressor32(float maxDiff): maxDiff(maxDiff) {}

std::vector<float> SerfQtDecompressor32::decompress(Array<uint8_t> &bs) {
    preValue = 2;
    in->setBuffer(bs);
    std::vector<float> decompressedValueList;
    float value;
    while (!std::isnan(value = nextValue())) {
        decompressedValueList.emplace_back(value);
    }
    return decompressedValueList;
}

float SerfQtDecompressor32::nextValue() {
    float returnValue;
    int exceptionFlag = in->readInt(1);
    if (exceptionFlag == 0) {
        long decodeValue = ZigZagCodec::decode(EliasDeltaCodec::decode(*in) - 1);
        float recoverValue = preValue + 2 * maxDiff * decodeValue;
        preValue = recoverValue;
        returnValue = recoverValue;
    } else {
        int leadingZeroCount = in->readInt(4);
        uint32_t leftBits = in->readInt(32 - leadingZeroCount);
        float recoverValue = Float::intBitsToFloat(leftBits ^ Float::floatToIntBits(preValue));
        preValue = recoverValue;
        returnValue = recoverValue;
    }
    return returnValue;
}
