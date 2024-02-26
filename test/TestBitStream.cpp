#include <gtest/gtest.h>

#include "../src/serf/InputBitStream.h"
#include "../src/serf/OutputBitStream.h"

TEST(TestInputBitStream, BasicTest) {
    uint8_t test_data = 0b1011000;
    InputBitStream input_bit_stream = InputBitStream(&test_data, 1);
    uint32_t output_data = input_bit_stream.readInt(8);
    EXPECT_EQ(test_data, output_data);
}

TEST(TestOutputBitStream, BasicTest) {
    OutputBitStream output_bit_stream = OutputBitStream(1);
    output_bit_stream.writeBit(true);
    output_bit_stream.writeBit(false);
    output_bit_stream.flush();
    EXPECT_EQ(output_bit_stream.getBuffer()[0], 128);
}