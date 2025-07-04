#ifndef SERF_INPUT_BIT_STREAM_H
#define SERF_INPUT_BIT_STREAM_H

#ifdef __APPLE__
#include <machine/endian.h>
#include <libkern/OSByteOrder.h>
#define htobe32(x) OSSwapHostToBigInt32(x)
#define htole32(x) OSSwapHostToLittleInt32(x)
#define be32toh(x) OSSwapBigToHostInt32(x)
#define le32toh(x) OSSwapLittleToHostInt32(x)
#elif defined(__linux__)
#include <endian.h>
#endif

#include <cstdlib>
#include <cstring>
#include <vector>
#include <cmath>
#include <memory>

#include "array.h"

class InputBitStream {
 public:
    InputBitStream() = default;

    InputBitStream(uint8_t *raw_data, size_t size);

    uint64_t ReadLong(size_t len);

    uint32_t ReadInt(size_t len);

    uint32_t ReadBit();

    void SetBuffer(const Array<uint8_t> &new_buffer);

    void SetBuffer(const std::vector<uint8_t> &new_buffer);

 private:
    void Forward(size_t len);
    uint64_t Peek(size_t len);

    Array<uint32_t> data_;
    uint64_t buffer_ = 0;
    uint64_t cursor_ = 0;
    uint64_t bit_in_buffer_ = 0;
};

#endif  // SERF_INPUT_BIT_STREAM_H
