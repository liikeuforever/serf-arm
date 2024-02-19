#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <vector>
#define SIZE 32768
class FpcDeCompressor
{
public:
    static const long long mask[8];
    long i, in, intot, hash, dhash, code, bcode, predsizem1, end, tmp, ioc;
    long long val, lastval, stride, pred1, pred2, next;
    long long *fcm, *dfcm;
    long long outbuf[SIZE];
    unsigned char inbuf[(SIZE / 2) + (SIZE * 8) + 6 + 2];

    FpcDeCompressor(long pred);
    ~FpcDeCompressor();

    void setBytes(char *data, size_t data_size);

    void close();

    void refresh();
    std::vector<double> decompress();
    std::vector<double> result;
};