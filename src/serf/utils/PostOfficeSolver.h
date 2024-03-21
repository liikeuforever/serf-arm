#ifndef SERF_POST_OFFICE_SOLVER_H
#define SERF_POST_OFFICE_SOLVER_H

#include <algorithm>
#include <limits>
#include <utility>
#include <vector>

#include "serf/utils/OutputBitStream.h"

class PostOfficeResult {
private:
    std::vector<int> officePositions;
    int totalAppCost;

public:
    PostOfficeResult(std::vector<int> officePositions, int totalAppCost);

    std::vector<int> getOfficePositions();

    int getAppCost() const;
};

class PostOfficeSolver {
public:
    static std::vector<int> initRoundAndRepresentation(std::vector<int> distribution, std::vector<int> representation,
                                                       std::vector<int> round);

    static int writePositions(std::vector<int> &positions, OutputBitStream *out);

    constexpr static int positionLength2Bits[] = {
            0, 0, 1, 2, 2, 3, 3, 3, 3,
            4, 4, 4, 4, 4, 4, 4, 4,
            5, 5, 5, 5, 5, 5, 5, 5,
            5, 5, 5, 5, 5, 5, 5, 5,
            6, 6, 6, 6, 6, 6, 6, 6,
            6, 6, 6, 6, 6, 6, 6, 6,
            6, 6, 6, 6, 6, 6, 6, 6,
            6, 6, 6, 6, 6, 6, 6, 6
    };

private:
    constexpr static int pow2z[] = {1, 2, 4, 8, 16, 32};

    static std::vector<int>
    calTotalCountAndNonZerosCounts(const std::vector<int> &arr, std::vector<int> &outPreNonZerosCount,
                                   std::vector<int> &outPostNonZerosCount);

    static PostOfficeResult
    buildPostOffice(std::vector<int> &arr, int num, int nonZerosCount, std::vector<int> &preNonZerosCount,
                    std::vector<int> &postNonZerosCount);
};

#endif
