#ifndef SERF_INPUT_BIT_STREAM_H
#define SERF_INPUT_BIT_STREAM_H

#include <cstdlib>
#include <cstring>
#include <vector>
#include <cmath>
#include <endian.h>

#include "serf/utils/Array.h"

class InputBitStream {
private:
    uint32_t *mem_start_addr;
    uint32_t *data;
    uint64_t len;
    uint64_t buffer;
    uint64_t cursor;
    uint64_t bit_in_buffer;

public:
    InputBitStream() = default;

    InputBitStream(uint8_t *raw_data, size_t size);

    ~InputBitStream();

    uint64_t peek(size_t num);

    void forward(size_t num);

    uint64_t readLong(size_t num);

    uint32_t readInt(size_t num);

    uint32_t readBit();

    void setBuffer(Array<uint8_t> newBuffer);

    void setBuffer(const std::vector<uint8_t> &newBuffer);
};


#endif
