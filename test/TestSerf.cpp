// Test Program
#pragma once

#include <iostream>
#include <bitset>

#include "SerfCompressor.h"

using std::cout, std::endl;

int main() {
    SerfCompressor compressor(2);
    compressor.addValue(0.01);
    compressor.addValue(0.11);
    compressor.close();

    std::vector<char> result = compressor.getBytes();
    for (const auto &item: result) {
        std::bitset<8> bin(item);
        cout << bin << endl;
    }
    return 0;
}