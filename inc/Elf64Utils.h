#ifndef SERFNATIVE_ELF64UTILS_H
#define SERFNATIVE_ELF64UTILS_H

#include <cmath>
#include <string>

class Elf64Utils {
private:
    constexpr static int f[] = {0, 4, 7, 10, 14, 17, 20, 24, 27, 30, 34, 37, 40, 44, 47, 50, 54, 57, 60, 64, 67};
    constexpr static double map10iN[] = {1.0f, 1.0E-1, 1.0E-2, 1.0E-3, 1.0E-4, 1.0E-5, 1.0E-6, 1.0E-7,
                                         1.0E-8, 1.0E-9, 1.0E-10, 1.0E-11, 1.0E-12, 1.0E-13, 1.0E-14,
                                         1.0E-15, 1.0E-16, 1.0E-17, 1.0E-18, 1.0E-19, 1.0E-20};

public:
    constexpr static double LOG_2_10 = 3.32192809488736263f;

    static int getFAlpha(int alpha);

    static double get10iN(int i);
};

#endif //SERFNATIVE_ELF64UTILS_H