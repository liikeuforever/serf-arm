#ifndef SPARSE_RESULT_H
#define SPARSE_RESULT_H


#include <cstdint>

#include "serf/utils/Array.h"

class SparseResult {
public:
    bool flag_;
    uint8_t frequent_value_;
    Array<uint8_t> bitmap_ = Array<uint8_t>(0);
    Array<bool> is_frequent_value_ = Array<bool>(0);
    Array<uint8_t> outliers_ = Array<uint8_t>(0);
    int outliers_count_ = 0;

    explicit SparseResult(int batch_size);

    void set_frequent_value(int frequent_value);

    Array<uint8_t> get_outliers();
};


#endif // SPARSE_RESULT_H
