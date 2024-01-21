//
// Created by czc on 18/01/24.
//

#ifndef SERFNATIVE_SERFDECOMPRESSOR_H
#define SERFNATIVE_SERFDECOMPRESSOR_H

#include "SerfXORDecompressor.h"

#include <vector>

class SerfDecompressor {
private:
    SerfXORDecompressor xor_decompressor;

public:

    std::vector<double> decompress() {
        std::vector<double> values;
        double value;
        while (!isnan(value = xor_decompressor.readValue())) {
            values.emplace_back(value);
        }
        return values;
    }

    double nextValue() {
        return xor_decompressor.readValue();
    }

    void setBytes(char *data, size_t data_size) {
        xor_decompressor.setBytes(data, data_size);
    }

    void refresh() {
        xor_decompressor.refresh();
    }

    bool available() {
        return xor_decompressor.available();
    }
};


#endif //SERFNATIVE_SERFDECOMPRESSOR_H
