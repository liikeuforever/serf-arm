#ifndef SERFNATIVE_SERFXORDECOMPRESSOR_H
#define SERFNATIVE_SERFXORDECOMPRESSOR_H

#include <limits>
#include <cstring>

#include "InputBitStream.h"
#include "NewInputBitStream.h"
#include "PostOfficeSolver.h"
#include "Elf64Utils.h"

typedef unsigned long b64;

class SerfXORDecompressor {
private:
    b64 storedVal = 0;
    int storedLeadingZeros = std::numeric_limits<int>::max();
    int storedTrailingZeros = std::numeric_limits<int>::max();
    bool first = true;
    bool endOfStream = false;
    NewInputBitStream in = NewInputBitStream(nullptr, 0);
    std::vector<int> leadingRepresentation = {0, 8, 12, 16, 18, 20, 22, 24};
    std::vector<int> trailingRepresentation = {0, 22, 28, 32, 36, 40, 42, 46};
    int leadingBitsPerValue = 3;
    int trailingBitsPerValue = 3;

public:
    SerfXORDecompressor();

    void initLeadingRepresentation();

    void initTrailingRepresentation();

    void setBytes(char *data, size_t data_size);

    void refresh();

    void nextValue();

    void next();

    double readValue();

    bool available() const;

private:
    double longBitsToDouble(b64 bits);
};


#endif //SERFNATIVE_SERFXORDECOMPRESSOR_H
