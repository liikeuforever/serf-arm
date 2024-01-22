#ifndef SERFNATIVE_NEWINPUTBITSTREAM_H
#define SERFNATIVE_NEWINPUTBITSTREAM_H

#include <cinttypes>
#include <cstdlib>
#include <iostream>
#include <cstring>
#include <bitset>

class NewInputBitStream {
private:
    uint32_t *data;
    int64_t len;
    uint64_t buffer;
    int64_t cursor;
    int64_t bitcnt;

private:
    void convertCharArrToUInt32Arr(const char *raw_data, int size) {
        data = (uint32_t *) malloc(len * sizeof(uint32_t));
        memset(data, 0, len * sizeof(uint32_t));

        int index = 0;
        short cnt = 1;
        for (int i = 0; i < size; ++i) {
            data[index] |= ((static_cast<uint8_t>(raw_data[i]) | (uint32_t) 0) << (32 - cnt * 8));
            cnt += 1;

            if (cnt == 5) {
                index += 1;
                cnt = 1;
            }
        }
    }

public:
    NewInputBitStream(char *raw_data, int size) {
        len = size / sizeof(uint32_t) + 1;
        convertCharArrToUInt32Arr(raw_data, size);
        buffer = ((uint64_t) data[0]) << 32;
        cursor = 1;
        bitcnt = 32;
    }

    uint64_t peek(size_t num) {
        return buffer >> (64 - num);
    }

    void forward(size_t num) {
        bitcnt -= num;
        buffer <<= num;
        if (bitcnt < 32) {
            if (cursor < len) {
                uint64_t data_ = data[cursor];
                buffer |= data_ << (32 - bitcnt);
                bitcnt += 32;
                cursor++;
            } else {
                bitcnt = 64;
            }
        }
    }

    long readLong(size_t num) {
        if (num == 0) return 0;
        long result = 0;
        if (num > 32) {
            result = static_cast<long>(peek(32));
            forward(32);
            result <<= num - 32;
            num -= 32;
        }
        result |= static_cast<long>(peek(num));
        forward(num);
        return result;
    }

    int readInt(size_t num) {
        if (num == 0) return 0;
        int result = 0;
        result |= static_cast<int>(peek(num));
        forward(num);
        return result;
    }

    int readBit() {
        uint32_t result = 0;
        result |= peek(1);
        forward(1);
        return static_cast<int>(result);
    }
};


#endif //SERFNATIVE_NEWINPUTBITSTREAM_H
