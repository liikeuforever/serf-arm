#include "EliasDeltaCodec.h"

int EliasDeltaCodec::encode(int64_t number, OutputBitStream &outputBitStream) {
    int compressedBits = 0;
    int32_t len;
    int32_t lengthOfLen;
    if (number <= 11) {
        len = 1 + (int) std::floor(logTable[number] / logTable[2]);
    } else {
        len = 1 + (int) std::floor(std::log(number) / logTable[2]);
    }
    if (len <= 11) {
        lengthOfLen = (int) std::floor(logTable[len] / logTable[2]);
    } else {
        lengthOfLen = (int) std::floor(std::log(len) / logTable[2]);
    }
    int totalLen = lengthOfLen + lengthOfLen + len;
    if (totalLen <= 64) {
        compressedBits += static_cast<int>(outputBitStream.writeLong(((int64_t) len << (len - 1)) |
                                                                  (number & ~(0xffffffffffffffffL << (len - 1))),
                                                                  totalLen));
    } else {
        compressedBits += static_cast<int>(outputBitStream.writeInt(0, lengthOfLen));
        compressedBits += static_cast<int>(outputBitStream.writeInt(len, lengthOfLen + 1));
        compressedBits += static_cast<int>(outputBitStream.writeLong(number, len - 1));
    }
    return compressedBits;
}

int64_t EliasDeltaCodec::decode(InputBitStream &inputBitStream) {
    int64_t num = 1;
    int32_t len = 1;
    int32_t lengthOfLen = 0;
    while (inputBitStream.readBit() == 0)
        lengthOfLen++;
    len <<= lengthOfLen;
    len |= static_cast<int32_t>(inputBitStream.readInt(lengthOfLen));
    num <<= (len - 1);
    num |= static_cast<int32_t>(inputBitStream.readLong(len - 1));
    return num;
}
