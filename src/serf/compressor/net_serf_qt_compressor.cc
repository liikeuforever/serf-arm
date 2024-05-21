#include "net_serf_qt_compressor.h"

NetSerfQtCompressor::NetSerfQtCompressor(double errorBound): maxDiff(errorBound) {}

Array<uint8_t> NetSerfQtCompressor::compress(double v) {
    int writtenBitsCount = out->WriteInt(0, 4);
    long q = std::round((v - preValue) / (2 * maxDiff));
    double recoverValue = preValue + 2 * maxDiff * q;
    if (std::abs(recoverValue - v) > maxDiff) {
        writtenBitsCount += out->WriteBit(true);
        uint64_t xorResult = Double::DoubleToLongBits(v) ^ Double::DoubleToLongBits(preValue);
        int leadingZeroCount = std::min(__builtin_clzll(xorResult), 31);
        writtenBitsCount += out->WriteInt(leadingZeroCount, 5);
        writtenBitsCount += out->WriteLong(xorResult, 64 - leadingZeroCount);
        preValue = v;
    } else {
        writtenBitsCount += out->WriteBit(false);
        writtenBitsCount += EliasDeltaCodec::Encode(ZigZagCodec::Encode(q) + 1, out.get());
        preValue = recoverValue;
    }
    out->Flush();
    Array<uint8_t> result = out->GetBuffer(std::ceil(writtenBitsCount / 8.0));
    out->Refresh();
    return result;
}

