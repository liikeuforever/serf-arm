#ifndef SERF_INPUTBITSTREAM_H
#define SERF_INPUTBITSTREAM_H

#include <cinttypes>
#include <cstdlib>
#include <iostream>
#include <cstring>
#include <bitset>
#include <cmath>
#include <endian.h>

class InputBitStream {
private:
    uint32_t *mem_start_addr;
    uint32_t *data;
    uint64_t len;
    uint64_t buffer;
    uint64_t cursor;
    uint64_t bitcnt;

public:
    InputBitStream() = default;

    InputBitStream(uint8_t *raw_data, size_t size);

    ~InputBitStream();

    uint64_t peek(size_t num);

    void forward(size_t num);

    uint64_t readLong(size_t num);

    uint32_t readInt(size_t num);

    uint32_t readBit();
};


#endif
