#ifndef SERFNATIVE_ELF64UTILS_H
#define SERFNATIVE_ELF64UTILS_H

#include <cmath>

class Elf64Utils {
private:
    constexpr static int f[] = {0, 4, 7, 10, 14, 17, 20, 24, 27, 30, 34, 37, 40, 44, 47, 50, 54, 57, 60, 64, 67};
    constexpr static double map10iP[] = {1.0, 1.0E1, 1.0E2, 1.0E3, 1.0E4, 1.0E5, 1.0E6, 1.0E7,
                                         1.0E8, 1.0E9, 1.0E10, 1.0E11, 1.0E12, 1.0E13, 1.0E14,
                                         1.0E15, 1.0E16, 1.0E17, 1.0E18, 1.0E19, 1.0E20};
    constexpr static double map10iN[] = {1.0, 1.0E-1, 1.0E-2, 1.0E-3, 1.0E-4, 1.0E-5, 1.0E-6, 1.0E-7,
                                         1.0E-8, 1.0E-9, 1.0E-10, 1.0E-11, 1.0E-12, 1.0E-13, 1.0E-14,
                                         1.0E-15, 1.0E-16, 1.0E-17, 1.0E-18, 1.0E-19, 1.0E-20};
    constexpr static long mapSPGreater1[] = {1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000};
    constexpr static double mapSPLess1[] = {1, 0.1, 0.01, 0.001, 0.0001, 0.00001, 0.000001, 0.0000001, 0.00000001,
                                          0.000000001, 0.0000000001};

public:
    static const long END_SIGN = 0x7ff8000000000000L;
    static double LOG_2_10;
    static int getFAlpha(int alpha);
    static double get10iP(int i);
    static double get10iN(int i);
};

#endif //SERFNATIVE_ELF64UTILS_H