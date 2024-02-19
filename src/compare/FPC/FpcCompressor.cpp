#include "FpcCompressor.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <iostream>
#include <bitset>

long FpcCompressor::getCompressedSizeInBits()
{
    return 1;
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

    out = 6 + ((intot + 1));
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

    *((long long *)&outbuf[(out >> 3) << 3]) |= xor1 << ((out & 0x7) << 3);
    if (0 == (out & 0x7))
        xor1 = 0;
    *((long long *)&outbuf[((out >> 3) << 3) + 8]) = (unsigned long long)xor1 >> (64 - ((out & 0x7) << 3));

    out += bcode + (bcode >> 2);
    code |= bcode << 4;

    outbuf[6 + i] = code;
    i++;
};

std::vector<char> FpcCompressor::getBytes()
{
    std::vector<char> result;
    result.reserve(6 + (SIZE / 2) + (SIZE * 8) + 2);
    for (int j = 0; j < 6 + (SIZE / 2) + (SIZE * 8) + 2; ++j)
    {
        result.push_back(static_cast<char>(outbuf[j]));
    }
    return std::move(result);
};
void FpcCompressor::close()
{
    if (0 != (intot & 1))
    {
        out -= bcode + (bcode >> 2);
    }
    outbuf[0] = intot;
    outbuf[1] = intot >> 8;
    outbuf[2] = intot >> 16;
    outbuf[3] = out;
    outbuf[4] = out >> 8;
    outbuf[5] = out >> 16;
    i = 0;
};
