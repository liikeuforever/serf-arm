#include "compressor/net_serf_xor_compressor.h"

NetSerfXORCompressor::NetSerfXORCompressor(int capacity, double max_diff, long adjust_digit): kBlockSize(capacity),
                                                                                              kMaxDiff(max_diff), kAdjustDigit(adjust_digit) {
  output_bit_stream_ = std::make_unique<OutputBitStream>(5 * 64);
}

Array<uint8_t> NetSerfXORCompressor::Compress(double v) {
  uint64_t this_val;
  // note we cannot let > maxDiff, because NaN - v > maxDiff is always false
  if (std::abs(Double::LongBitsToDouble(stored_val_) - kAdjustDigit - v) > kMaxDiff) {
    // in our implementation, we do not consider special cases and overflow case
    double adjust_value = v + kAdjustDigit;
    this_val = SerfUtils64::FindAppLong(adjust_value - kMaxDiff, adjust_value + kMaxDiff, v, stored_val_,
                                        kMaxDiff, kAdjustDigit);
  } else {
    // let current value be the last value, making an XORed value of 0.
    this_val = stored_val_;
  }
  Array<uint8_t> result = AddValue(this_val);
  stored_val_ = this_val;
  return result;
}

Array<uint8_t> NetSerfXORCompressor::AddValue(uint64_t value) {
  // Reserve 4 bits for transition header
  int this_size = output_bit_stream_->WriteInt(0, 4);
  if (number_of_values_ >= kBlockSize) {
    this_size += UpdateFlagAndPositionsIfNeeded();
  }
  this_size += CompressValue(value);
  compressed_size_in_bits_ += this_size;
  output_bit_stream_->Flush();
  Array<uint8_t> ret = output_bit_stream_->GetBuffer(std::ceil(this_size / 8.0));
  output_bit_stream_->Refresh();
  ++number_of_values_;
  return ret;
}

int NetSerfXORCompressor::CompressValue(uint64_t value) {
  int this_size = 0;
  uint64_t xor_result = stored_val_ ^ value;

  if (xor_result == 0) {
    // case 01
    if (equal_win_) {
      this_size += output_bit_stream_->WriteBit(true);
    } else {
      this_size += output_bit_stream_->WriteInt(1, 2);
    }
    equal_vote_++;
  } else {
    int leading_count = __builtin_clzll(xor_result);
    int trailing_count = __builtin_ctzll(xor_result);
    int leading_zeros = leading_round_[leading_count];
    int trailing_zeros = trailing_round_[trailing_count];
    ++lead_distribution_[leading_count];
    ++trail_distribution_[trailing_count];

    if (leading_zeros >= stored_leading_zeros_ && trailing_zeros >= stored_trailing_zeros_ &&
        (leading_zeros - stored_leading_zeros_) + (trailing_zeros - stored_trailing_zeros_)
            < 1 + leading_bits_per_value_ + trailing_bits_per_value_) {
      // case 1
      int center_bits = 64 - stored_leading_zeros_ - stored_trailing_zeros_;
      int len;
      if (equal_win_) {
        len = 2 + center_bits;
        if (len > 64) {
          output_bit_stream_->WriteInt(1, 2);
          output_bit_stream_->WriteLong(xor_result >> stored_trailing_zeros_, center_bits);
        } else {
          output_bit_stream_->WriteLong((1ULL << center_bits) | (xor_result >> stored_trailing_zeros_), 2 +
          center_bits);
        }
      } else {
        len = 1 + center_bits;
        if (len > 64) {
          output_bit_stream_->WriteInt(1, 1);
          output_bit_stream_->WriteLong(xor_result >> stored_trailing_zeros_, center_bits);
        } else {
          output_bit_stream_->WriteLong((1ULL << center_bits) | (xor_result >> stored_trailing_zeros_), 1 +
          center_bits);
        }
      }
      this_size += len;
      equal_vote_--;
    } else {
      stored_leading_zeros_ = leading_zeros;
      stored_trailing_zeros_ = trailing_zeros;
      int center_bits = 64 - stored_leading_zeros_ - stored_trailing_zeros_;

      // case 00
      int len = 2 + leading_bits_per_value_ + trailing_bits_per_value_ + center_bits;
      if (len > 64) {
        output_bit_stream_->WriteInt((leading_representation_[stored_leading_zeros_] << trailing_bits_per_value_) |
        trailing_representation_[stored_trailing_zeros_], 2 + leading_bits_per_value_ + trailing_bits_per_value_);
        output_bit_stream_->WriteLong(xor_result >> stored_trailing_zeros_, center_bits);
      } else {
        output_bit_stream_->WriteLong(((((uint64_t) leading_representation_[stored_leading_zeros_] <<
        trailing_bits_per_value_) | trailing_representation_[stored_trailing_zeros_]) << center_bits) | (xor_result
        >> stored_trailing_zeros_), len);
      }
      this_size += len;
    }
  }
  return this_size;
}

int NetSerfXORCompressor::UpdateFlagAndPositionsIfNeeded() {
  int len;
  equal_win_ = equal_vote_ > 0;
  double this_compression_ratio = compressed_size_in_bits_ / (number_of_values_ * 64.0);
  if (stored_compression_ratio_ < this_compression_ratio) {
    // update positions
    Array<int> lead_positions = PostOfficeSolver::InitRoundAndRepresentation(lead_distribution_, leading_representation_, leading_round_);
    leading_bits_per_value_ = PostOfficeSolver::kPositionLength2Bits[lead_positions.length()];
    Array<int> trail_positions = PostOfficeSolver::InitRoundAndRepresentation(trail_distribution_, trailing_representation_, trailing_round_);
    trailing_bits_per_value_ = PostOfficeSolver::kPositionLength2Bits[trail_positions.length()];
    len = output_bit_stream_->WriteInt(equal_win_ ? 3 : 1, 2)
        + PostOfficeSolver::WritePositions(lead_positions, output_bit_stream_.get())
        + PostOfficeSolver::WritePositions(trail_positions, output_bit_stream_.get());
  } else {
    len = output_bit_stream_->WriteInt(equal_win_ ? 2 : 0, 2);
  }
  equal_vote_ = 0;
  stored_compression_ratio_ = this_compression_ratio;
  compressed_size_in_bits_ = 0;
  number_of_values_ = 0;
  __builtin_memset(lead_distribution_.begin(), 0, 64 * sizeof(int));
  __builtin_memset(trail_distribution_.begin(), 0, 64 * sizeof(int));
  return len;
}
