#include <gtest/gtest.h>

#include "serf/utils/ZigZagCodec.h"

TEST(TestZigZag, RangeTest) {
    for (int64_t i = -10000000; i <= 10000000; ++i) {
        EXPECT_EQ(i, ZigZagCodec::decode(ZigZagCodec::encode(i)));
    }
}