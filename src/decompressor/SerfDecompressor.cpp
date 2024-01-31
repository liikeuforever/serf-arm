#include "SerfDecompressor.h"

std::vector<double> SerfDecompressor::decompress() {
    std::vector<double> values;
    double value;
    while (!std::isnan(value = xor_decompressor->readValue())) {
        values.push_back(value);
//        std::cout << value << std::endl;
    }
    return std::move(values);
}

double SerfDecompressor::nextValue() {
    return xor_decompressor->readValue();
}

void SerfDecompressor::setBytes(char *data, size_t data_size) {
    xor_decompressor->setBytes(data, data_size);
}

void SerfDecompressor::refresh() {
    xor_decompressor->refresh();
}

bool SerfDecompressor::available() {
    return xor_decompressor->available();
}

SerfDecompressor::SerfDecompressor() {
    xor_decompressor = new SerfXORDecompressor();
}

SerfDecompressor::~SerfDecompressor() {
    delete xor_decompressor;
}
