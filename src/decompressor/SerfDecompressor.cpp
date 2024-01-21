#include "SerfDecompressor.h"

std::vector<double> SerfDecompressor::decompress() {
    std::vector<double> values;
    double value;
    while (!isnan(value = xor_decompressor.readValue())) {
        values.emplace_back(value);
    }
    return values;
}

double SerfDecompressor::nextValue() {
    return xor_decompressor.readValue();
}

void SerfDecompressor::setBytes(char *data, size_t data_size) {
    xor_decompressor.setBytes(data, data_size);
}

void SerfDecompressor::refresh() {
    xor_decompressor.refresh();
}

bool SerfDecompressor::available() {
    return xor_decompressor.available();
}
