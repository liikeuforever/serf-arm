#include "serf/compressor_32/serf_xor_compressor_32.h"

SerfXORCompressor32::SerfXORCompressor32(int capacity, float max_diff) : kMaxDiff(max_diff) {
  output_bit_stream_ = std::make_unique<OutputBitStream>(std::floor(((capacity + 1) * 4 + capacity / 4 + 1) * 1.2));
  compressed_size_in_bits_ = output_bit_stream_->WriteInt(0, 2);
}

void SerfXORCompressor32::AddValue(float v) {
  uint32_t thisVal;
  // note we cannot let > maxDiff, because kNan - v > maxDiff is always false
  if (std::abs(Float::IntBitsToFloat(stored_val_) - v) > kMaxDiff) {
    // in our implementation, we do not consider special cases and overflow case
    thisVal = SerfUtils32::FindAppInt(v - kMaxDiff, v + kMaxDiff, v,
                                      stored_val_, kMaxDiff);
  } else {
    // let current value be the last value, making an XORed value of 0.
    thisVal = stored_val_;
  }

  compressed_size_in_bits_ += CompressValue(thisVal);
  stored_val_ = thisVal;
  ++number_of_values_;
}

long SerfXORCompressor32::compressed_size_in_bits() const {
  return compressed_size_in_bits_;
}

Array<uint8_t> SerfXORCompressor32::out_buffer() {
  return out_buffer_;
}

void SerfXORCompressor32::Close() {
  compressed_size_in_bits_ += CompressValue(Float::FloatToIntBits(Float::kNan));
  output_bit_stream_->Flush();
  Array<uint8_t> result = output_bit_stream_->GetBuffer(std::ceil(compressed_size_in_bits_ / 8.0));
  output_bit_stream_->Refresh();
  stored_compressed_size_in_bits_ = compressed_size_in_bits_;
  compressed_size_in_bits_ = UpdateFlagAndPositionsIfNeeded();
}

int SerfXORCompressor32::CompressValue(uint32_t value) {
  int this_size = 0;
  uint32_t xor_result = stored_val_ ^ value;

  if (xor_result == 0) {
    // case 01
    if (equal_win_) {
      this_size += static_cast<int>(output_bit_stream_->WriteBit(true));
    } else {
      this_size += static_cast<int>(output_bit_stream_->WriteInt(1, 2));
    }
    equal_vote_++;
  } else {
    int leading_count = __builtin_clz(xor_result);
    int trailing_count = __builtin_ctz(xor_result);
    int leading_zeros = leading_round_[leading_count];
    int trailing_zeros = trailing_round_[trailing_count];
    ++lead_distribution_[leading_count];
    ++trail_distribution_[trailing_count];

    if (leading_zeros >= stored_leading_zeros_ && trailing_zeros >= stored_trailing_zeros_ &&
        (leading_zeros - stored_leading_zeros_) + (trailing_zeros - stored_trailing_zeros_)
            < 1 + leading_bits_per_value_ + trailing_bits_per_value_) {
      // case 1
      int center_bits = 32 - stored_leading_zeros_ - stored_trailing_zeros_;
      int len;
      if (equal_win_) {
        len = 2 + center_bits;
        if (len > 32) {
          output_bit_stream_->WriteInt(1, 2);
          output_bit_stream_->WriteInt(xor_result >> stored_trailing_zeros_, center_bits);
        } else {
          output_bit_stream_->WriteInt((1 << center_bits) |
                                           (xor_result >> stored_trailing_zeros_),
                                       2 + center_bits);
        }
      } else {
        len = 1 + center_bits;
        if (len > 32) {
          output_bit_stream_->WriteInt(1, 1);
          output_bit_stream_->WriteInt(xor_result >> stored_trailing_zeros_, center_bits);
        } else {
          output_bit_stream_->WriteInt((1 << center_bits) |
                                           (xor_result >> stored_trailing_zeros_),
                                       1 + center_bits);
        }
      }
      this_size += len;
      equal_vote_--;
    } else {
      stored_leading_zeros_ = leading_zeros;
      stored_trailing_zeros_ = trailing_zeros;
      int center_bits = 32 - stored_leading_zeros_ - stored_trailing_zeros_;

      // case 00
      int len = 2 + leading_bits_per_value_ + trailing_bits_per_value_ + center_bits;
      if (len > 32) {
        output_bit_stream_->WriteInt((leading_representation_[stored_leading_zeros_]
                                         << trailing_bits_per_value_)
                                         | trailing_representation_[stored_trailing_zeros_],
                                     2 + leading_bits_per_value_ + trailing_bits_per_value_);
        output_bit_stream_->WriteInt(xor_result >> stored_trailing_zeros_, center_bits);
      } else {
        output_bit_stream_->WriteInt((((leading_representation_[stored_leading_zeros_]
            << trailing_bits_per_value_)
            | trailing_representation_[stored_trailing_zeros_])
            << center_bits)
                                         | (xor_result >> stored_trailing_zeros_), len);
      }
      this_size += len;
    }
  }
  return this_size;
}

int SerfXORCompressor32::UpdateFlagAndPositionsIfNeeded() {
  int len;
  equal_win_ = equal_vote_ > 0;
  double this_compression_ratio = static_cast<double>(compressed_size_in_bits_) / (number_of_values_ * 32.0);
  if (stored_compression_ratio_ < this_compression_ratio) {
    // update positions
    Array<int> lead_positions = PostOfficeSolver32::InitRoundAndRepresentation(
        lead_distribution_, leading_representation_, leading_round_);
    leading_bits_per_value_ = PostOfficeSolver32::kPositionLength2Bits[lead_positions.length()];
    Array<int> trail_positions = PostOfficeSolver32::InitRoundAndRepresentation(
        trail_distribution_, trailing_representation_, trailing_round_);
    trailing_bits_per_value_ = PostOfficeSolver32::kPositionLength2Bits[trail_positions.length()];
    len = static_cast<int>(output_bit_stream_->WriteInt(equal_win_ ? 3 : 1, 2))
        + PostOfficeSolver32::WritePositions(lead_positions, output_bit_stream_.get())
        + PostOfficeSolver32::WritePositions(trail_positions, output_bit_stream_.get());
  } else {
    len = static_cast<int>(output_bit_stream_->WriteInt(equal_win_ ? 2 : 0, 2));
  }
  equal_vote_ = 0;
  stored_compression_ratio_ = this_compression_ratio;
  number_of_values_ = 0;
  __builtin_memset(lead_distribution_.begin(), 0, 32 * sizeof(int));
  __builtin_memset(trail_distribution_.begin(), 0, 32 * sizeof(int));
  return len;
}
