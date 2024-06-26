#ifndef SERF_QT_DECOMPRESSOR_H
#define SERF_QT_DECOMPRESSOR_H

#include <memory>
#include <vector>

#include "serf/src/utils/double.h"
#include "serf/src/utils/input_bit_stream.h"
#include "serf/src/utils/zig_zag_codec.h"
#include "serf/src/utils/elias_delta_codec.h"

class SerfQtDecompressor {
 public:
  SerfQtDecompressor() = default;
  std::vector<double> Decompress(const Array<uint8_t> &bs);

 private:
  int block_size_ = 0;
  double max_diff_ = 0;
  std::unique_ptr<InputBitStream> input_bit_stream_ = std::make_unique<InputBitStream>();
  double pre_value_ = 2;

  double NextValue();
};

#endif //SERF_QT_DECOMPRESSOR_H
