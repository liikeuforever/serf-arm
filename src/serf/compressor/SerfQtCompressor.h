#ifndef SERF_QT_COMPRESSOR_H
#define SERF_QT_COMPRESSOR_H

#include <cstdint>
#include <cmath>
#include <memory>

#include "serf/utils/OutputBitStream.h"
#include "serf/utils/Array.h"
#include "serf/utils/Double.h"
#include "serf/utils/EliasDeltaCodec.h"
#include "serf/utils/ZigZagCodec.h"

/*
 * +------------+-----------------+---------------+
 * |16bits - len|64bits - max_diff|Encoded Content|
 * +------------+-----------------+---------------+
 */

class SerfQtCompressor {
public:
    SerfQtCompressor(int block_size, double max_diff): max_diff_(max_diff * 0.999) {
        output_bit_stream_ = std::make_unique<OutputBitStream>(2 * block_size * 8);
        compressed_size_in_bits_ += output_bit_stream_->writeInt(block_size, 16);
        compressed_size_in_bits_ += output_bit_stream_->writeLong(Double::doubleToLongBits(max_diff_), 64);
    }

    void AddValue(double v);

    Array<uint8_t> GetBytes();

    void Close();

    long get_compressed_size_in_bits() const;

private:
    const double max_diff_;
    std::unique_ptr<OutputBitStream> output_bit_stream_;
    double pre_value_ = 2;
    long compressed_size_in_bits_ = 0;
    long stored_compressed_size_in_bits_;
};

#endif //SERF_QT_COMPRESSOR_H
