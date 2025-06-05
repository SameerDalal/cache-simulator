

#ifndef CACHELINE_H
#define CACHELINE_H
#include <cstdint>
#include <vector>


class CacheLine {
public:

    int lineSize;
    bool valid;
    bool dirty;
    unsigned int tag;
    unsigned int lru;
    std::vector<uint8_t> data;

    CacheLine(int lineSize, unsigned int lru);

    void print();

};



#endif //CACHELINE_H
