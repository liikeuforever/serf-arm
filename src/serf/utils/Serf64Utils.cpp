#include "Serf64Utils.h"

uint64_t Serf64Utils::findAppLong(double min, double max, double v, uint64_t lastLong, double maxDiff, double adjustD) {
    if (min >= 0) {
        // both positive
        return findAppLong(min, max, 0, v, lastLong, maxDiff, adjustD);
    } else if (max <= 0) {
        // both negative
        return findAppLong(-max, -min, 0x8000000000000000L, v, lastLong, maxDiff, adjustD);
    } else if (lastLong >> 63 == 0) {
        // consider positive part only, to make more leading zeros
        return findAppLong(0, max, 0, v, lastLong, maxDiff, adjustD);
    } else {
        // consider negative part only, to make more leading zeros
        return findAppLong(0, -min, 0x8000000000000000L, v, lastLong, maxDiff, adjustD);
    }
}

uint64_t Serf64Utils::findAppLong(double minDouble, double maxDouble, uint64_t sign, double original, uint64_t lastLong,
                                  double maxDiff, double adjustD) {
    uint64_t min = Double::doubleToLongBits(minDouble) & 0x7fffffffffffffffULL; // may be negative zero
    uint64_t max = Double::doubleToLongBits(maxDouble);
    int leadingZeros = __builtin_clzll(min ^ max);
    uint64_t frontMask = 0xffffffffffffffffULL << (64 - leadingZeros);
    int shift = 64 - leadingZeros;
    uint64_t resultLong;
    double diff;
    uint64_t append;
    while (shift >= 0) {
        uint64_t front = frontMask & min;
        uint64_t rear = (~frontMask) & lastLong;

        append = rear | front;
        if (append >= min && append <= max) {
            resultLong = append ^ sign;
            diff = Double::longBitsToDouble(resultLong) - adjustD - original;
            if (diff >= -maxDiff && diff <= maxDiff) {
                return resultLong;
            }
        }

        append = (append + bw[shift]) & 0x7fffffffffffffffL; // may be overflow
        if (append <= max) {    // append must be greater than min
            resultLong = append ^ sign;
            diff = Double::longBitsToDouble(resultLong) - adjustD - original;
            if (diff >= -maxDiff && diff <= maxDiff) {
                return resultLong;
            }
        }

        frontMask = frontMask >> 1;

        --shift;
    }

    return Double::doubleToLongBits(
            original + adjustD);    // we do not find a satisfied value, so we return the original value
}
