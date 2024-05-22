#include "serf/compressor_32/serf_qt_compressor_32.h"

SerfQtCompressor32::SerfQtCompressor32(int block_size, float max_diff): kMaxDiff(max_diff) {
  output_bit_stream_ = std::make_unique<OutputBitStream>(2 * block_size * 8);
  compressed_size_in_bits_ += output_bit_stream_->WriteInt(block_size, 16);
  compressed_size_in_bits_ += output_bit_stream_->WriteInt(Float::FloatToIntBits(kMaxDiff), 32);
}

void SerfQtCompressor32::AddValue(float v) {
  long q = static_cast<long>(std::round((v - pre_value_) / (2 * kMaxDiff)));
  float recover_value = pre_value_ + 2 * kMaxDiff * static_cast<float>(q);
  compressed_size_in_bits_ += EliasDeltaCodec::Encode(ZigZagCodec::Encode(q) + 1,
                                                      output_bit_stream_.get());
  pre_value_ = recover_value;
}

Array<uint8_t> SerfQtCompressor32::GetBytes() {
    Array<uint8_t> result = output_bit_stream_->GetBuffer(std::ceil(stored_compressed_size_in_bits_ / 8.0));
    output_bit_stream_->Refresh();
    return result;
}

void SerfQtCompressor32::Close() {
  output_bit_stream_->Flush();
  pre_value_ = 2;
  stored_compressed_size_in_bits_ = compressed_size_in_bits_;
  compressed_size_in_bits_ = 0;
}

long SerfQtCompressor32::compressed_size_in_bits() const {
    return compressed_size_in_bits_;
}