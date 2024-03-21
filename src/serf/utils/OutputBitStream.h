#ifndef SERF_OUTPUT_BIT_STREAM_H
#define SERF_OUTPUT_BIT_STREAM_H

#include <cstdlib>
#include <cstdint>

class OutputBitStream {
private:
    uint32_t *data;
    uint32_t data_len;
    uint32_t cursor;
    uint32_t bit_in_buffer;
    uint64_t buffer;

public:
    explicit OutputBitStream(uint32_t bufSize);

    ~OutputBitStream();

    uint32_t write(uint64_t src, uint32_t length);

    uint32_t writeLong(uint64_t src, uint64_t length);

    uint32_t writeInt(uint32_t src, uint32_t length);

    uint32_t writeBit(bool bit);

    void flush();

    uint8_t *getBuffer();

    void refresh();
};

#endif
