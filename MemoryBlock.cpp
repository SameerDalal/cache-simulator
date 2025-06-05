
#include "MemoryBlock.h"

#include <cstdio>
#include <stdlib.h>

//Member Initializer List Syntax
MemoryBlock::MemoryBlock(int size)
    :size(size), data(size, 0x00)
{}

void MemoryBlock::print() {
    for (int i = 0; i < size; i++) {
        printf(" 0x%02X", data[i]);
    }
    printf("\n");
}

void MemoryBlock::randomize() {
    for (int i = 0; i < size; i++) {
        this->data.at(i) = rand() % 256;
    }
}

