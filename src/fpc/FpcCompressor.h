#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <vector>
#include <iostream>
#include <cstring>
#define SIZE 32768
#include "NewOutputBitStream.h"
#include <cmath>

class FpcCompressor
{
private:
    NewOutputBitStream outStream = NewOutputBitStream(6 + (SIZE / 2) + (SIZE * 8) + 2);

public:
    static const long long mask[8];
    long i, out, intot, hash, dhash, code, bcode, ioc;
    long long val, lastval, stride, pred1, pred2, xor1, xor2;
    long long *fcm, *dfcm;
    unsigned long long inbuf[SIZE + 1];
    unsigned char outbuf[6 + (SIZE / 2) + (SIZE * 8) + 2];
    long predsizem1 = 0;

    FpcCompressor(long pred, int intot);
    ~FpcCompressor();
    void addValue(double v);
    void close();
    void refresh();
    void Compress(long predsizem1);
    void Decompress();
    long getCompressedSizeInBits();
    std::vector<char> getBytes();

    int compressedSizeInBits = 0;

    long _tmp_;
    long _out_;
};