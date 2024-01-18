// Implementation of int[] in java

#ifndef SERFNATIVE_INT_H
#define SERFNATIVE_INT_H

#include <initializer_list>

class IntArray {
private:
    int *data;
public:
    int length = 0;

    IntArray() {
        data = nullptr;
    }

    explicit IntArray(int count) {
        data = new int[count];
        length = count;
    }

    IntArray(const std::initializer_list<int> &list) {
        data = new int [list.size()];
        length = static_cast<int>(list.size());
        int index = 0;
        for (const auto &item: list) {
            data[index] = item;
            ++index;
        }
    }

    IntArray(const IntArray &intArray) {
        data = new int [intArray.length];
        for (int i = 0; i < intArray.length; ++i) {
            data[i] = intArray[i];
        }
        length = intArray.length;
    }

    int &operator[](int index) const {
        return data[index];
    }

    ~IntArray() {
        delete data;
    }
};

#endif //SERFNATIVE_INT_H
