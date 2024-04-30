#ifndef CHIMP_COMPRESSOR_H
#define CHIMP_COMPRESSOR_H

#include <cstdint>
#include <memory>
#include <limits>
#include <cmath>

#include "serf/utils/OutputBitStream.h"
#include "serf/utils/Double.h"
#include "serf/utils/Array.h"

class ChimpCompressor {
private:
    const uint16_t leadingRep[64] = {
            0, 0, 0, 0, 0, 0, 0, 0,
            1, 1, 1, 1, 2, 2, 2, 2,
            3, 3, 4, 4, 5, 5, 6, 6,
            7, 7, 7, 7, 7, 7, 7, 7,
            7, 7, 7, 7, 7, 7, 7, 7,
            7, 7, 7, 7, 7, 7, 7, 7,
            7, 7, 7, 7, 7, 7, 7, 7,
            7, 7, 7, 7, 7, 7, 7, 7
    };

    const uint16_t leadingRnd[64] = {
            0, 0, 0, 0, 0, 0, 0, 0,
            8, 8, 8, 8, 12, 12, 12, 12,
            16, 16, 18, 18, 20, 20, 22, 22,
            24, 24, 24, 24, 24, 24, 24, 24,
            24, 24, 24, 24, 24, 24, 24, 24,
            24, 24, 24, 24, 24, 24, 24, 24,
            24, 24, 24, 24, 24, 24, 24, 24,
            24, 24, 24, 24, 24, 24, 24, 24,
    };

    std::unique_ptr<OutputBitStream> output_bit_stream;

    int32_t storedLeadingZeros = std::numeric_limits<int>::max();

    int32_t index = 0;

    int32_t current = 0;

    long size = 0;

    int32_t previousValues = 128;

    int32_t previousValuesLog2 = 31 - __builtin_clz(128);

    int32_t threshold = 6 + previousValuesLog2;

    int32_t setLsb = (1 << (threshold + 1)) - 1;

    std::unique_ptr<int32_t []> indices = std::make_unique<int32_t []>(1 << (threshold + 1));

    std::unique_ptr<uint64_t []> storedValues = std::make_unique<uint64_t []>(128);

    int32_t flagZeroSize = previousValuesLog2 + 2;

    int32_t flagOneSize = previousValuesLog2 + 11;

    bool first = true;

    Array<uint8_t> bs = Array<uint8_t>(0);

public:
    explicit ChimpCompressor(int capacity);

    void addValue(double v);

    void close();

    Array<uint8_t> getBytes();

    long getCompressedSize();
};

#endif //CHIMPCOMPRESSOR_H