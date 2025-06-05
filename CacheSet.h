
#ifndef CACHESET_H
#define CACHESET_H
#include "CacheLine.h"
#include "CacheTypes.h"



class CacheSet {
    public:

    int numLines;
    std::vector<CacheLine> lines;

    CacheSet(int numLines, int lineSize);

    void print();

    CacheLine* find(unsigned int tag);

    bool isDirty();

    int selectVictim(ReplacementPolicy replacementPolicy);

    int getRandomNumber(int max);

    void updateLRU(int victimIndex);

};



#endif //CACHESET_H
