#ifndef ELF_UTILS_H
#define ELF_UTILS_H


#include <cmath>
#include <cassert>
#include <limits>

#include "serf/utils/array.h"

namespace ElfUtils {
    static constexpr int f_[] = {0, 4, 7, 10, 14, 17, 20, 24, 27, 30, 34, 37, 40, 44, 47, 50, 54, 57, 60, 64, 67};

    static constexpr double map10iP_[] = {1.0, 1.0E1, 1.0E2, 1.0E3, 1.0E4, 1.0E5, 1.0E6, 1.0E7, 1.0E8, 1.0E9, 1.0E10,
                                          1.0E11, 1.0E12, 1.0E13, 1.0E14, 1.0E15, 1.0E16, 1.0E17, 1.0E18, 1.0E19,
                                          1.0E20};

    static constexpr double map10iN_[] = {1.0, 1.0E-1, 1.0E-2, 1.0E-3, 1.0E-4, 1.0E-5, 1.0E-6, 1.0E-7, 1.0E-8, 1.0E-9,
                                          1.0E-10,
                                          1.0E-11, 1.0E-12, 1.0E-13, 1.0E-14, 1.0E-15, 1.0E-16, 1.0E-17, 1.0E-18,
                                          1.0E-19,
                                          1.0E-20};

    static constexpr long mapSPGreater1_[] = {1, 10, 100, 1000, 10000, 1000000, 10000000, 100000000, 1000000000};

    static constexpr double mapSPLess1_[] = {1, 0.1, 0.01, 0.001, 0.0001, 0.00001, 0.000001, 0.0000001, 0.00000001,
                                             0.000000001, 0.0000000001};

    static constexpr double LOG_2_10_ = 3.321928095;

    template<class T>
    inline static int len(T &array) {
        return sizeof(array) / sizeof(array[0]);
    }

    int getFAlpha(int alpha);

    Array<int> getAlphaAndBetaStar(double v, int lastBetaStar);

    double roundUp(double v, int alpha);

    int getSignificantCount(double v, int sp, int lastBetaStar);

    double get10iP(int i);

    double get10iN(int i);

    int getSP(double v);

    Array<int> getSPAnd10iNFlag(double v);
};


#endif // ELF_UTILS_H
