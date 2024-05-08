#include "elf_decompressor.h"

double AbstractElfDecompressor::recoverVByBetaStar() {
    double v;
    double vPrime = xorDecompress();
    int sp = ElfUtils::getSP(std::abs(vPrime));
    if (last_beta_star == 0) {
        v = ElfUtils::get10iN(-sp - 1);
        if (vPrime < 0) {
            v = -v;
        }
    } else {
        int alpha = last_beta_star - sp - 1;
        v = ElfUtils::roundUp(vPrime, alpha);
    }
    return v;
}

double AbstractElfDecompressor::nextValue() {
    double v;
    if (readInt(1) == 0) {
        v = recoverVByBetaStar();
    } else if (readInt(1) == 0) {
        v = xorDecompress();
    } else {
        last_beta_star = readInt(4);
        v = recoverVByBetaStar();
    }
    return v;
}

std::vector<double> AbstractElfDecompressor::decompress() {
    std::vector<double> values;
    double cur_value;
    while (!std::isnan(cur_value = nextValue())) {
        values.emplace_back(cur_value);
    }
    return values;
}

void ElfXORDecompressor::nextValue() {
    uint64_t value;
    uint32_t center_bits, lead_and_center;
    uint32_t flag = input_bit_stream_->readInt(2);
    if (flag == 3) {
        // case 11
        lead_and_center = input_bit_stream_->readInt(9);
        stored_leading_zeros_ = leading_rep_[lead_and_center >> 6];
        center_bits = lead_and_center & 0x3f;
        if (center_bits == 0) {
            center_bits = 64;
        }
        stored_trailing_zeros_ = 64 - stored_leading_zeros_ - center_bits;
        value = ((input_bit_stream_->readLong(center_bits - 1) << 1) + 1) << stored_trailing_zeros_;
        value = stored_val_ ^ value;
        stored_val_ = value;
    } else if (flag == 2) {
        // case 10
        lead_and_center = input_bit_stream_->readInt(7);
        stored_leading_zeros_ = leading_rep_[lead_and_center >> 4];
        center_bits = lead_and_center & 0xf;
        if (center_bits == 0) {
            center_bits = 16;
        }
        stored_trailing_zeros_ = 64 - stored_leading_zeros_ - center_bits;
        value = ((input_bit_stream_->readLong(center_bits - 1) << 1) + 1) << stored_trailing_zeros_;
        value = stored_val_ ^ value;
        stored_val_ = value;
    } else if (flag == 1) {
        // case 01
        return;
    } else {
        // case 00
        center_bits = 64 - stored_leading_zeros_ - stored_trailing_zeros_;
        value = input_bit_stream_->readLong(center_bits) << stored_trailing_zeros_;
        value = stored_val_ ^ value;
        stored_val_ = value;
    }
}

void ElfXORDecompressor::next() {
    if (first_) {
        first_ = false;
        uint32_t trailing_zeros = input_bit_stream_->readInt(7);
        if (trailing_zeros < 64) {
            stored_val_ = ((input_bit_stream_->readLong(63 - trailing_zeros) << 1) + 1) << trailing_zeros;
        } else {
            stored_val_ = 0;
        }
    } else {
        nextValue();
    }
}

double ElfXORDecompressor::readValue() {
    return Double::longBitsToDouble(stored_val_);
}

InputBitStream *ElfXORDecompressor::get_input_bit_stream() {
    return input_bit_stream_.get();
}

std::vector<double> ElfXORDecompressor::getValues() {
    std::vector<double> values;
    double cur_value;
    while (!std::isnan(cur_value = readValue())) {
        values.emplace_back(cur_value);
    }
    return values;
}
