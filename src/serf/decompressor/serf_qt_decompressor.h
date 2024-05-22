#ifndef SERF_QT_DECOMPRESSOR_H
#define SERF_QT_DECOMPRESSOR_H

#include <memory>
#include <vector>

#include "serf/utils/double.h"
#include "serf/utils/input_bit_stream.h"
#include "serf/utils/zig_zag_codec.h"
#include "serf/utils/elias_delta_codec.h"

class SerfQtDecompressor {
 public:
  explicit SerfQtDecompressor(const Array<uint8_t> &bs);

  std::vector<double> Decompress();

 private:
  int block_size_;
  double max_diff_;
  std::unique_ptr<InputBitStream> input_bit_stream_ = std::make_unique<InputBitStream>();
  double pre_value_;

  double NextValue();
};

#endif //SERF_QT_DECOMPRESSOR_H
