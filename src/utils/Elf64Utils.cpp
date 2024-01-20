#include <stdexcept>
#include "Elf64Utils.h"

int Elf64Utils::getFAlpha(int alpha) {
    if (alpha < 0) {
        throw std::invalid_argument("Alpha should be greater than 0");
    }
    if (alpha >= sizeof(f) / sizeof(int)) {
        return (int) ceil(alpha * LOG_2_10);
    } else {
        return f[alpha];
    }
}

double Elf64Utils::get10iP(int i) {
    if (i < 0) {
        throw std::invalid_argument("The argument should be greater than 0");
    }
    if (i >= sizeof(map10iP) / sizeof(double)) {
        return std::stod("10E" + std::to_string(i));
    } else {
        return map10iP[i];
    }
}

double Elf64Utils::get10iN(int i) {
    if (i < 0) {
        throw std::invalid_argument("The argument should be greater than 0");
    }
    if (i >= sizeof(map10iN) / sizeof(double)) {
        return std::stod("10E-" + std::to_string(i));
    } else {
        return map10iN[i];
    }
}