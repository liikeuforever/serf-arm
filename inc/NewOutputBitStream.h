#ifndef SERFNATIVE_NEWOUTPUTBITSTREAM_H
#define SERFNATIVE_NEWOUTPUTBITSTREAM_H

#include <cinttypes>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <bitset>

class NewOutputBitStream {
private:
    uint32_t *mem_start_addr;
    uint32_t *output;
    uint64_t len;
    uint64_t buffer;
    uint64_t cursor;
    uint64_t bitcnt;

public:
    explicit NewOutputBitStream(int bufSize) {
        len = bufSize / 4 + 1;
        output = new uint32_t [len];
        mem_start_addr = output;
        buffer = 0;
        cursor = 0;
        bitcnt = 0;
    }

    ~NewOutputBitStream() {
        delete[] mem_start_addr;
    }

public:
    void write(uint64_t data, uint64_t length) {
        data <<= (64 - length);
        buffer |= (data >> bitcnt);
        bitcnt += length;
        if (bitcnt >= 32) {
            output[cursor++] = (buffer >> 32);
            buffer <<= 32;
            bitcnt -= 32;
        }
    }

    void writeLong(uint64_t data, uint64_t length) {
        if (length == 0) return;
        if (length > 32) {
            write(data >> (length-32), 32);
            length -= 32;
        }
        write(data, length);
    }

    int flush() {
        if (bitcnt) {
            output[cursor++] = buffer >> 32;
            buffer = 0;
            bitcnt = 0;
        }
        return cursor;
    }

public:
    int writeInt(int n, int length) {
        write(static_cast<uint64_t>(n), length);
        return length;
    }

    int writeBit(bool bit) {
        write(bit, 1);
        return 1;
    }

    uint8_t *getBuffer() {
        for (int i = 0; i < len ; ++i) {
            mem_start_addr[i] = htobe32(mem_start_addr[i]);
        }
        return (uint8_t *) mem_start_addr;
    }
};


#endif //SERFNATIVE_NEWOUTPUTBITSTREAM_H
