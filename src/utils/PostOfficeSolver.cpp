#include "PostOfficeSolver.h"

#include <utility>
#include "NewOutputBitStream.h"

PostOfficeResult::PostOfficeResult(std::vector<int> officePositions, int totalAppCost) {
    this->officePositions = std::move(officePositions);
    this->totalAppCost = totalAppCost;
}

std::vector<int> PostOfficeResult::getOfficePositions() const {
    return officePositions;
}

int PostOfficeResult::getAppCost() const {
    return totalAppCost;
}


std::vector<int>
PostOfficeSolver::initRoundAndRepresentation(std::vector<int> &distribution, std::vector<int> &representation,
                                             std::vector<int> &round) {
    std::vector<int> preNonZerosCount(distribution.size());   // 当前及前面的非零个数（包括当前）
    std::vector<int> postNonZerosCount(distribution.size());  // 当前后面的非零个数（不包括当前）
    std::vector<int> totalCountAndNonZerosCount = calTotalCountAndNonZerosCounts(distribution, preNonZerosCount,
                                                                                 postNonZerosCount);

    int maxZ = std::min(positionLength2Bits[totalCountAndNonZerosCount[1]], 5); // 最多用5个bit来表示

    int totalCost = std::numeric_limits<int>::max();
    std::vector<int> positions;

    for (int z = 0; z <= maxZ; ++z) {
        int presentCost = totalCountAndNonZerosCount[0] * z;
        if (presentCost >= totalCost) {
            break;
        }
        int num = PostOfficeSolver::pow2z[z];     // 邮局的总数量
        PostOfficeResult por = PostOfficeSolver::buildPostOffice(
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
    for (int j = 1; j < distribution.size(); ++j) {
        if (i < positions.size() && j == positions[i]) {
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

std::vector<int>
PostOfficeSolver::calTotalCountAndNonZerosCounts(const std::vector<int> &arr, std::vector<int> &outPreNonZerosCount,
                                                 std::vector<int> &outPostNonZerosCount) {
    int nonZerosCount = static_cast<int>(arr.size());
    int totalCount = arr[0];
    outPreNonZerosCount[0] = 1;            // 第一个视为非零
    for (int i = 1; i < arr.size(); ++i) {
        totalCount += arr[i];
        if (arr[i] == 0) {
            --nonZerosCount;
            outPreNonZerosCount[i] = outPreNonZerosCount[i - 1];
        } else {
            outPreNonZerosCount[i] = outPreNonZerosCount[i - 1] + 1;
        }
    }
    for (int i = 0; i < arr.size(); ++i) {
        outPostNonZerosCount[i] = nonZerosCount - outPreNonZerosCount[i];
    }
    return std::vector<int>{totalCount, nonZerosCount};
}

PostOfficeResult
PostOfficeSolver::buildPostOffice(std::vector<int> &arr, int num, int nonZerosCount, std::vector<int> &preNonZerosCount,
                                  std::vector<int> &postNonZerosCount) {
    int originalNum = num;
    num = std::min(num, nonZerosCount);

    int dp[arr.size()][num];      // 状态矩阵。d[i][j]表示，只考虑前i个居民点，且第i个位置是第j个邮局的总距离，i >= j，
    // 下标从0开始。注意，并非是所有居民点的总距离，因为没有考虑第j个邮局之后的居民点的距离
    int pre[arr.size()][num];     // 对应于dp[i][j]，表示让dp[i][j]最小时，第j-1个邮局所在的位置信息

    dp[0][0] = 0;                       // 第0个位置是第0个邮局，此时状态为0
    pre[0][0] = -1;                     // 让dp[0][0]最小时，第-1个邮局所在的位置信息为-1

    for (int i = 1; i < arr.size(); ++i) {
        if (arr[i] == 0) {
            continue;
        }
        for (int j = std::max(1, static_cast<int>((num + i - arr.size()))); j <= i && j < num; ++j) {
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
                for (int k = j - 1; k <= i - 1; ++k) {
                    if (arr[k] == 0 && k > 0 || preNonZerosCount[k] < j || postNonZerosCount[k] < num - j) {
                        continue;
                    }
                    int sum = dp[k][j - 1];
                    for (int p = k + 1; p <= i - 1; ++p) {
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
    for (int i = num - 1; i < arr.size(); ++i) {
        if (num - 1 == 0 && i > 0) {
            break;
        }
        if (arr[i] == 0 && i > 0 || preNonZerosCount[i] < num) {
            continue;
        }
        int sum = dp[i][num - 1];
        for (int j = i + 1; j < arr.size(); ++j) {
            sum += arr[j] * (j - i);
        }
        if (tempTotalAppCost > sum) {
            tempTotalAppCost = sum;
            tempBestLast = i;
        }
    }

    std::vector<int> officePositions(num);
    int i = 1;

    while (tempBestLast != -1) {
        officePositions[num - i] = tempBestLast;
        tempBestLast = pre[tempBestLast][num - i];
        ++i;
    }

    if (originalNum > nonZerosCount) {
        std::vector<int> modifyingOfficePositions(originalNum);
        int j = 0, k = 0;
        while (j < originalNum && k < num) {
            if (j - k < originalNum - num && j < officePositions[k]) {
                modifyingOfficePositions[j] = j;
                ++j;
            } else {
                modifyingOfficePositions[j] = officePositions[k];
                ++j;
                ++k;
            }
        }
        officePositions = modifyingOfficePositions;
    }

    return {officePositions, tempTotalAppCost};
}

int PostOfficeSolver::writePositions(std::vector<int> &positions, NewOutputBitStream &out) {
    int thisSize = out.writeInt(static_cast<int>(positions.size()), 5);
    for (int p: positions) {
        thisSize += out.writeInt(p, 6);
    }
    return thisSize;
}

