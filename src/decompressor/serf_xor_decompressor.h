#ifndef SERF_XOR_DECOMPRESSOR_H
#define SERF_XOR_DECOMPRESSOR_H

#include <cstdint>
#include <memory>
#include <vector>

#include "serf/src/utils/post_office_solver.h"
#include "serf/src/utils/input_bit_stream.h"
#include "serf/src/utils/double.h"
#include "serf/src/utils/array.h"
#include "serf/src/utils/serf_utils_64.h"

class SerfXORDecompressor {
 public:
  explicit SerfXORDecompressor(long adjust_digit) : adjust_digit_(adjust_digit) {};

  std::vector<double> Decompress(const Array<uint8_t> &bs);

 private:
  uint64_t stored_val_ = Double::DoubleToLongBits(2);
  int stored_leading_zeros_ = std::numeric_limits<int>::max();
  int stored_trailing_zeros_ = std::numeric_limits<int>::max();
  InputBitStream input_bit_stream_;
  Array<int> leading_representation_ = {0, 8, 12, 16, 18, 20, 22, 24};
  Array<int> trailing_representation_ = {0, 22, 28, 32, 36, 40, 42, 46};
  int leading_bits_per_value_ = 3;
  int trailing_bits_per_value_ = 3;
  bool equal_win_ = false;
  long adjust_digit_;

  uint64_t ReadValueEqualWinTrue();
  uint64_t ReadValueEqualWinFalse();
  void UpdateFlagAndPositionsIfNeeded();
  void UpdateLeadingRepresentation();
  void UpdateTrailingRepresentation();
};

#endif  // SERF_XOR_DECOMPRESSOR_H
