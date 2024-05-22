#include "serf/decompressor_32/serf_qt_decompressor_32.h"

SerfQtDecompressor32::SerfQtDecompressor32(const Array<uint8_t> &bs) {
  input_bit_stream_->SetBuffer(bs);
  block_size_ = input_bit_stream_->ReadInt(16);
  max_diff_ = Float::IntBitsToFloat(input_bit_stream_->ReadInt(32));
}

std::vector<float> SerfQtDecompressor32::Decompress() {
  std::vector<float> decompressedValueList;
  while (block_size_--) decompressedValueList.emplace_back(NextValue());
  return decompressedValueList;
}

float SerfQtDecompressor32::NextValue() {
  long decodeValue = ZigZagCodec::Decode(EliasDeltaCodec::Decode(input_bit_stream_.get()) - 1);
  float recoverValue = pre_value_ + 2 * max_diff_ * static_cast<float>(decodeValue);
  pre_value_ = recoverValue;
  return recoverValue;
}
