#ifndef CHIMP_DECOMPRESSOR_H
#define CHIMP_DECOMPRESSOR_H

#include <memory>
#include <limits>
#include <cstdint>
#include <vector>

#include "serf/utils/Array.h"
#include "serf/utils/InputBitStream.h"

class ChimpDecompressor {
private:
    int storedLeadingZeros = std::numeric_limits<int>::max();

    int storedTrailingZeros = std::numeric_limits<int>::max();

    std::unique_ptr<InputBitStream> input_bit_stream;

public:
    std::vector<double> decompress(const Array<uint8_t> &bs);
};

#endif //CHIMP_DECOMPRESSOR_H