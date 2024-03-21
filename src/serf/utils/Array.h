#ifndef SERF_ARRAY_H
#define SERF_ARRAY_H

#include <memory>

#include "serf/utils/InputBitStream.h"

template<typename T>
class Array {
public:
    int length;
    std::unique_ptr<T []> _data = nullptr;

public:
    explicit Array<T> (int length): length(length) {
        this->_data = std::make_unique<T []>(this->length);
    };

    Array<T> (std::initializer_list<T> list): length(list.size()) {
        this->_data = std::make_unique<T []>(this->length);
        std::copy(list.begin(), list.end(), _data.get());
    }

    Array<T> (const Array<T> &other): length(other.length) {
        this->_data = std::make_unique<T []>(this->length);
        std::copy(other._data.get(), other._data.get() + other.length, _data.get());
    }

    Array<T> &operator = (const Array<T> &right) {
        this->_data.reset(new T[right.length]);
        std::copy(right._data.get(), right._data.get() + right.length, _data.get());
        this->length = right.length;
        return *this;
    }

    T &operator [] (int index) {
        return this->_data[index];
    };
};

#endif //SERF_ARRAY_H