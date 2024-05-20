#ifndef GORILLA_DECOMPRESSOR_H
#define GORILLA_DECOMPRESSOR_H


#include <limits>
#include <memory>
#include "serf/utils/array.h"
#include "serf/utils/InputBitStream.h"
#include "serf/utils/double.h"

class GorillaDecompressor {
public:
    GorillaDecompressor() = default;

    std::vector<double> decompress(const Array<uint8_t>& compress_pack);

private:
    uint64_t pr_value_ = 0;
    int pr_lead_ = std::numeric_limits<int>::max();
    int pr_trail_ = 0;

    bool first_ = true;

    std::unique_ptr<InputBitStream> input_bit_stream_ = std::make_unique<InputBitStream>();

    double nextValue();
};


#endif // GORILLA_DECOMPRESSOR_H
