#ifndef SERF_POST_OFFICE_SOLVER_32_H
#define SERF_POST_OFFICE_SOLVER_32_H

#include <algorithm>
#include <limits>
#include <utility>

#include "serf/utils/Array.h"
#include "serf/utils/OutputBitStream.h"
#include "serf/utils/PostOfficeResult.h"

class PostOfficeSolver32 {
public:
    static Array<int> initRoundAndRepresentation(Array<int> &distribution, Array<int> &representation, Array<int> &round);

    static int writePositions(Array<int> &positions, OutputBitStream *out);

    constexpr static int positionLength2Bits[] = {
            0, 0, 1, 2, 2, 3, 3, 3, 3,
            4, 4, 4, 4, 4, 4, 4, 4,
            5, 5, 5, 5, 5, 5, 5, 5,
            5, 5, 5, 5, 5, 5, 5, 5,
    };

private:
    constexpr static int pow2z[] = {1, 2, 4, 8, 16};

    static Array<int> calTotalCountAndNonZerosCounts(Array<int> &arr, Array<int> &outPreNonZerosCount, Array<int> &outPostNonZerosCount);

    static PostOfficeResult buildPostOffice(Array<int> &arr, int num, int nonZerosCount, Array<int> &preNonZerosCount, Array<int> &postNonZerosCount);
};

#endif //SERF_POST_OFFICE_SOLVER_32_H