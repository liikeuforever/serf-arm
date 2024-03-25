#ifndef SERF_QT_COMPRESSOR_H
#define SERF_QT_COMPRESSOR_H

#include <cstdint>
#include <cmath>
#include <memory>

#include "serf/utils/OutputBitStream.h"
#include "serf/utils/Array.h"
#include "serf/utils/Double.h"
#include "serf/utils/EliasDeltaCodec.h"
#include "serf/utils/ZigZagCodec.h"

class SerfQtCompressor {
private:
    static const int BLOCK_SIZE = 1000;
    const double maxDiff;
    const std::unique_ptr<OutputBitStream> out = std::make_unique<OutputBitStream>(2 * BLOCK_SIZE * 8);
    double preValue = 2;
    long compressedBits = 0;
    long storedCompressedBits;

public:
    explicit SerfQtCompressor(double maxDiff): maxDiff(maxDiff) {}

    void addValue(double v);

    Array<uint8_t> getBytes();

    void close();

    long getCompressedSizeInBits() const;
};

#endif //SERF_QT_COMPRESSOR_H
