#ifndef SERFNATIVE_DOUBLE_H
#define SERFNATIVE_DOUBLE_H

namespace Double {
    inline unsigned long doubleToLongBits(double value) {
        return *(reinterpret_cast<unsigned long *>(&value));
    }

    inline double longBitsToDouble(unsigned long bits) {
        return *(reinterpret_cast<double *>(&bits));
    }
}

#endif //SERFNATIVE_DOUBLE_H
