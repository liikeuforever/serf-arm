#ifndef SERF_ELIAS_DELTA_CODEC_H
#define SERF_ELIAS_DELTA_CODEC_H

#include <cmath>

#include "serf/utils/OutputBitStream.h"
#include "serf/utils/InputBitStream.h"
#include "serf/utils/Double.h"

class EliasDeltaCodec {
private:
    constexpr static double logTable[12] = {
            Double::NaN,
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

public:
    static int encode(int64_t number, OutputBitStream &outputBitStream);

    static int64_t decode(InputBitStream &inputBitStream);
};

#endif //SERF_ELIAS_DELTA_CODEC_H
