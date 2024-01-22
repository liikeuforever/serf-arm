#ifndef SERFNATIVE_NEWOUTPUTBITSTREAM_H
#define SERFNATIVE_NEWOUTPUTBITSTREAM_H

#include <cinttypes>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <bitset>

class NewOutputBitStream {
private:
    uint32_t *output;
    int64_t len;
    uint64_t buffer;
    int64_t cursor;
    uint64_t bitcnt;

public:
    explicit NewOutputBitStream(int bufSize) {
        output = (uint32_t *) malloc((bufSize / 4 + 1) * sizeof(uint32_t));
        len = bufSize / 4 + 1;
        buffer = 0;
        cursor = 0;
        bitcnt = 0;
    }

    ~NewOutputBitStream() {
        delete output;
    }

public:
    void write(uint64_t data, uint64_t length) {
        data <<= (64 - length);
        buffer |= (data >> bitcnt);
        bitcnt += length;
        if (bitcnt >= 32) {
            output[cursor++] = buffer >> 32;
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
        write(n, length);
        return length;
    }

    int writeBit(bool bit) {
        write(bit, 1);
        return 1;
    }

    uint8_t *getBuffer() {
        // Tmp resolution for Big-Little Endian Problem
        uint8_t *convert_buffer = (uint8_t *) malloc((cursor + 1) * sizeof(uint32_t));
        int array_index = 0;
        for (int i = 0; i <= cursor; i++) {
            for (int j = 3; j >= 0; j--) {
                convert_buffer[array_index] = ((uint8_t *) (output + i))[j];
                array_index++;
            }
        }

        return convert_buffer;
    }
};


#endif //SERFNATIVE_NEWOUTPUTBITSTREAM_H
