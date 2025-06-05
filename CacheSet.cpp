#include "CacheSet.h"
#include <cstdio>
#include <iostream>
#include <ostream>
#include <stdlib.h>


CacheSet::CacheSet(int numLines, int line_size)
    : numLines(numLines)
{
    // each cache line should have a unique initial LRU value
    lines.reserve(numLines);
    for (int i = 0; i < numLines; i++) {
        lines.push_back(CacheLine(line_size, i));
    }

}

void CacheSet::print() {
    for (int i = 0; i < this->numLines; i++) {
        this->lines.at(i).print();
        printf("    ");
    }
}

CacheLine* CacheSet::find(unsigned int tag) {
    if (this->numLines > this->lines.size()) {
        std::cerr << "Cache set is out of bounds" << std::endl;
    }
    for (int i = 0; i < this->numLines; i++) {
        if (this->lines.at(i).tag == tag) {
            return &this->lines.at(i);
        }
    }
    return nullptr;
}

// checks if all the Lines in the CacheSet are dirty.
bool CacheSet::isDirty() {
    for (int i = 0; i < this->numLines; i++) {
        if (this->lines.at(i).dirty == false) {
            return false;
        }
    }
    return true;
}

int CacheSet::selectVictim(ReplacementPolicy replacementPolicy) {

    for (int i = 0; i < this->numLines; i++) {
        if (this->lines.at(i).valid == false) {
            // if a line within the set is uninitialized then select that as the victim block
            return i;
        }
    }
    if (replacementPolicy == LRU) {
        for (int i = 0; i < this->numLines; i++) {
            if (this->lines.at(i).lru == this->numLines - 1) {
                return i;
            }
        }
    } else if (replacementPolicy == RAND) {
        int randomNumber = getRandomNumber(this->numLines);
        return randomNumber;
    }

    printf("Did not select valid replacement policy\n");
    return -1;
}

int CacheSet::getRandomNumber(int max) {
    //max is non-inclusive
    return rand() % max;
}


void CacheSet::updateLRU(int victimIndex) {
    unsigned int victimIndexLRU = this->lines.at(victimIndex).lru;

    for (int i = 0; i < this->numLines; i++) {
        if (i == victimIndex) {
            this->lines.at(i).lru = 0;
        } else {
            if (this->lines.at(i).lru < victimIndexLRU) {
                this->lines.at(i).lru += 1;
            }
        }
    }
}
