#include "PostOfficeSolver32.h"

Array<int> PostOfficeSolver32::initRoundAndRepresentation(Array<int> &distribution, Array<int> &representation,
                                                          Array<int> &round) {
    Array<int> preNonZerosCount(distribution.length());   // 当前及前面的非零个数（包括当前）
    Array<int> postNonZerosCount(distribution.length());  // 当前后面的非零个数（不包括当前）
    Array<int> totalCountAndNonZerosCount = calTotalCountAndNonZerosCounts(distribution, preNonZerosCount, postNonZerosCount);

    int maxZ = std::min(positionLength2Bits[totalCountAndNonZerosCount[1]], 4); // 最多用4个bit来表示

    int totalCost = std::numeric_limits<int>::max();
    Array<int> positions = {};

    for (int z = 0; z <= maxZ; z++) {
        int presentCost = totalCountAndNonZerosCount[0] * z;
        if (presentCost >= totalCost) {
            break;
        }
        int num = PostOfficeSolver32::pow2z[z];     // 邮局的总数量
        PostOfficeResult por = PostOfficeSolver32::buildPostOffice(distribution, num, totalCountAndNonZerosCount[1], preNonZerosCount, postNonZerosCount);
        int tempTotalCost = por.total_app_cost() + presentCost;
        if (tempTotalCost < totalCost) {
            totalCost = tempTotalCost;
            positions = por.office_positions();
        }
    }

    representation[0] = 0;
    round[0] = 0;
    int i = 1;
    for (int j = 1; j < distribution.length(); j++) {
        if (i < positions.length() && j == positions[i]) {
            representation[j] = representation[j - 1] + 1;
            round[j] = j;
            i++;
        } else {
            representation[j] = representation[j - 1];
            round[j] = round[j - 1];
        }
    }

    return positions;
}

int PostOfficeSolver32::writePositions(Array<int> &positions, OutputBitStream *out) {
    int thisSize = out->WriteInt(positions.length(), 4);
    for (int i = 0; i < positions.length(); i++) {
        thisSize += out->WriteInt(positions[i], 5);
    }
    return thisSize;
}

Array<int> PostOfficeSolver32::calTotalCountAndNonZerosCounts(Array<int> &arr, Array<int> &outPreNonZerosCount,
                                                              Array<int> &outPostNonZerosCount) {
    int nonZerosCount = arr.length();
    int totalCount = arr[0];
    outPreNonZerosCount[0] = 1;            // 第一个视为非零
    for (int i = 1; i < arr.length(); i++) {
        totalCount += arr[i];
        if (arr[i] == 0) {
            nonZerosCount--;
            outPreNonZerosCount[i] = outPreNonZerosCount[i - 1];
        } else {
            outPreNonZerosCount[i] = outPreNonZerosCount[i - 1] + 1;
        }
    }
    for (int i = 0; i < arr.length(); i++) {
        outPostNonZerosCount[i] = nonZerosCount - outPreNonZerosCount[i];
    }
    return Array<int> {totalCount, nonZerosCount};
}

PostOfficeResult
PostOfficeSolver32::buildPostOffice(Array<int> &arr, int num, int nonZerosCount, Array<int> &preNonZerosCount,
                                    Array<int> &postNonZerosCount) {
    int originalNum = num;
    num = std::min(num, nonZerosCount);

    int dp[arr.length()][num];      // 状态矩阵。d[i][j]表示，只考虑前i个居民点，且第i个位置是第j个邮局的总距离，i >= j，
    // 下标从0开始。注意，并非是所有居民点的总距离，因为没有考虑第j个邮局之后的居民点的距离
    int pre[arr.length()][num];     // 对应于dp[i][j]，表示让dp[i][j]最小时，第j-1个邮局所在的位置信息

    dp[0][0] = 0;                       // 第0个位置是第0个邮局，此时状态为0
    pre[0][0] = -1;                     // 让dp[0][0]最小时，第-1个邮局所在的位置信息为-1

    for (int i = 1; i < arr.length(); i++) {
        if (arr[i] == 0) {
            continue;
        }
        for (int j = std::max(1, num + i - arr.length()); j <= i && j < num; j++) {
            // arr.length - i < num - j，表示i后面的居民数（arr.length - i）不足以构建剩下的num - j个邮局
            if (i > 1 && j == 1) {
                dp[i][j] = 0;
                for (int k = 1; k < i; k++) {
                    dp[i][j] += arr[k] * k;
                }
                pre[i][j] = 0;
            } else {
                if (preNonZerosCount[i] < j + 1 || postNonZerosCount[i] < num - 1 - j) {
                    continue;
                }
                int appCost = std::numeric_limits<int>::max();
                int preK = 0;
                for (int k = j - 1; k <= i - 1; k++) {
                    if (arr[k] == 0 && k > 0) {
                        continue;
                    }
                    if (preNonZerosCount[k] < j || postNonZerosCount[k] < num - j) {
                        continue;
                    }
                    int sum = dp[k][j - 1];
                    for (int p = k + 1; p <= i - 1; p++) {
                        sum += arr[p] * (p - k);
                    }
                    if (appCost > sum) {
                        appCost = sum;
                        preK = k;
                        if (sum == 0) { // 找到其中一个0，提前终止
                            break;
                        }
                    }
                }
                if (appCost != std::numeric_limits<int>::max()) {
                    dp[i][j] = appCost;
                    pre[i][j] = preK;
                }
            }
        }
    }
    int tempTotalAppCost = std::numeric_limits<int>::max();
    int tempBestLast = std::numeric_limits<int>::max();
    for (int i = num - 1; i < arr.length(); i++) {
        if (num - 1 == 0 && i > 0) {
            break;
        }
        if (arr[i] == 0 && i > 0) {
            continue;
        }
        if (preNonZerosCount[i] < num) {
            continue;
        }
        int sum = dp[i][num - 1];
        for (int j = i + 1; j < arr.length(); j++) {
            sum += arr[j] * (j - i);
        }
        if (tempTotalAppCost > sum) {
            tempTotalAppCost = sum;
            tempBestLast = i;
        }
    }

    Array<int> officePositions(num);
    int i = 1;

    while (tempBestLast != -1) {
        officePositions[num - i] = tempBestLast;
        tempBestLast = pre[tempBestLast][num - i];
        i++;
    }

    if (originalNum > nonZerosCount) {
        Array<int> modifyingOfficePositions(originalNum);
        int j = 0, k = 0;
        while (j < originalNum && k < num) {
            if (j - k < originalNum - num && j < officePositions[k]) {
                modifyingOfficePositions[j] = j;
                j++;
            } else {
                modifyingOfficePositions[j] = officePositions[k];
                j++;
                k++;
            }
        }
        officePositions = modifyingOfficePositions;
    }

    return {officePositions, tempTotalAppCost};
}
