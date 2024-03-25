#ifndef SERF_QT_DECOMPRESSOR_H
#define SERF_QT_DECOMPRESSOR_H

#include <memory>
#include <vector>

#include "serf/utils/Double.h"
#include "serf/utils/InputBitStream.h"
#include "serf/utils/ZigZagCodec.h"
#include "serf/utils/EliasDeltaCodec.h"

class SerfQtDecompressor {
private:
    const double maxDiff;
    const std::unique_ptr<InputBitStream> in = std::make_unique<InputBitStream>();
    double preValue;

public:
    explicit SerfQtDecompressor(double maxDiff): maxDiff(maxDiff) {}

    std::vector<double> decompress(const Array<uint8_t>& bs);

private:
    double nextValue();
};

#endif //SERF_QT_DECOMPRESSOR_H
