
#ifndef CACHE_H
#define CACHE_H
#include "CacheSet.h"
#include "Memory.h"
#include "CacheTypes.h"

class Cache {
public:

    int cacheLevel;
    int cacheSize;
    int associativity;
    int blockSize;
    int numSets;
    CacheType cacheType;
    ReplacementPolicy replacementPolicy;
    WritePolicy writePolicy;
    std::vector<CacheSet> cacheSets;
    Cache* next; // points to the next level in the cache hierarchy. If nullptr then next level is memory
    Memory* memory;// only the lowest cache level will point to memory. All other cache levels will contain nullptr

    int reads;
    int readMisses;
    int writes;
    int writeMisses;
    int writeBacks;

    Cache(int cacheLevel, int cacheSize, int associativity, int blockSize, int numSets, CacheType cacheType, ReplacementPolicy replacementPolicy, WritePolicy writePolicy, int lineSize);

    void print();

    std::vector<uint8_t> read(uint32_t address);

    void write(uint32_t address, std::vector<uint8_t> value, bool fullBlock);

    std::vector<unsigned int> parseAddress(uint32_t address);

    int selectVictim(unsigned int index);

    uint32_t reconstructAddress(unsigned int index, unsigned int tag);

    uint8_t readByte(uint32_t address);

    void stats();

};


#endif //CACHE_H
