#ifndef SERF_ELIAS_DELTA_CODEC_H
#define SERF_ELIAS_DELTA_CODEC_H

#include <cmath>

#include "output_bit_stream.h"
#include "input_bit_stream.h"
#include "double.h"

class EliasDeltaCodec {
 public:
    static int Encode(int64_t number, OutputBitStream *output_bit_stream_ptr);

    static int64_t Decode(InputBitStream *input_bit_stream_ptr);

 private:
    constexpr static double kLogTable[12] = {
            Double::kNan,
            0,
            0.6931471805599453,
            1.0986122886681098,
            1.3862943611198906,
            1.6094379124341003,
            1.791759469228055,
            1.9459101490553132,
            2.0794415416798357,
            2.1972245773362196,
            2.302585092994046,
            2.3978952727983707
    };
};

#endif  // SERF_ELIAS_DELTA_CODEC_H
