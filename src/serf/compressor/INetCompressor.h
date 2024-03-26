#ifndef SERF_I_NET_COMPRESSOR_H
#define SERF_I_NET_COMPRESSOR_H

#include <string>
#include <typeinfo>

class INetCompressor {
public:
    virtual ~INetCompressor() {}

    virtual const char *compress(double v) = 0;

    virtual std::string getKey() {
        return typeid(*this).name();
    }
};

#endif //SERF_I_NET_COMPRESSOR_H