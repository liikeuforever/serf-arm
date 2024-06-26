#include "utils/serf_utils_64.h"

uint64_t
SerfUtils64::FindAppLong(double min, double max, double v, uint64_t last_long,
                         double max_diff, double adjust_digit) {
  if (min >= 0) {
    // both positive
    return FindAppLong(min, max, 0, v, last_long, max_diff, adjust_digit);
  } else if (max <= 0) {
    // both negative
    return FindAppLong(-max, -min, 0x8000000000000000ULL, v, last_long,
                       max_diff, adjust_digit);
  } else if (last_long >> 63 == 0) {
    // consider positive part only, to make more leading zeros
    return FindAppLong(0, max, 0, v, last_long, max_diff, adjust_digit);
  } else {
    // consider negative part only, to make more leading zeros
    return FindAppLong(0, -min, 0x8000000000000000ULL, v, last_long,
                       max_diff, adjust_digit);
  }
}

uint64_t
SerfUtils64::FindAppLong(double min_double, double max_double, uint64_t sign,
                         double original, uint64_t last_long,
                         double max_diff, double adjust_digit) {
  // may be negative zero
  uint64_t min = Double::DoubleToLongBits(min_double) & 0x7fffffffffffffffULL;
  uint64_t max = Double::DoubleToLongBits(max_double);
  int leading_zeros = __builtin_clzll(min ^ max);
  int64_t front_mask = 0xffffffffffffffff << (64 - leading_zeros);
  int shift = 64 - leading_zeros;
  uint64_t result_long;
  double diff;
  uint64_t append;
  while (shift >= 0) {
    uint64_t front = front_mask & min;
    uint64_t rear = (~front_mask) & last_long;

    append = rear | front;
    if (append >= min && append <= max) {
      result_long = append ^ sign;
      diff = Double::LongBitsToDouble(result_long) - adjust_digit -
          original;
      if (diff >= -max_diff && diff <= max_diff) {
        return result_long;
      }
    }

    // may be overflow
    append = (append + kBitWeight[shift]) & 0x7fffffffffffffffL;
    if (append <= max) {
      // append must be greater than min
      result_long = append ^ sign;
      diff = Double::LongBitsToDouble(result_long) - adjust_digit -
          original;
      if (diff >= -max_diff && diff <= max_diff) {
        return result_long;
      }
    }

    front_mask = front_mask >> 1;

    --shift;
  }

  // we do not find a satisfied value, so we return the original value
  return Double::DoubleToLongBits(original + adjust_digit);
}
