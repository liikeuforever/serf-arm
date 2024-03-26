#include "Serf32Utils.h"

uint32_t Serf32Utils::findAppInt(float min, float max, float v, uint32_t lastInt, float maxDiff) {
    if (min >= 0) {
        // both positive
        return findAppInt(min, max, 0, v, lastInt, maxDiff);
    } else if (max <= 0) {
        // both negative
        return findAppInt(-max, -min, 0x80000000, v, lastInt, maxDiff);
    } else if (lastInt >> 31 == 0) {
        // consider positive part only, to make more leading zeros
        return findAppInt(0, max, 0, v, lastInt, maxDiff);
    } else {
        // consider negative part only, to make more leading zeros
        return findAppInt(0, -min, 0x80000000, v, lastInt, maxDiff);
    }
}

uint32_t Serf32Utils::findAppInt(float minFloat, float maxFloat, uint32_t sign, float original, uint32_t lastInt, float maxDiff) {
    uint32_t min = Float::floatToIntBits(minFloat) & 0x7fffffff; // may be negative zero
    uint32_t max = Float::floatToIntBits(maxFloat);
    int leadingZeros = __builtin_clz(min ^ max);
    uint32_t frontMask = 0xffffffff << (32 - leadingZeros);
    int shift = 32 - leadingZeros;
    uint32_t resultInt;
    float diff;
    uint32_t append;
    while (shift >= 0) {
        uint32_t front = frontMask & min;
        uint32_t rear = (~frontMask) & lastInt;

        append = rear | front;
        if (append >= min && append <= max) {
            resultInt = append ^ sign;
            diff = Float::intBitsToFloat(resultInt) - original;
            if (diff >= -maxDiff && diff <= maxDiff) {
                return resultInt;
            }
        }

        append = (append + bw[shift]) & 0x7fffffff; // may be overflow
        if (append <= max) {    // append must be greater than min
            resultInt = append ^ sign;
            diff = Float::intBitsToFloat(resultInt) - original;
            if (diff >= -maxDiff && diff <= maxDiff) {
                return resultInt;
            }
        }

        frontMask = frontMask >> 1;

        --shift;
    }

    return Float::floatToIntBits(original);    // we do not find a satisfied value, so we return the original value
}
