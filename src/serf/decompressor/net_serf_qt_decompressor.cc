#include "net_serf_qt_decompressor.h"

NetSerfQtDecompressor::NetSerfQtDecompressor(double maxDiff): maxDiff(maxDiff) {}

double NetSerfQtDecompressor::decompress(Array<uint8_t> &bs) {
    in->SetBuffer(bs);
    in->ReadInt(4);
    int exceptionFlag = in->ReadInt(1);
    if (exceptionFlag == 0) {
        long decodeValue =
            ZigZagCodec::Decode(EliasDeltaCodec::Decode(in.get()) - 1);
        preValue = preValue + 2 * maxDiff * decodeValue;
    } else {
        int leadingZeroCount = in->ReadInt(5);
        long leftBits = in->ReadLong(64 - leadingZeroCount);
        preValue = Double::LongBitsToDouble(leftBits ^ Double::DoubleToLongBits(preValue));
    }
    return preValue;
}
