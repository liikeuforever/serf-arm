#ifndef GORILLA_COMPRESSOR_H
#define GORILLA_COMPRESSOR_H


#include <memory>
#include "serf/utils/OutputBitStream.h"

class GorillaCompressor {
public:
    explicit GorillaCompressor(int capacity);

    void addValue(double v);

    void close();

    void getBytes();

private:
    std::unique_ptr<OutputBitStream> output_bit_stream_;
};


#endif // GORILLA_COMPRESSOR_H
