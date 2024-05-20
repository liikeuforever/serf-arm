#include "NetSerfQtCompressor.h"

NetSerfQtCompressor::NetSerfQtCompressor(double errorBound): maxDiff(errorBound) {}

Array<uint8_t> NetSerfQtCompressor::compress(double v) {
    int writtenBitsCount = out->writeInt(0, 4);
    long q = std::round((v - preValue) / (2 * maxDiff));
    double recoverValue = preValue + 2 * maxDiff * q;
    if (std::abs(recoverValue - v) > maxDiff) {
        writtenBitsCount += out->writeBit(true);
        uint64_t xorResult = Double::DoubleToLongBits(v) ^ Double::DoubleToLongBits(preValue);
        int leadingZeroCount = std::min(__builtin_clzll(xorResult), 31);
        writtenBitsCount += out->writeInt(leadingZeroCount, 5);
        writtenBitsCount += out->writeLong(xorResult, 64 - leadingZeroCount);
        preValue = v;
    } else {
        writtenBitsCount += out->writeBit(false);
        writtenBitsCount += elias_delta_codec::Encode(ZigZagCodec::encode(q) + 1, out.get());
        preValue = recoverValue;
    }
    out->flush();
    uint8_t *data_ptr = out->getBuffer();
    Array<uint8_t> result(std::ceil(writtenBitsCount / 8.0));
    for (int i = 0; i < result.length(); ++i) {
        result[i] = data_ptr[i];
    }
    out->refresh();
    return result;
}

