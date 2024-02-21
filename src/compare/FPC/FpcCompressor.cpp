#include "FpcCompressor.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <iostream>
#include <bitset>

long FpcCompressor::getCompressedSizeInBits()
{
    // out -= (bcode + (bcode >> 2));
    // return 8 * (((out >> 3) << 3) + 8);

    return compressedSizeInBits;
}
const long long FpcCompressor::mask[8] =
    {0x0000000000000000LL,
     0x00000000000000ffLL,
     0x000000000000ffffLL,
     0x0000000000ffffffLL,
     0x000000ffffffffffLL,
     0x0000ffffffffffffLL,
     0x00ffffffffffffffLL,
     static_cast<long long>(0xffffffffffffffff)};
FpcCompressor::FpcCompressor(long pred, int num)
{
    intot = num;
    predsizem1 = pred;

    outbuf[0] = predsizem1;
    predsizem1 = (1L << predsizem1) - 1;

    hash = 0;
    dhash = 0;
    lastval = 0;
    pred1 = 0;
    pred2 = 0;
    fcm = (long long *)calloc(predsizem1 + 1, 8);
    dfcm = (long long *)calloc(predsizem1 + 1, 8);

    out = ((intot + 1));
    *((long long *)&outbuf[(out >> 3) << 3]) = 0;
};
FpcCompressor::~FpcCompressor()
{
    free(fcm);
    free(dfcm);
};
void FpcCompressor::addValue(double v)
{
    memcpy(&val, &v, sizeof(double));
    xor1 = val ^ pred1;
    fcm[hash] = val;
    hash = ((hash << 6) ^ ((unsigned long long)val >> 48)) & predsizem1;
    pred1 = fcm[hash];

    stride = val - lastval;
    xor2 = val ^ (lastval + pred2);
    lastval = val;
    dfcm[dhash] = stride;
    dhash = ((dhash << 2) ^ ((unsigned long long)stride >> 40)) & predsizem1;
    pred2 = dfcm[dhash];

    code = 0;
    if ((unsigned long long)xor1 > (unsigned long long)xor2)
    {
        code = 0x80;
        xor1 = xor2;
    }
    bcode = 7; // 8 bytes
    if (0 == (xor1 >> 56))
        bcode = 6; // 7 bytes
    if (0 == (xor1 >> 48))
        bcode = 5; // 6 bytes
    if (0 == (xor1 >> 40))
        bcode = 4; // 5 bytes
    if (0 == (xor1 >> 24))
        bcode = 3; // 3 bytes
    if (0 == (xor1 >> 16))
        bcode = 2; // 2 bytes
    if (0 == (xor1 >> 8))
        bcode = 1; // 1 byte
    if (0 == xor1)
        bcode = 0; // 0 bytes

    *((long long *)&outbuf[(out >> 3) << 3]) |= xor1 << ((out & 0x7) << 3); // 从左往右写入数据
    if (0 == (out & 0x7))                                                   // out是8的倍数
        xor1 = 0;
    *((long long *)&outbuf[((out >> 3) << 3) + 8]) = (unsigned long long)xor1 >> (64 - ((out & 0x7) << 3));

    long tmp = out;
    out += bcode + (bcode >> 2); // 0 ~ 8 需要写入的数据大小
    code |= bcode << 4;
    outbuf[i] = code;
    i++;

    // 第一个outbuf写入数据时，前面的所有数据都已经稳定，可以write进数据流中
    outStream.writeInt(code >> 4, 4);
    compressedSizeInBits += 4;

    _tmp_ = ((tmp >> 3) << 3); // 当前起始位置
    _out_ = ((out >> 3) << 3); // 下一轮的位置
    if (_tmp_ < _out_)         // 如果不变，就不写，等待下一轮，但写入的数据肯定到达_out_-1
    {
        for (int j = _tmp_; j < _out_; j++)
        {
            outStream.writeLong(outbuf[j], 8);
            compressedSizeInBits += 8;
        }
    }
    // 最后一轮的情况
    // ①_tmp_ = _out_，写入停留在_tmp_-1 = _out_-1，还需要写入 _tmp_ ~ _tmp_ + 8 + 8
    // ②_tmp_ < _out_，写入停留在_out_-1，还需要写入 _out_-1~ _tmp_ + 8 + 8
};

std::vector<char> FpcCompressor::getBytes()
{
    // std::vector<char> result;
    // result.reserve(6 + (SIZE / 2) + (SIZE * 8) + 2);
    // for (int j = 0; j < 6 + (SIZE / 2) + (SIZE * 8) + 2; ++j)
    // {
    //     result.push_back(static_cast<char>(outbuf[j]));
    // }
    // return result;

    int byteCount = ceil(compressedSizeInBits / 8.0);
    std::vector<char> result;
    result.reserve(byteCount);
    auto bytes = outStream.getBuffer();
    for (int i = 0; i < byteCount; ++i)
    {
        result.push_back(static_cast<char>(bytes[i]));
    }
    return result;
};
void FpcCompressor::close()
{
    if (0 != (intot & 1))
    {
        out -= bcode + (bcode >> 2);
    }
    i = 0;

    for (int j = _out_; j < _tmp_ + 16; j++)
    {
        outStream.writeLong(outbuf[j], 8);
        compressedSizeInBits += 8;
    }
};
