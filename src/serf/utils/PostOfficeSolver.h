#ifndef SERF_POST_OFFICE_SOLVER_H
#define SERF_POST_OFFICE_SOLVER_H

#include <algorithm>
#include <limits>
#include <utility>

#include "serf/utils/Array.h"
#include "serf/utils/OutputBitStream.h"

class PostOfficeResult {
private:
    Array<int> officePositions;
    int totalAppCost;

public:
    PostOfficeResult(const Array<int>& officePositions, int totalAppCost);

    Array<int> getOfficePositions();

    int getAppCost() const;
};

class PostOfficeSolver {
public:
    static Array<int> initRoundAndRepresentation(Array<int> distribution, Array<int> representation, Array<int> round);

    static int writePositions(Array<int> &positions, OutputBitStream *out);

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

    static Array<int> calTotalCountAndNonZerosCounts(Array<int> arr, Array<int> outPreNonZerosCount, Array<int> outPostNonZerosCount);

    static PostOfficeResult buildPostOffice(Array<int> arr, int num, int nonZerosCount, Array<int> preNonZerosCount, Array<int>postNonZerosCount);
};

#endif
