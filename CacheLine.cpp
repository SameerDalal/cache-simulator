#include "CacheLine.h"
#include <cstdio>


CacheLine::CacheLine(int lineSize, unsigned int lru)
    : lineSize(lineSize), valid(false), dirty(false), tag(0), lru(lru), data(lineSize, 0x00)

{}


void CacheLine::print() {
    for (int i = 0; i < this->lineSize; i++) {
        printf(" 0x%02X", this->data.at(i));
    }
}

