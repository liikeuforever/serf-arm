#ifndef SERFNATIVE_POSTOFFICESOLVER_H
#define SERFNATIVE_POSTOFFICESOLVER_H


class PostOfficeSolver {
    constexpr static int pow2z[] = {1, 2, 4, 8, 16, 32};
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

    int initRoundAndRepresentation (int *distribution, int distributionLen, int *representation, int representationLen, int *round, int roundLen, int *result) {
        int preNonZerosCount[distributionLen];   // 当前及前面的非零个数（包括当前）
        int postNonZerosCount[distributionLen];  // 当前后面的非零个数（不包括当前）
        int[] totalCountAndNonZerosCount = calTotalCountAndNonZerosCounts(distribution, preNonZerosCount, postNonZerosCount);

        int maxZ = Math.min(positionLength2Bits[totalCountAndNonZerosCount[1]], 5); // 最多用5个bit来表示

        int totalCost = Integer.MAX_VALUE;
        int[] positions = {};

        for (int z = 0; z <= maxZ; ++z) {
            int presentCost = totalCountAndNonZerosCount[0] * z;
            if (presentCost >= totalCost) {
                break;
            }
            int num = PostOfficeSolver.pow2z[z];     // 邮局的总数量
            PostOfficeResult por = PostOfficeSolver.buildPostOffice(
                    distribution, num, totalCountAndNonZerosCount[1], preNonZerosCount, postNonZerosCount);
            int tempTotalCost = por.getAppCost() + presentCost;
            if (tempTotalCost < totalCost) {
                totalCost = tempTotalCost;
                positions = por.getOfficePositions();
            }
        }

        representation[0] = 0;
        round[0] = 0;
        int i = 1;
        for (int j = 1; j < distribution.length; ++j) {
            if (i < positions.length && j == positions[i]) {
                representation[j] = representation[j - 1] + 1;
                round[j] = j;
                ++i;
            } else {
                representation[j] = representation[j - 1];
                round[j] = round[j - 1];
            }
        }

        return positions;
    }

    void calTotalCountAndNonZerosCounts(int *arr, int arrLen, int *outPreNonZerosCount, int outPreNonZerosCountLen, int *outPostNonZerosCount, int outPostNonZerosCountLen, int *result, int *totalCount, int *nonZerosCount) {
        int nonZerosCount = arrLen;
        int totalCount = arr[0];
        outPreNonZerosCount[0] = 1;            // 第一个视为非零
        for (int i = 1; i < arrLen; ++i) {
            totalCount += arr[i];
            if (arr[i] == 0) {
                --nonZerosCount;
                outPreNonZerosCount[i] = outPreNonZerosCount[i - 1];
            } else {
                outPreNonZerosCount[i] = outPreNonZerosCount[i - 1] + 1;
            }
        }
        for (int i = 0; i < arrLen; ++i) {
            outPostNonZerosCount[i] = nonZerosCount - outPreNonZerosCount[i];
        }
    }
};


#endif //SERFNATIVE_POSTOFFICESOLVER_H
