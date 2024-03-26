#ifndef SERF_FLOAT_H
#define SERF_FLOAT_H

#include <cstdint>
#include <limits>

class Float {
public:
    static constexpr float NaN = std::numeric_limits<float>::quiet_NaN();

    static inline uint32_t floatToIntBits(float value) {
        return *reinterpret_cast<uint32_t *>(&value);
    }

    static inline float intBitsToFloat(uint32_t bits) {
        return *reinterpret_cast<float *>(&bits);
    }
};

#endif //SERF_FLOAT_H
