#ifndef CHIMP_DECOMPRESSOR_H
#define CHIMP_DECOMPRESSOR_H

#include <memory>
#include <limits>
#include <cstdint>
#include <vector>

#include "serf/utils/Array.h"
#include "serf/utils/InputBitStream.h"
#include "serf/utils/Double.h"

class ChimpDecompressor {
private:
    constexpr static const int16_t leadingRep[] = {0, 8, 12, 16, 18, 20, 22, 24};

    int storedLeadingZeros = std::numeric_limits<int>::max();

    int storedTrailingZeros = std::numeric_limits<int>::max();

    int32_t previousValues = 128;

    int32_t previousValuesLog2 = 31 - __builtin_clz(128);

    int32_t initialFill = previousValuesLog2 + 9;

    std::unique_ptr<uint64_t []> storedValues = std::make_unique<uint64_t []>(128);

    uint64_t delta;

    std::unique_ptr<InputBitStream> input_bit_stream;

public:
    std::vector<double> decompress(const Array<uint8_t> &bs);
};

#endif //CHIMP_DECOMPRESSOR_H