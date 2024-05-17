#include "SerfQtCompressor.h"

void SerfQtCompressor::AddValue(double v) {
    double q = std::round((v - pre_value_) / (2 * max_diff_));
    double recoverValue = pre_value_ + 2 * max_diff_ * q;
    compressed_size_in_bits_ += EliasDeltaCodec::encode(ZigZagCodec::encode(static_cast<int64_t>(q)) + 1, output_bit_stream_.get());
    pre_value_ = recoverValue;
}

Array<uint8_t> SerfQtCompressor::GetBytes() {
    Array<uint8_t> result(static_cast<int>(std::ceil(static_cast<double>(stored_compressed_size_in_bits_) / 8.0)));
    __builtin_memcpy(result._data.get(), output_bit_stream_->getBuffer(), result.length);
    output_bit_stream_->refresh();
    return result;
}

void SerfQtCompressor::Close() {
    output_bit_stream_->flush();
    pre_value_ = 2;
    stored_compressed_size_in_bits_ = compressed_size_in_bits_;
    compressed_size_in_bits_ = 0;
}

long SerfQtCompressor::get_compressed_size_in_bits() const {
    return stored_compressed_size_in_bits_;
}
