#ifndef SERF_DOUBLE_H
#define SERF_DOUBLE_H

namespace Double {
    inline unsigned long doubleToULongBits(double value) {
        return *reinterpret_cast<unsigned long *>(&value);
    }

    inline double UlongBitsToDouble(unsigned long bits) {
        return *reinterpret_cast<double *>(&bits);
    }

    inline long doubleToLongBits(double value) {
        return *reinterpret_cast<long *>(&value);
    }

    inline double longBitsToDouble(long bits) {
        return *reinterpret_cast<double *>(&bits);
    }
}

#endif
