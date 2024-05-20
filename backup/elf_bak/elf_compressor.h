#ifndef ELF_COMPRESSOR_H
#define ELF_COMPRESSOR_H


#include <cmath>
#include <cstdint>
#include <limits>
#include <memory>

#include "serf/utils/double.h"
#include "serf/utils/array.h"
#include "serf/utils/OutputBitStream.h"
#include "elf/elf_utils.h"

class AbstractElfCompressor {
public:
    void addValue(double v) {
        uint64_t v_long = Double::doubleToLongBits(v);
        uint64_t v_prime_long;

        if (v == 0.0 || std::isinf(v)) {
            size_ += writeInt(2, 2);
            v_prime_long = v_long;
//        } else if (std::isnan(v)) {
//            size_ += writeInt(2, 2);
//            v_prime_long = Double::doubleToLongBits(Double::NaN);
        } else {
            Array<int> alpha_and_beta_star = ElfUtils::getAlphaAndBetaStar(v, lastBetaStar_);
            int e = ((int) (v_long >> 52)) & 0x7ff;
            int gAlpha = ElfUtils::getFAlpha(alpha_and_beta_star[0]) + e - 1023;
            int erase_bits = 52 - gAlpha;
            uint64_t mask = 0xffffffffffffffffL << erase_bits;
            uint64_t delta = (~mask) & v_long;
            if (delta != 0 && erase_bits > 4) {
                if (alpha_and_beta_star[1] == lastBetaStar_) {
                    size_ += writeBit(false);
                } else {
                    size_ += writeInt(alpha_and_beta_star[1] | 0x30, 6);
                    lastBetaStar_ = alpha_and_beta_star[1];
                }
                v_prime_long = mask & v_long;
            } else {
                size_ += writeInt(2, 2);
                v_prime_long = v_long;
            }
        }
        size_ += xorCompress(v_prime_long);
    }

    int get_size() {
        return size_;
    }

protected:
    virtual int writeInt(int n, int len) = 0;
    virtual int writeBit(bool bit) = 0;
    virtual int xorCompress(uint64_t vPrimeLong) = 0;

private:
    int size_ = 0;
    int lastBetaStar_ = std::numeric_limits<int>::max();
};

class ElfXORCompressor {
public:
    ElfXORCompressor() {
        output_bit_stream_ = std::make_unique<OutputBitStream>(10000);
        size_ = 0;
    }

    OutputBitStream* get_output_bit_stream() {
        return output_bit_stream_.get();
    }

    int addValue(uint64_t value) {
        if (first_) {
            return writeFirst(value);
        } else {
            return compressValue(value);
        }
    }

    int get_size() {
        return size_;
    }

    Array<uint8_t> getOut() {
        uint8_t *data_ptr = output_bit_stream_->getBuffer();
        int compress_size_in_bytes = std::ceil(size_ / 8.0);
        Array<uint8_t> compress_pack = Array<uint8_t>(compress_size_in_bytes);
        for (int i = 0; i < compress_size_in_bytes; ++i) {
            compress_pack[i] = data_ptr[i];
        }
        return compress_pack;
    }

    void close() {
        addValue(Double::doubleToLongBits(Double::NaN));
        output_bit_stream_->writeBit(false);
        output_bit_stream_->flush();
    }

private:
    int stored_leading_zeros_ = std::numeric_limits<int>::max();
    int stored_trailing_zeros_ = std::numeric_limits<int>::max();
    uint64_t stored_val_ = 0;
    bool first_ = true;
    int size_;
    constexpr static short leading_rep_[] = {
            0, 0, 0, 0, 0, 0, 0, 0,
            1, 1, 1, 1, 2, 2, 2, 2,
            3, 3, 4, 4, 5, 5, 6, 6,
            7, 7, 7, 7, 7, 7, 7, 7,
            7, 7, 7, 7, 7, 7, 7, 7,
            7, 7, 7, 7, 7, 7, 7, 7,
            7, 7, 7, 7, 7, 7, 7, 7,
            7, 7, 7, 7, 7, 7, 7, 7
    };
    constexpr static short leading_rnd_[] = {
            0, 0, 0, 0, 0, 0, 0, 0,
            8, 8, 8, 8, 12, 12, 12, 12,
            16, 16, 18, 18, 20, 20, 22, 22,
            24, 24, 24, 24, 24, 24, 24, 24,
            24, 24, 24, 24, 24, 24, 24, 24,
            24, 24, 24, 24, 24, 24, 24, 24,
            24, 24, 24, 24, 24, 24, 24, 24,
            24, 24, 24, 24, 24, 24, 24, 24
    };
    std::unique_ptr<OutputBitStream> output_bit_stream_;

    int writeFirst(uint64_t value) {
        first_ = false;
        stored_val_ = value;
        int trailing_zeros = __builtin_ctzll(value);
        output_bit_stream_->writeInt(trailing_zeros, 7);
        if (value != 0) {
            output_bit_stream_->writeLong(stored_val_ >> (trailing_zeros + 1), 63 - trailing_zeros);
            size_ += 70 - trailing_zeros;
            return 70 - trailing_zeros;
        } else {
            size_ += 7;
            return 7;
        }
    }

    int compressValue(uint64_t value) {
        int this_size = 0;
        uint64_t xored_value = stored_val_ ^ value;

        if (xored_value == 0) {
            output_bit_stream_->writeInt(1, 2);
            size_ += 2;
            this_size += 2;
        } else {
            int leading_zeros = leading_rnd_[__builtin_clzll(xored_value)];
            int trailing_zeros = __builtin_ctzll(xored_value);

            if (leading_zeros == stored_leading_zeros_ && trailing_zeros >= stored_trailing_zeros_) {
                // case 00
                int center_bits = 64 - stored_leading_zeros_ - stored_trailing_zeros_;
                int len = 2 + center_bits;
                if (len > 64) {
                    output_bit_stream_->writeInt(0, 2);
                    output_bit_stream_->writeLong(xored_value >> stored_trailing_zeros_, center_bits);
                } else {
                    output_bit_stream_->writeLong(xored_value >> stored_trailing_zeros_, len);
                }

                size_ += len;
                this_size += len;
            } else {
                stored_leading_zeros_ = leading_zeros;
                stored_trailing_zeros_ = trailing_zeros;
                int center_bits = 64 - stored_leading_zeros_ - stored_trailing_zeros_;

                if (center_bits <= 16) {
                    // case 10
                    output_bit_stream_->writeInt((((0x2 << 3) | leading_rep_[stored_leading_zeros_]) << 4) | (center_bits & 0xf), 9);
                    output_bit_stream_->writeLong(xored_value >> (stored_trailing_zeros_ + 1), center_bits - 1);

                    size_ += 8 + center_bits;
                    this_size += 8 + center_bits;
                } else {
                    // case 11
                    output_bit_stream_->writeInt((((0x3 << 3) | leading_rep_[stored_leading_zeros_]) << 6) | (center_bits & 0x3f), 11);
                    output_bit_stream_->writeLong(xored_value >> (stored_trailing_zeros_ + 1), center_bits - 1);

                    size_ += 10 + center_bits;
                    this_size += 10 + center_bits;
                }
            }

            stored_val_ = value;
        }
        return this_size;
    }
};

class ElfCompressor : public AbstractElfCompressor{
public:
    ElfCompressor() {
        xor_compressor_ = std::make_unique<ElfXORCompressor>();
    }

    Array<uint8_t> getBytes() {
        return xor_compressor_->getOut();
    }

    void close() {
        writeInt(2, 2);
        xor_compressor_->close();
    }

protected:
    int writeInt(int n, int len) override {
        OutputBitStream *output_bit_stream = xor_compressor_->get_output_bit_stream();
        output_bit_stream->writeInt(n, len);
        return len;
    }

    int writeBit(bool bit) override {
        OutputBitStream *output_bit_stream = xor_compressor_->get_output_bit_stream();
        output_bit_stream->writeBit(bit);
        return 1;
    }

    int xorCompress(uint64_t vPrimeLong) override {
        return xor_compressor_->addValue(vPrimeLong);
    }

private:
    std::unique_ptr<ElfXORCompressor> xor_compressor_;
};


#endif // ELF_COMPRESSOR_H
