#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <vector>

#include "serf/utils/InputBitStream.h"

#define SIZE 32768

class FpcDeCompressor {
private:
    InputBitStream inStream = InputBitStream(nullptr, 0);

public:
    static const long long mask[8];
    long i, in, intot, hash, dhash, code, bcode, predsizem1, end, tmp, ioc;
    long long val, lastval, stride, pred1, pred2, next;
    long long *fcm, *dfcm;
    long long outbuf[SIZE];
    unsigned char inbuf[(SIZE / 2) + (SIZE * 8) + 6 + 2];

    FpcDeCompressor(long pred, int num);
    ~FpcDeCompressor();

    void setBytes(char *data, size_t data_size);

    void close();

    void refresh();
    std::vector<double> decompress();
    std::vector<double> result;

    long _tmp_;
    long _out_;
};