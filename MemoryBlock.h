
#ifndef MEMORYBLOCK_H
#define MEMORYBLOCK_H
#include <cstdint>
#include <vector>


class MemoryBlock {
public:
    int size;
    std::vector<uint8_t> data;

    public:
    MemoryBlock(int size);

    void print();

    void randomize();
};



#endif //MEMORYBLOCK_H
