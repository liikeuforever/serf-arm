#ifndef SERF_QT_DECOMPRESSOR_32_H
#define SERF_QT_DECOMPRESSOR_32_H

#include <vector>
#include <memory>

#include "serf/utils/ZigZagCodec.h"
#include "serf/utils/EliasDeltaCodec.h"
#include "serf/utils/Float.h"
#include "serf/utils/InputBitStream.h"

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