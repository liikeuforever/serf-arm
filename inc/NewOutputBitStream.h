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
    explicit NewOutputBitStream(int bufSize);

    ~NewOutputBitStream();

    void write(uint64_t data, uint64_t length);

    void writeLong(uint64_t data, uint64_t length);

    int flush();

    int writeInt(int n, int length);

    int writeBit(bool bit);

    uint8_t *getBuffer();
};


#endif //SERFNATIVE_NEWOUTPUTBITSTREAM_H
