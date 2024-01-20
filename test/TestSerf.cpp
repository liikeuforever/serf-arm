// Test Program
#pragma once

#include <iostream>
#include <bitset>

#include "SerfCompressor.h"

using std::cout, std::endl;

int main() {
    SerfCompressor compressor(1);
    compressor.addValue(0.111111);
    compressor.addValue(0.122222);
    std::vector<char> output = compressor.getBytes();
    for (const auto &item: output) {
        std::bitset<8> bin(item);
        std::cout << bin << std::endl;
    }
    return 0;
}