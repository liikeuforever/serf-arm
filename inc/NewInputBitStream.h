#ifndef SERFNATIVE_NEWINPUTBITSTREAM_H
#define SERFNATIVE_NEWINPUTBITSTREAM_H

#include <cinttypes>
#include <cstdlib>
#include <iostream>
#include <cstring>
#include <bitset>
#include <cmath>
#include <endian.h>

class NewInputBitStream {
private:
    uint32_t *mem_start_addr;
    uint32_t *data;
    uint64_t len;
    uint64_t buffer;
    uint64_t cursor;
    uint64_t bitcnt;

public:
    NewInputBitStream() = default;

    NewInputBitStream(uint8_t *raw_data, size_t size);

    ~NewInputBitStream();

    uint64_t peek(size_t num);

    void forward(size_t num);

    uint64_t readLong(size_t num);

    uint32_t readInt(size_t num);

    uint32_t readBit();
};


#endif //SERFNATIVE_NEWINPUTBITSTREAM_H
