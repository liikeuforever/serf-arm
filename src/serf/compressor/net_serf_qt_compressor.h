#ifndef NET_SERF_QT_COMPRESSOR_H
#define NET_SERF_QT_COMPRESSOR_H

#include <memory>
#include <cstdint>
#include <cmath>

#include "serf/utils/array.h"
#include "serf/utils/output_bit_stream.h"
#include "serf/utils/double.h"
#include "serf/utils/elias_delta_codec.h"
#include "serf/utils/zig_zag_codec.h"

class NetSerfQtCompressor {
 public:
  explicit NetSerfQtCompressor(double error_bound);
  Array<uint8_t> Compress(double v);

 private:
  const double kMaxDiff;
  double pre_value_ = 2;
  std::unique_ptr<OutputBitStream> output_bit_stream_ = std::make_unique<OutputBitStream>(5 * 8);
};

#endif  // NET_SERF_QT_COMPRESSOR_H
