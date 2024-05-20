#include <gtest/gtest.h>
#include <bitset>

#include "serf/utils/elias_delta_codec.h"

TEST(TestEliasDeltaCodec, RangeTest) {
    OutputBitStream output_bit_stream(10);
    InputBitStream input_bit_stream = InputBitStream();

    for (int64_t i = 1; i <= 10000000; ++i) {
        int bitCount = EliasDeltaCodec::Encode(i, output_bit_stream);
        output_bit_stream.flush();

        uint8_t *data_ptr = output_bit_stream.getBuffer();
        Array<uint8_t> encoded(std::ceil(static_cast<double>(bitCount) / 8.0));
        for (int j = 0; j < encoded.length; ++j) {
            encoded[j] = data_ptr[j];
        }

        input_bit_stream.SetBuffer(encoded);
        output_bit_stream.refresh();

        EXPECT_EQ(i, EliasDeltaCodec::Decode(input_bit_stream));
    }
}