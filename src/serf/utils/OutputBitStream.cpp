#include "serf/utils/OutputBitStream.h"

OutputBitStream::OutputBitStream(uint32_t bufSize) {
    data_len = bufSize / 4 + 1;
    data = new uint32_t [data_len];
    buffer = 0;
    cursor = 0;
    bit_in_buffer = 0;
}

OutputBitStream::~OutputBitStream() {
    delete[] data;
}

uint32_t OutputBitStream::write(uint64_t src, uint32_t length) {
    src <<= (64 - length);
    buffer |= (src >> bit_in_buffer);
    bit_in_buffer += length;
    if (bit_in_buffer >= 32) {
        data[cursor++] = (buffer >> 32);
        buffer <<= 32;
        bit_in_buffer -= 32;
    }
    return length;
}

uint32_t OutputBitStream::writeLong(uint64_t src, uint64_t length) {
    if (length == 0) return 0;
    if (length > 32) {
        write(src >> (length - 32), 32);
        write(src, length - 32);
        return length;
    }
    return write(src, length);
}

uint32_t OutputBitStream::writeInt(uint32_t src, uint32_t length) {
    return write(static_cast<uint64_t>(src), length);
}

uint32_t OutputBitStream::writeBit(bool bit) {
    return write(static_cast<uint64_t>(bit), 1);
}

uint8_t *OutputBitStream::getBuffer() {
    for (int i = 0; i < data_len ; ++i) {
        data[i] = htobe32(data[i]);
    }
    return (uint8_t *) data;
}

void OutputBitStream::flush() {
    if (bit_in_buffer) {
        data[cursor++] = buffer >> 32;
        buffer = 0;
        bit_in_buffer = 0;
    }
}

void OutputBitStream::refresh() {
    cursor = 0;
    bit_in_buffer = 0;
    buffer = 0;
}