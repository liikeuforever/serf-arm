#ifndef SERF_I_COMPRESSOR_H
#define SERF_I_COMPRESSOR_H

#include <string>
#include <cstdint>
#include <typeinfo>
#include <memory>

class ICompressor {
public:
    virtual void addValue(double v) = 0;

    virtual std::unique_ptr<uint8_t []> getBytes() = 0;

    virtual void close() = 0;

    virtual long getCompressedSizeInBits() = 0;

    virtual std::string getKey() {
        return typeid(*this).name();
    }
};


#endif //SERF_I_COMPRESSOR_H
