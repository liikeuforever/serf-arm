#ifndef SERF_ARRAY_H
#define SERF_ARRAY_H

#include <initializer_list>
#include <memory>
#include <stdexcept>

template<typename T>
class Array {
public:
    int length = 0;
    std::unique_ptr<T []> _data = nullptr;

public:
    Array<T> () = default;

    explicit Array<T> (int length): length(length) {
        _data = std::make_unique<T []>(this->length);
    }

    Array<T> (std::initializer_list<T> list): length(list.size()) {
        this->_data = std::make_unique<T []>(this->length);
        std::copy(list.begin(), list.end(), this->begin());
    }

    Array<T> (const Array<T> &other): length(other.length) {
        this->_data = std::make_unique<T []>(this->length);
        std::copy(other.begin(), other.end(), this->begin());
    }

    Array<T> &operator = (const Array<T> &right) {
        this->length = right.length;
        this->_data = std::make_unique<T []>(right.length);
        std::copy(right.begin(), right.end(), this->begin());
        return *this;
    }

    T &operator [] (int index) const {
        return _data[index];
    }

    T *begin() const {
        return _data.get();
    }

    T *end() const {
        return _data.get() + length;
    }
};

#endif //SERF_ARRAY_H
