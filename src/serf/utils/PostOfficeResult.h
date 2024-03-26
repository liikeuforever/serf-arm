#ifndef SERF_POST_OFFICE_RESULT_H
#define SERF_POST_OFFICE_RESULT_H

#include "serf/utils/Array.h"

class PostOfficeResult {
private:
    Array<int> officePositions;
    int totalAppCost;

public:
    PostOfficeResult(const Array<int>& officePositions, int totalAppCost): officePositions(officePositions), totalAppCost(totalAppCost) {};

    Array<int> getOfficePositions() {
        return officePositions;
    }

    int getAppCost() const {
        return totalAppCost;
    }
};

#endif //SERF_POST_OFFICE_RESULT_H
