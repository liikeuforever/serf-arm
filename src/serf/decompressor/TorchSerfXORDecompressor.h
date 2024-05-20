#ifndef TORCH_SERF_XOR_DECOMPRESSOR_H
#define TORCH_SERF_XOR_DECOMPRESSOR_H

#include <cmath>
#include <limits>
#include <memory>
#include <vector>

#include "serf/utils/double.h"
#include "serf/utils/array.h"
#include "serf/utils/InputBitStream.h"
#include "serf/utils/PostOfficeSolver.h"

class TorchSerfXORDecompressor {
private:
    static const int BLOCK_SIZE = 1000;
    uint64_t storedVal = Double::DoubleToLongBits(2);
    int storedLeadingZeros = std::numeric_limits<int>::max();
    int storedTrailingZeros = std::numeric_limits<int>::max();
    const std::unique_ptr<InputBitStream> in = std::make_unique<InputBitStream>();
    Array<int> leadingRepresentation = {0, 8, 12, 16, 18, 20, 22, 24};
    Array<int> trailingRepresentation = {0, 22, 28, 32, 36, 40, 42, 46};
    int leadingBitsPerValue = 3;
    int trailingBitsPerValue = 3;
    int numberOfValues = 0;
    bool equalWin = false;
    const long adjustD;

public:
    explicit TorchSerfXORDecompressor(long adjustD): adjustD(adjustD) {}

    double decompress(const Array<uint8_t>& input);

    double decompress(const std::vector<uint8_t> &input);

private:
    uint64_t readValue();

    void nextValue();

    void updateFlagAndPositionsIfNeeded();

    void updateLeadingRepresentation();

    void updateTrailingRepresentation();
};

#endif //TORCH_SERF_XOR_DECOMPRESSOR_H