#ifndef SERF_DOUBLE_H
#define SERF_DOUBLE_H

class Double {
public:
    static inline unsigned long doubleToULongBits(double value) {
        return *reinterpret_cast<unsigned long *>(&value);
    }

    static inline double UlongBitsToDouble(unsigned long bits) {
        return *reinterpret_cast<double *>(&bits);
    }

    static inline long doubleToLongBits(double value) {
        return *reinterpret_cast<long *>(&value);
    }

    static inline double longBitsToDouble(long bits) {
        return *reinterpret_cast<double *>(&bits);
    }
};

#endif
