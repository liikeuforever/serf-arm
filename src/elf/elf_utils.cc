#include "elf_utils.h"

int ElfUtils::getFAlpha(int alpha) {
    assert(alpha >= 0);
    if (alpha >= len(f_)) {
        return (int) std::ceil(alpha * LOG_2_10_);
    } else {
        return f_[alpha];
    }
}

Array<int> ElfUtils::getAlphaAndBetaStar(double v, int lastBetaStar) {
    if (v < 0) v = -v;
    Array<int> alpha_and_beta_star = Array<int>(2);
    Array<int> spAnd10iNFlag = getSPAnd10iNFlag(v);
    int beta = getSignificantCount(v, spAnd10iNFlag[0], lastBetaStar);
    alpha_and_beta_star[0] = beta - spAnd10iNFlag[0] - 1;
    alpha_and_beta_star[1] = spAnd10iNFlag[1] == 1 ? 0 : beta;
    return alpha_and_beta_star;
}

double ElfUtils::roundUp(double v, int alpha) {
    double scale = get10iP(alpha);
    if (v < 0) {
        return std::floor(v * scale) / scale;
    } else {
        return std::ceil(v * scale) / scale;
    }
}

int ElfUtils::getSignificantCount(double v, int sp, int lastBetaStar) {
    int i;
    if (lastBetaStar != std::numeric_limits<int>::max() && lastBetaStar != 0) {
        i = std::max(lastBetaStar - sp - 1, 1);
    } else if (lastBetaStar == std::numeric_limits<int>::max()) {
        i = 17 - sp - 1;
    } else if (sp >= 0) {
        i = 1;
    } else {
        i = -sp;
    }

    double temp = v * get10iP(i);
    long temp_long = (long) temp;
    while (temp_long != temp) {
        i++;
        temp = v * get10iP(i);
        temp_long = (long) temp;
    }

    if (temp / get10iP(i) != v) {
        return 17;
    } else {
        while (i > 0 && temp_long % 10 == 0) {
            i--;
            temp_long = temp_long / 10;
        }
        return sp + i + 1;
    }
}

double ElfUtils::get10iP(int i) {
    assert(i >= 0);
    if (i >= len(map10iP_)) {
        return std::pow(10, i);
    } else {
        return map10iP_[i];
    }
}

double ElfUtils::get10iN(int i) {
    assert(i >= 0);
    if (i >= len(map10iN_)) {
        return std::pow(10, -i);
    } else {
        return map10iN_[i];
    }
}

int ElfUtils::getSP(double v) {
    if (v >= 1) {
        int i = 0;
        while (i < len(mapSPGreater1_) - 1) {
            if (v < mapSPGreater1_[i + 1]) {
                return i;
            }
            i++;
        }
    } else {
        int i = 1;
        while (v < len(mapSPLess1_)) {
            if (v >= mapSPLess1_[i]) {
                return -i;
            }
            i++;
        }
    }
    return (int) std::floor(std::log10(v));
}

Array<int> ElfUtils::getSPAnd10iNFlag(double v) {
    Array<int> spAnd10iNFlag = Array<int>(2);
    if (v >= 1) {
        int i = 0;
        while (i < len(mapSPGreater1_) - 1) {
            if (v < mapSPGreater1_[i + 1]) {
                spAnd10iNFlag[0] = i;
                return spAnd10iNFlag;
            }
            i++;
        }
    } else {
        int i = 1;
        while (i < len(mapSPLess1_)) {
            if (v >= mapSPLess1_[i]) {
                spAnd10iNFlag[0] = -i;
                spAnd10iNFlag[1] = v == mapSPLess1_[i] ? 1 : 0;
                return spAnd10iNFlag;
            }
            i++;
        }
    }
    double log10v = std::log10(v);
    spAnd10iNFlag[0] = (int) floor(log10v);
    spAnd10iNFlag[1] = log10v == (long) log10v ? 1 : 0;
    return spAnd10iNFlag;
}
