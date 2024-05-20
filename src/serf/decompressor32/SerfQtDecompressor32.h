#ifndef SERF_QT_DECOMPRESSOR_32_H
#define SERF_QT_DECOMPRESSOR_32_H

#include <vector>
#include <memory>

#include "serf/utils/ZigZagCodec.h"
#include "serf/utils/elias_delta_codec.h"
#include "serf/utils/float.h"
#include "serf/utils/input_bit_stream.h"

class SerfQtDecompressor32 {
private:
    const float maxDiff;
    std::unique_ptr<InputBitStream> in = std::make_unique<InputBitStream>();
    float preValue;

public:
    explicit SerfQtDecompressor32(float maxDiff);

    std::vector<float> decompress(Array<uint8_t> &bs);

private:
    float nextValue();
};

#endif //SERF_QT_DECOMPRESSOR_32_H