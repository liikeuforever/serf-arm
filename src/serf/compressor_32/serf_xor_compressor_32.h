#ifndef SERF_XOR_COMPRESSOR_32_H
#define SERF_XOR_COMPRESSOR_32_H

#include <cmath>
#include <cstdint>
#include <memory>

#include "serf/utils/output_bit_stream.h"
#include "serf/utils/float.h"
#include "serf/utils/array.h"
#include "serf/utils/serf_utils_32.h"
#include "serf/utils/post_office_solver_32.h"

class SerfXORCompressor32 {
 public:
  SerfXORCompressor32(int capacity, float max_diff);
  void AddValue(float v);
  long compressed_size_in_bits() const;
  Array<uint8_t> out_buffer();
  void Close();

 private:
  const float kMaxDiff;
  uint32_t stored_val_ = Float::FloatToIntBits(2);

  long compressed_size_in_bits_;
  long stored_compressed_size_in_bits_ = 0;

  std::unique_ptr<OutputBitStream> output_bit_stream_;
  Array<uint8_t> out_buffer_ = Array<uint8_t>(0);

  int number_of_values_ = 0;
  double stored_compression_ratio_ = 0;

  int equal_vote_ = 0;
  bool equal_win_ = false;

  Array<int> leading_representation_ = {
      0, 0, 0, 0, 0, 0, 0, 0,
      1, 1, 1, 1, 2, 2, 2, 2,
      3, 3, 3, 3, 3, 3, 3, 3,
      3, 3, 3, 3, 3, 3, 3, 3,
  };
  Array<int> leading_round_ = {
      0, 0, 0, 0, 0, 0, 0, 0,
      8, 8, 8, 8, 12, 12, 12, 12,
      16, 16, 16, 16, 16, 16, 16, 16,
      16, 16, 16, 16, 16, 16, 16, 16
  };
  Array<int> trailing_representation_ = {
      0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0,
      1, 1, 1, 1, 1, 1, 1, 1,
      1, 1, 1, 1, 1, 1, 1, 1
  };
  Array<int> trailing_round_ = {
      0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0,
      16, 16, 16, 16, 16, 16, 16, 16,
      16, 16, 16, 16, 16, 16, 16, 16
  };

  int leading_bits_per_value_ = 2;
  int trailing_bits_per_value_ = 1;
  Array<int> lead_distribution_ = Array<int>(32);
  Array<int> trail_distribution_ = Array<int>(32);
  int stored_leading_zeros_ = std::numeric_limits<int>::max();
  int stored_trailing_zeros_ = std::numeric_limits<int>::max();

  int CompressValue(uint32_t value);
  int UpdateFlagAndPositionsIfNeeded();
};

#endif  // SERF_XOR_COMPRESSOR_32_H
