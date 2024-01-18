//
// Created by czc on 18/01/24.
//

#ifndef SERFNATIVE_INPUTBITSTREAM_H
#define SERFNATIVE_INPUTBITSTREAM_H

#include <fstream>
#include <bitset>

class InputBitStream {
private:
    std::ifstream &is_;
    int bufCapacity_;
    char *buffer_;
    long bufCount_;
    int bufPos_;

public:
    InputBitStream(std::ifstream &is, int bufCapacity) : is_(is), bufCapacity_(bufCapacity) {
        buffer_ = new char[bufCapacity_];
        bufCount_ = is_.readsome(buffer_, bufCapacity_);
        bufPos_ = 0;
    }

    bool readBitTo(bool *dst) {
        if ()
    }

    ~InputBitStream() {
        delete buffer_;
    }
};


#endif //SERFNATIVE_INPUTBITSTREAM_H
