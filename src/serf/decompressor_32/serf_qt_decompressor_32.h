#ifndef SERF_QT_DECOMPRESSOR_32_H
#define SERF_QT_DECOMPRESSOR_32_H

#include <vector>
#include <memory>

#include "serf/utils/zig_zag_codec.h"
#include "serf/utils/elias_delta_codec.h"
#include "serf/utils/float.h"
#include "serf/utils/input_bit_stream.h"

class SerfQtDecompressor32 {
 public:
  SerfQtDecompressor32() = default;
  std::vector<float> Decompress(const Array<uint8_t> &bs);

 private:
  int block_size_;
  float max_diff_;
  std::unique_ptr<InputBitStream> input_bit_stream_ = std::make_unique<InputBitStream>();
  float pre_value_ = 2;

  float NextValue();
};

#endif  // SERF_QT_DECOMPRESSOR_32_H
