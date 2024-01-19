#include "Elf64Utils.h"
#include <stdexcept>

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

int Elf64Utils::getSignificantCount(double v, int sp) {
    int i;
    if (sp >= 0) {
        i = 1;
    } else {
        i = -sp;
    }
    double temp = v * get10iP(i);
    while ((long) temp != temp) {
        i++;
        temp = v * get10iP(i);
    }
    // There are some bugs for those with high significand, i.e., 0.23911204406033099
    // So we should further check
    if (temp / get10iP(i) != v) {
        return 17;
    } else {
        return sp + i + 1;
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