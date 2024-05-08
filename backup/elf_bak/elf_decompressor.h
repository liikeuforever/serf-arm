#ifndef ELF_DECOMPRESSOR_H
#define ELF_DECOMPRESSOR_H


#include <vector>
#include <limits>
#include <memory>
#include <cmath>

#include "serf/utils/InputBitStream.h"
#include "serf/utils/Double.h"
#include "elf/elf_utils.h"

class AbstractElfDecompressor {
public:
    std::vector<double> decompress();

protected:
    virtual double xorDecompress() = 0;
    virtual int readInt(int len) = 0;

private:
    int last_beta_star = std::numeric_limits<int>::max();

    double nextValue();

    double recoverVByBetaStar();
};


class ElfXORDecompressor {
public:
    explicit ElfXORDecompressor(const Array<uint8_t>& bs) {
        input_bit_stream_ = std::make_unique<InputBitStream>();
        input_bit_stream_->setBuffer(bs);
    }

    std::vector<double> getValues();

    InputBitStream *get_input_bit_stream();

    double readValue();

private:
    uint64_t stored_val_ = 0;
    int stored_leading_zeros_ = std::numeric_limits<int>::max();
    int stored_trailing_zeros_ = std::numeric_limits<int>::max();
    bool first_ = true;
    std::unique_ptr<InputBitStream> input_bit_stream_;
    static constexpr short leading_rep_[] = {0, 8, 12, 16, 18, 20, 22, 24};

    void next();
    void nextValue();
};


class ElfDecompressor : public AbstractElfDecompressor {
public:
    explicit ElfDecompressor(const Array<uint8_t>& bytes) {
        xor_decompressor_ = std::make_unique<ElfXORDecompressor>(bytes);
    }

protected:
    double xorDecompress() override {
        return xor_decompressor_->readValue();
    }

    int readInt(int len) override {
        InputBitStream *input_bit_stream = xor_decompressor_->get_input_bit_stream();
        return input_bit_stream->readInt(len);
    }

private:
    std::unique_ptr<ElfXORDecompressor> xor_decompressor_;
};


#endif // ELF_DECOMPRESSOR_H
