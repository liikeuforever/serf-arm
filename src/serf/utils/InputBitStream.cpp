#include "utils/InputBitStream.h"

InputBitStream::InputBitStream(uint8_t *raw_data, size_t size) {
    bool overflow = size % sizeof(uint32_t);
    len = ceil(static_cast<double>(size) / sizeof(uint32_t));
    data = new uint32_t [len];
    mem_start_addr = data;

    auto *tmp_ptr = (uint32_t *) (raw_data);
    if (overflow) {
        for (int i = 0; i < len - 1; ++i) {
            data[i] = be32toh(*(tmp_ptr + i));
        }
        int byte_index = 1;
        data[len - 1] = 0;
        for (uint64_t i = (size / 4 * 4); i < size; ++i) {
            data[len - 1] |= (raw_data[i] << (32 - byte_index * 8));
            ++byte_index;
        }
    } else {
        for (int i = 0; i < len; ++i) {
            data[i] = be32toh(*(tmp_ptr + i));
        }
    }

    buffer = ((uint64_t) data[0]) << 32;
    cursor = 1;
    bitcnt = 32;
}

InputBitStream::~InputBitStream() {
    delete[] mem_start_addr;
}

uint64_t InputBitStream::peek(size_t num) {
    return buffer >> (64 - num);
}

void InputBitStream::forward(size_t num) {
    bitcnt -= num;
    buffer <<= num;
    if (bitcnt < 32) {
        if (cursor < len) {
            auto data_ = (uint64_t) data[cursor];
            buffer |= (data_ << (32 - bitcnt));
            bitcnt += 32;
            cursor++;
        } else {
            bitcnt = 64;
        }
    }
}

uint64_t InputBitStream::readLong(size_t num) {
    if (num == 0) return 0;
    uint64_t result = 0;
    if (num > 32) {
        result = peek(32);
        forward(32);
        result <<= num - 32;
        num -= 32;
    }
    result |= peek(num);
    forward(num);
    return result;
}

uint32_t InputBitStream::readInt(size_t num) {
    if (num == 0) return 0;
    uint32_t result = 0;
    result |= peek(num);
    forward(num);
    return result;
}

uint32_t InputBitStream::readBit() {
    uint32_t result = peek(1);
    forward(1);
    return result;
}
