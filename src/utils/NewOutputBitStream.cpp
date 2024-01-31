#include "NewOutputBitStream.h"

NewOutputBitStream::NewOutputBitStream(int bufSize) {
    len = bufSize / 4 + 1;
    output = new uint32_t [len];
    mem_start_addr = output;
    buffer = 0;
    cursor = 0;
    bitcnt = 0;
}

NewOutputBitStream::~NewOutputBitStream() {
    delete[] mem_start_addr;
}

void NewOutputBitStream::write(uint64_t data, uint64_t length) {
    data <<= (64 - length);
    buffer |= (data >> bitcnt);
    bitcnt += length;
    if (bitcnt >= 32) {
        output[cursor++] = (buffer >> 32);
        buffer <<= 32;
        bitcnt -= 32;
    }
}

void NewOutputBitStream::writeLong(uint64_t data, uint64_t length) {
    if (length == 0) return;
    if (length > 32) {
        write(data >> (length-32), 32);
        length -= 32;
    }
    write(data, length);
}

int NewOutputBitStream::flush() {
    if (bitcnt) {
        output[cursor++] = buffer >> 32;
        buffer = 0;
        bitcnt = 0;
    }
    return cursor;
}

int NewOutputBitStream::writeInt(int n, int length) {
    write(static_cast<uint64_t>(n), length);
    return length;
}

int NewOutputBitStream::writeBit(bool bit) {
    write(bit, 1);
    return 1;
}

uint8_t *NewOutputBitStream::getBuffer() {
    for (int i = 0; i < len ; ++i) {
        mem_start_addr[i] = htobe32(mem_start_addr[i]);
    }
    return (uint8_t *) mem_start_addr;
}
