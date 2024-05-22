#include "net_serf_qt_compressor.h"

NetSerfQtCompressor::NetSerfQtCompressor(double errorBound) : kMaxDiff(errorBound) {}

Array<uint8_t> NetSerfQtCompressor::Compress(double v) {
  int written_bits_count = output_bit_stream_->WriteInt(0, 4);
  double q = std::round((v - pre_value_) / (2 * kMaxDiff));
  double recoverValue = pre_value_ + 2 * kMaxDiff * q;
  written_bits_count += EliasDeltaCodec::Encode(ZigZagCodec::Encode(static_cast<int64_t>(q)) + 1,
                                                      output_bit_stream_.get());
  pre_value_ = recoverValue;
  output_bit_stream_->Flush();
  Array<uint8_t> result = output_bit_stream_->GetBuffer(std::ceil(written_bits_count / 8.0));
  output_bit_stream_->Refresh();
  return result;
}

