#ifndef SERF_ZIG_ZAG_CODEC_H
#define SERF_ZIG_ZAG_CODEC_H

#include <cstdint>

class ZigZagCodec {
public:
    static inline int64_t encode(int64_t value) {
        return (value << 1) ^ (value >> 63);
    }

    static inline int64_t decode(int64_t value) {
        return (value >> 1) ^ -(value & 1);
    }
};

#endif //SERF_ZIG_ZAG_CODEC_H
