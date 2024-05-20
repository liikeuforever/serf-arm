#include <gtest/gtest.h>

#include "serf/utils/input_bit_stream.h"
#include "serf/utils/OutputBitStream.h"

TEST(TestInputBitStream, BasicTest) {
    uint8_t test_data = 0b1011000;
    InputBitStream input_bit_stream = InputBitStream(&test_data, 1);
    uint32_t output_data = input_bit_stream.ReadInt(8);
    EXPECT_EQ(test_data, output_data);
}

TEST(TestOutputBitStream, BasicTest) {
    OutputBitStream output_bit_stream = OutputBitStream(1);
    output_bit_stream.writeBit(true);
    output_bit_stream.writeBit(false);
    output_bit_stream.flush();
    EXPECT_EQ(output_bit_stream.getBuffer()[0], 128);
}

TEST(TestOutputBitStream, LongTest) {
    OutputBitStream output_bit_stream = OutputBitStream(10);
    output_bit_stream.writeInt(0b00100011, 8);
    output_bit_stream.writeInt(0b00011010, 8);
    output_bit_stream.writeInt(0b01011010, 8);
    output_bit_stream.writeInt(0b10011010, 8);
    output_bit_stream.writeInt(0b00000010, 8);
    output_bit_stream.writeInt(0b00010000, 8);
    output_bit_stream.writeInt(0b01000010, 8);
    output_bit_stream.writeInt(0b10000010, 8);
    output_bit_stream.writeInt(0b00100010, 8);
    output_bit_stream.flush();

    uint8_t *buffer = output_bit_stream.getBuffer();

    EXPECT_EQ(buffer[0], 0b00100011);
    EXPECT_EQ(buffer[1], 0b00011010);
    EXPECT_EQ(buffer[2], 0b01011010);
    EXPECT_EQ(buffer[3], 0b10011010);
    EXPECT_EQ(buffer[4], 0b00000010);
    EXPECT_EQ(buffer[5], 0b00010000);
    EXPECT_EQ(buffer[6], 0b01000010);
    EXPECT_EQ(buffer[7], 0b10000010);
    EXPECT_EQ(buffer[8], 0b00100010);
}