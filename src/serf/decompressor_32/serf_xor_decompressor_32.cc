#include "serf/decompressor_32/serf_xor_decompressor_32.h"

std::vector<float> SerfXORDecompressor32::Decompress(const Array<uint8_t> &bs) {
  input_bit_stream_->SetBuffer(bs);
  UpdateFlagAndPositionsIfNeeded();
  std::vector<float> values;
  uint32_t value;
  while ((value = ReadValue()) != Float::FloatToIntBits(Float::kNan)) {
    values.emplace_back(Float::IntBitsToFloat(value));
    stored_val_ = value;
  }
  return values;
}

uint32_t SerfXORDecompressor32::ReadValue() {
  uint32_t value = stored_val_;
  int center_bits;

  if (equal_win_) {
    if (input_bit_stream_->ReadInt(1) == 0) {
      if (input_bit_stream_->ReadInt(1) != 1) {
        // case 00
        int lead_and_trail = static_cast<int>(input_bit_stream_->ReadInt(
            leading_bits_per_value_ + trailing_bits_per_value_));
        int lead = lead_and_trail >> trailing_bits_per_value_;
        int trail = ~(0xffff << trailing_bits_per_value_) & lead_and_trail;
        stored_leading_zeros_ = leading_representation_[lead];
        stored_trailing_zeros_ = trailing_representation_[trail];
      }
      center_bits = 32 - stored_leading_zeros_ - stored_trailing_zeros_;
      value = input_bit_stream_->ReadInt(center_bits) << stored_trailing_zeros_;
      value = stored_val_ ^ value;
    }
  } else {
    if (input_bit_stream_->ReadInt(1) == 1) {
      // case 1
      center_bits = 32 - stored_leading_zeros_ - stored_trailing_zeros_;

      value = input_bit_stream_->ReadInt(center_bits) << stored_trailing_zeros_;
      value = stored_val_ ^ value;
    } else if (input_bit_stream_->ReadInt(1) == 0) {
      // case 00
      int lead_and_trail = static_cast<int>(input_bit_stream_->ReadInt(
          leading_bits_per_value_ + trailing_bits_per_value_));
      int lead = lead_and_trail >> trailing_bits_per_value_;
      int trail = ~(0xffff << trailing_bits_per_value_) & lead_and_trail;
      stored_leading_zeros_ = leading_representation_[lead];
      stored_trailing_zeros_ = trailing_representation_[trail];
      center_bits = 32 - stored_leading_zeros_ - stored_trailing_zeros_;

      value = input_bit_stream_->ReadInt(center_bits) << stored_trailing_zeros_;
      value = stored_val_ ^ value;
    }
  }
  return value;
}

void SerfXORDecompressor32::UpdateFlagAndPositionsIfNeeded() {
  equal_win_ = input_bit_stream_->ReadBit() == 1;
  if (input_bit_stream_->ReadBit() == 1) {
    UpdateLeadingRepresentation();
    UpdateTrailingRepresentation();
  }
}

void SerfXORDecompressor32::UpdateLeadingRepresentation() {
  int num = static_cast<int>(input_bit_stream_->ReadInt(4));
  if (num == 0) {
    num = 16;
  }
  leading_bits_per_value_ = PostOfficeSolver32::kPositionLength2Bits[num];
  leading_representation_ = Array<int>(num);
  for (int i = 0; i < num; i++) {
    leading_representation_[i] = static_cast<int>(input_bit_stream_->ReadInt(5));
  }
}

void SerfXORDecompressor32::UpdateTrailingRepresentation() {
  int num = static_cast<int>(input_bit_stream_->ReadInt(4));
  if (num == 0) {
    num = 16;
  }
  trailing_bits_per_value_ = PostOfficeSolver32::kPositionLength2Bits[num];
  trailing_representation_ = Array<int>(num);
  for (int i = 0; i < num; i++) {
    trailing_representation_[i] = static_cast<int>(input_bit_stream_->ReadInt(5));
  }
}
