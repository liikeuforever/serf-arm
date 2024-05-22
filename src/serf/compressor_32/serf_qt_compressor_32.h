#ifndef SERF_QT_COMPRESSOR_32_H
#define SERF_QT_COMPRESSOR_32_H

#include <cstdint>
#include <memory>
#include <cmath>

#include "serf/utils/output_bit_stream.h"
#include "serf/utils/float.h"
#include "serf/utils/elias_delta_codec.h"
#include "serf/utils/zig_zag_codec.h"

class SerfQtCompressor32 {
 public:
  explicit SerfQtCompressor32(int block_size, float max_diff);
  void AddValue(float v);
  Array<uint8_t> GetBytes();
  void Close();
  long compressed_size_in_bits() const;

 private:
  const float kMaxDiff;
  std::unique_ptr<OutputBitStream> output_bit_stream_;
  float pre_value_ = 2;
  long compressed_size_in_bits_ = 0;
  long stored_compressed_size_in_bits_ = 0;
};

#endif  // SERF_QT_COMPRESSOR_32_H
