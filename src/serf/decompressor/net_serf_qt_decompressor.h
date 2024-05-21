#ifndef NET_SERF_QT_DECOMPRESSOR_H
#define NET_SERF_QT_DECOMPRESSOR_H

#include <cstdint>
#include <memory>

#include "serf/utils/input_bit_stream.h"
#include "serf/utils/zig_zag_codec.h"
#include "serf/utils/elias_delta_codec.h"

class NetSerfQtDecompressor {
private:
    const double maxDiff;
    double preValue = 2;
    std::unique_ptr<InputBitStream> in = std::make_unique<InputBitStream>();

public:
    explicit NetSerfQtDecompressor(double maxDiff);

    double decompress(Array<uint8_t> &bs);
};

#endif //NET_SERF_QT_DECOMPRESSOR_H