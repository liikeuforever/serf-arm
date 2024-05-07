#include "buff_compressor.h"

BuffCompressor::BuffCompressor(int batch_size) {
    batch_size_ = batch_size;
    output_bit_stream_ = std::make_unique<OutputBitStream>(100000);
    size_ = 0;
}

int BuffCompressor::getWidthNeeded(uint64_t number) {
    if (number == 0) return 0;
    int bit_count = 0;
    while (number > 0) {
        bit_count++;
        number = number >> 1;
    }
    return bit_count;
}

std::string BuffCompressor::toStringWithPrecision(double val, int precision) {
    std::ostringstream stringBuffer;
    stringBuffer << std::setprecision(precision) << std::fixed << val;
    return stringBuffer.str();
}

int BuffCompressor::getDecimalPlace(double v) {
    if (v == 0.0) return 0;
    std::string str_double = toStringWithPrecision(v, 16);
    int index_of_decimal_point = (int) str_double.find('.');
    int index_of_last_zero = (int) str_double.find('0');
    return index_of_last_zero - index_of_decimal_point - 1;
}

SparseResult BuffCompressor::findMajority(Array<uint8_t> nums) {
    SparseResult result(batch_size_);
    uint8_t candidate = 0;
    int count = 0;

    for (const auto &num: nums) {
        if (count = 0) {
            candidate = num;
            count = 1;
        } else if (num == candidate) {
            count++;
        } else {
            count--;
        }
    }

    count = 0;
    for (int i = 0; i < nums.length; ++i) {
        int index = i / 8;
        result.bitmap_[index] = (uint8_t) (result.bitmap_[index] << 1);
        if (nums[i] == candidate) {
            count++;
        } else {
            result.bitmap_[index] = (uint8_t) (result.bitmap_[index] | 0b1);
            result.outliers_[result.outliers_count_++] = nums[i];
        }
        if (i + 1 == nums.length && (i + 1) % 8 != 0) {
            // Here this line of code differs from that in Java
            result.bitmap_[index] = (uint8_t) (result.bitmap_[index] << ((i % 8) + 1));
        }
    }

    if (count >= nums.length * 0.9) {
        result.flag_ = true;
        result.frequent_value_ = candidate;
    } else {
        result.flag_ = false;
    }
    return result;
}

Array<uint8_t> BuffCompressor::get_out() {
    uint8_t *data_ptr = output_bit_stream_->getBuffer();
    Array<uint8_t> out = Array<uint8_t>(std::ceil(size_ / 8.0));
    for (int i = 0; i < out.length; ++i) {
        out[i] = data_ptr[i];
    }
    return out;
}

void BuffCompressor::compress(std::vector<double> values) {

}

void BuffCompressor::wholeWidthLongCompress(std::vector<double> values) {
    for (const auto &value: values) {
        size_ += output_bit_stream_->writeLong(Double::doubleToLongBits(value), 64);
    }
}

void BuffCompressor::close() {
    size_ += output_bit_stream_->writeInt(0, 8);
}

long BuffCompressor::get_size() {
    return size_;
}
