#include <gtest/gtest.h>

#include "serf/utils/zig_zag_codec.h"

TEST(TestZigZag, RangeTest) {
    for (int64_t i = -10000000; i <= 10000000; ++i) {
        EXPECT_EQ(i, ZigZagCodec::Decode(ZigZagCodec::Encode(i)));
    }
}