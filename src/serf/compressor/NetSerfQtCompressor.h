#ifndef NET_SERF_QT_COMPRESSOR_H
#define NET_SERF_QT_COMPRESSOR_H

#include <memory>
#include <cstdint>
#include <cmath>

#include "serf/utils/array.h"
#include "serf/utils/output_bit_stream.h"
#include "serf/utils/double.h"
#include "serf/utils/elias_delta_codec.h"
#include "serf/utils/zig_zag_codec.h"

class NetSerfQtCompressor {
private:
    double preValue = 2;
    const double maxDiff;
    std::unique_ptr<OutputBitStream> out = std::make_unique<OutputBitStream>(5 * 8);

public:
    explicit NetSerfQtCompressor(double errorBound);

    Array<uint8_t> compress(double v);
};

#endif //NET_SERF_QT_COMPRESSOR_H