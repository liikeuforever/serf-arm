#include "SerfQtDecompressor.h"

std::vector<double> SerfQtDecompressor::decompress(const Array<uint8_t>& bs) {
    preValue = 2;
    in->setBuffer(bs);
    std::vector<double> decompressedValueList;
    double value;
    while (!std::isnan(value = nextValue())) {
        decompressedValueList.emplace_back(value);
    }
    return decompressedValueList;
}

double SerfQtDecompressor::nextValue() {
    double returnValue;
    int exceptionFlag = static_cast<int>(in->readInt(1));
    if (exceptionFlag == 0) {
        int64_t decodeValue = ZigZagCodec::decode(EliasDeltaCodec::decode(*in) - 1);
        double recoverValue = preValue + 2 * maxDiff * static_cast<double>(decodeValue);
        preValue = recoverValue;
        returnValue = recoverValue;
    } else {
        int leadingZeroCount = static_cast<int>(in->readInt(5));
        uint64_t leftBits = in->readLong(64 - leadingZeroCount);
        double recoverValue = Double::longBitsToDouble(leftBits ^ Double::doubleToLongBits(preValue));
        preValue = recoverValue;
        returnValue = recoverValue;
    }
    return returnValue;
}
