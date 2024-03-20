#ifndef SERF_I_COMPRESSOR_H
#define SERF_I_COMPRESSOR_H

#include <string>

class ICompressor {
public:
    virtual void addValue(double v) = 0;

    virtual const char *getBytes() = 0;

    virtual void close() = 0;

    virtual long getCompressedSizeInBits() = 0;

    virtual std::string getKey() {
        return typeid(*this).name();
    }

    virtual ~ICompressor() {}
};


#endif //SERF_I_COMPRESSOR_H
