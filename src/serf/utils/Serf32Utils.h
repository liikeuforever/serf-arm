#ifndef SERF_32_UTILS_H
#define SERF_32_UTILS_H

#include <cstdint>

#include "serf/utils/float.h"

class Serf32Utils {
public:
    static uint32_t findAppInt(float min, float max, float v, uint32_t lastInt, float maxDiff);

private:
    static uint32_t findAppInt(float minFloat, float maxFloat, uint32_t sign, float original, uint32_t lastInt, float maxDiff);

    static constexpr uint32_t bw[32] = {
            1U, 2U, 4U, 8U, 16U, 32U, 64U, 128U,
            256U, 512U, 1024U, 2048U, 4096U, 8192U, 16384U, 32768U,
            65536U, 131072U, 262144U, 524288U, 1048576U, 2097152U, 4194304U, 8388608U,
            16777216U, 33554432U, 67108864U, 134217728U, 268435456U, 536870912U, 1073741824U,
            2147483648U
    };
};

#endif //SERF_32_UTILS_H