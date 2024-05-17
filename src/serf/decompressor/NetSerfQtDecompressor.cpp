#include "NetSerfQtDecompressor.h"

NetSerfQtDecompressor::NetSerfQtDecompressor(double maxDiff): maxDiff(maxDiff) {}

double NetSerfQtDecompressor::decompress(Array<uint8_t> &bs) {
    in->setBuffer(bs);
    in->readInt(4);
    int exceptionFlag = in->readInt(1);
    if (exceptionFlag == 0) {
        long decodeValue = ZigZagCodec::decode(EliasDeltaCodec::decode(in.get()) - 1);
        preValue = preValue + 2 * maxDiff * decodeValue;
    } else {
        int leadingZeroCount = in->readInt(5);
        long leftBits = in->readLong(64 - leadingZeroCount);
        preValue = Double::longBitsToDouble(leftBits ^ Double::doubleToLongBits(preValue));
    }
    return preValue;
}
