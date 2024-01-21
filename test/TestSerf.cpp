// Test Program
#pragma once

#include <iostream>

#include "SerfCompressor.h"
#include "SerfDecompressor.h"

using std::cout, std::endl;

int main() {
    SerfCompressor compressor(2);
    compressor.addValue(0.01);
    compressor.addValue(0.11);
    compressor.close();

    std::vector<char> compressed_data = compressor.getBytes();

    SerfDecompressor decompressor;
    decompressor.setBytes(compressed_data.data(), compressed_data.size());
    for (const auto &item: decompressor.decompress()) {
        cout << item << endl;
    }

    return 0;
}