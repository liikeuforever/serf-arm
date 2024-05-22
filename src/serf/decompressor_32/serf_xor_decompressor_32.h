#ifndef SERF_XOR_DECOMPRESSOR_32_H
#define SERF_XOR_DECOMPRESSOR_32_H

#include <limits>
#include <memory>
#include <vector>

#include "serf/utils/serf_utils_32.h"
#include "serf/utils/post_office_solver_32.h"
#include "serf/utils/float.h"
#include "serf/utils/input_bit_stream.h"

class SerfXORDecompressor32 {
 public:
  SerfXORDecompressor32() = default;
  std::vector<float> Decompress(const Array<uint8_t> &bs);

 private:
  uint32_t stored_val_ = Float::FloatToIntBits(2);
  int stored_leading_zeros_ = std::numeric_limits<int>::max();
  int stored_trailing_zeros_ = std::numeric_limits<int>::max();
  std::unique_ptr<InputBitStream> input_bit_stream_ = std::make_unique<InputBitStream>();
  Array<int> leading_representation_ = {0, 8, 12, 16};
  Array<int> trailing_representation_ = {0, 16};
  int leading_bits_per_value_ = 2;
  int trailing_bits_per_value_ = 1;
  bool equal_win_ = false;

  uint32_t ReadValue();
  void UpdateFlagAndPositionsIfNeeded();
  void UpdateLeadingRepresentation();
  void UpdateTrailingRepresentation();
};

#endif  // SERF_XOR_DECOMPRESSOR_32_H
