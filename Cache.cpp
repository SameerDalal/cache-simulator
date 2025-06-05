#include "Cache.h"
#include <math.h>
#include <cstdio>

Cache::Cache(int cacheLevel, int cacheSize, int associativity, int blockSize, int numSets, CacheType cacheType, ReplacementPolicy replacementPolicy, WritePolicy writePolicy, int lineSize)
    : cacheLevel(cacheLevel), cacheSize(cacheSize), associativity(associativity), blockSize(blockSize), numSets(numSets), cacheType(cacheType), replacementPolicy(replacementPolicy), writePolicy(writePolicy),
    next(nullptr), memory(nullptr), reads(0), readMisses(0), writes(0), writeMisses(0), writeBacks(0) {

    cacheSets.reserve(numSets);
    for (int i = 0; i < numSets; ++i) {
        cacheSets.push_back(CacheSet(associativity, lineSize));
    }
}


void Cache::print() {

    printf("Cache L%d:\n", this->cacheLevel);

    printf("       ");
    for (int j = 0; j < this->associativity; j++) {
        printf("Way #%d:", j);
        for (int i = 0; i < this->blockSize; i++) {
            printf("    ");
        }
        printf("|    ");
    }
    printf("\n");

    for (int i = 0; i < this->numSets; i++) {
        printf("Set %d:", i);
        this->cacheSets.at(i).print();
        printf("\n");
    }


}

// returns a vector containing the tag, index, and block offset respectively
std::vector<unsigned int> Cache::parseAddress(uint32_t address) {

    std::vector<unsigned int> parsedAddress;

    int numBlockOffsetBits = static_cast<int>(log2(this->blockSize));
    int numIndexBits = static_cast<int>(log2(this->numSets));
    int numTagBits = 32 - numIndexBits - numBlockOffsetBits;

    unsigned int blockOffset = address & ((1U << numBlockOffsetBits) - 1);
    unsigned int index = address >> numBlockOffsetBits & ((1U << numIndexBits) -1);
    unsigned int tag = address >> (numBlockOffsetBits + numIndexBits);

    parsedAddress.push_back(tag);
    parsedAddress.push_back(index);
    parsedAddress.push_back(blockOffset);

    return parsedAddress;
}

// if function returns null means cache miss
std::vector<uint8_t> Cache::read(uint32_t address) {
    std::vector<unsigned int> parsedAddress = parseAddress(address);
    unsigned int tag = parsedAddress.at(0);
    unsigned int index = parsedAddress.at(1);
    unsigned int blockOffset = parsedAddress.at(2);
    printf("L%d read request (tag=%d, index=%d)\n", this->cacheLevel, tag, index);
    this->reads++;
    CacheLine* foundLine = this->cacheSets.at(index).find(tag);
    if (foundLine == nullptr) {
        printf("L%d read miss\n", this->cacheLevel);
        this->readMisses++;
        // chose the victim here
        int victimIndex = this->cacheSets.at(index).selectVictim(this->replacementPolicy);
        CacheLine* victimLine = &this->cacheSets.at(index).lines.at(victimIndex);

        if (victimLine->dirty) {
            printf("L%d dirty victim block\n", this->cacheLevel);
            // reconstruct the address here
            uint32_t new_address = reconstructAddress(index, victimLine->tag);
            if (this->next == nullptr) {
                printf("L%d issues write request to MEM\n", this->cacheLevel);
                this->writeBacks++;
                this->memory->write(new_address, victimLine->data, true);
                printf("MEM performs L%d's write request ", this->cacheLevel);
            } else {
                printf("L%d issues write request to L%d\n", this->cacheLevel, this->cacheLevel+1);
                this->writeBacks++;
                this->next->write(new_address, victimLine->data, true);
                printf("L%d performs L%d's write request ", this->cacheLevel+1, this->cacheLevel);
            }
            victimLine->dirty = false;
            printf("and sets dirty bit\n");
        } else {
            printf("L%d clean victim block\n", this->cacheLevel);
        }
        std::vector<uint8_t> newBlock;
        if (this->next == nullptr) {
            printf("L%d issues read request to MEM\n", this->cacheLevel);
            newBlock = this->memory->read(address);
        } else {
            printf("L%d issues read request to L%d\n", this->cacheLevel, this->cacheLevel+1);
            newBlock = this->next->read(address);
        }

        printf("L%d installs the block (tag=%d, index=%d, clean)\n", this->cacheLevel, tag, index);
        victimLine->data = newBlock;
        victimLine->valid = true;
        victimLine->dirty = false;
        victimLine->tag = tag;

        this->cacheSets.at(index).updateLRU(victimIndex); // update LRU bits

        return newBlock;

    } else {
        printf("L%d read hit\n", this->cacheLevel);
        return foundLine->data;
    }
    return {};
}

// returns the index of the victim block in the set
int Cache::selectVictim(unsigned int index) {

    return this->cacheSets.at(index).selectVictim(this->replacementPolicy);

}

uint32_t Cache::reconstructAddress(unsigned int index, unsigned int tag) {

    int numIndexBits = static_cast<int>(log2(this->numSets));
    int numBlockOffsetBits = static_cast<int>(log2(this->blockSize));

    tag <<= numIndexBits + numBlockOffsetBits;
    index <<= numBlockOffsetBits;
    uint32_t reconstructedAddress = tag | index; // adding tag to index

    return reconstructedAddress;
}


void Cache::write(uint32_t address, std::vector<uint8_t> data, bool fullBlock) {
    std::vector<unsigned int> parsedAddress = parseAddress(address);
    unsigned int tag = parsedAddress.at(0);
    unsigned int index = parsedAddress.at(1);
    unsigned int blockOffset = parsedAddress.at(2);

    printf("L%d write request (tag=%d, index=%d)\n", this->cacheLevel, tag, index);
    this->writes++;
    CacheLine* foundLine = this->cacheSets.at(index).find(tag);
    if (foundLine == nullptr) {
        printf("L%d write miss\n", this->cacheLevel);
        this->writeMisses++;
        if (this->writePolicy == WBWA || this->writePolicy == WTWA) {
            // chose the victim here
            int victimIndex = this->cacheSets.at(index).selectVictim(this->replacementPolicy);
            CacheLine* victimLine = &this->cacheSets.at(index).lines.at(victimIndex);

            if (victimLine->dirty) {
                printf("L%d dirty victim block\n", this->cacheLevel);
                // reconstruct the address here
                uint32_t new_address = reconstructAddress(index, victimLine->tag);
                if (this->next == nullptr) {
                    printf("L%d issues write request to MEM\n", this->cacheLevel);
                    this->writeBacks++;
                    this->memory->write(new_address, victimLine->data, true);
                    printf("MEM performs L%d's write request ", this->cacheLevel);
                } else {
                    printf("L%d issues write request to L%d\n", this->cacheLevel, this->cacheLevel+1);
                    this->writeBacks++;
                    this->next->write(new_address, victimLine->data, true);
                    printf("L%d performs L%d's write request ", this->cacheLevel+1, this->cacheLevel);
                }
                victimLine->dirty = false;
                printf("and sets dirty bit\n");
            } else {
                printf("L%d clean victim block\n", this->cacheLevel);
            }
            std::vector<uint8_t> newBlock;
            if (this->next == nullptr) {
                printf("L%d issues read request to MEM\n", this->cacheLevel);
                newBlock = this->memory->read(address);
            } else {
                printf("L%d issues read request to L%d\n", this->cacheLevel, this->cacheLevel+1);
                newBlock = this->next->read(address);
            }

            printf("L%d installs the block (tag=%d, index=%d, clean)\n", this->cacheLevel, tag, index);
            victimLine->data = newBlock;
            victimLine->valid = true;
            victimLine->dirty = false;
            victimLine->tag = tag;

            this->cacheSets.at(index).updateLRU(victimIndex); // update LRU bits

            printf("L%d performs CPU's write and sets dirty bit\n", this->cacheLevel);
            if (fullBlock) {
                victimLine->data = data;
            } else {
                victimLine->data.at(blockOffset) = data.at(0);
            }
            victimLine->dirty = true;

        } else {
            // WBNA or WTNA
            if (this->next == nullptr) {
                // Write directly to memory
                this->memory->write(address, data, fullBlock);
            } else {
                // Write directly to next cache level
                this->next->write(address, data, fullBlock);
            }
        }
    } else {
        printf("L%d write hit\n", this->cacheLevel);
        printf("L%d performs CPU's write", this->cacheLevel);
        if (fullBlock) {
            foundLine->data = data;
        } else {
            foundLine->data.at(blockOffset) = data.at(0);
        }
        if (this->writePolicy == WBWA || this->writePolicy == WBNA) {
            printf(" and sets dirty bit\n");
            foundLine->dirty = true;
        } else {
            // WTWA or WTNA
            printf("\n");
            if (this->next == nullptr) {
                printf("L%d issues write request to MEM\n", this->cacheLevel);
                this->writeBacks++;
                this->memory->write(address, data, fullBlock);
                printf("MEM performs L%d's write request\n", this->cacheLevel);
            } else {
                printf("L%d issues write request to L%d\n", this->cacheLevel, this->cacheLevel+1);
                this->writeBacks++;
                this->next->write(address, data, fullBlock);
                printf("L%d performs L%d's write request\n", this->cacheLevel+1, this->cacheLevel);
            }
        }
    }
}

uint8_t Cache::readByte(uint32_t address) {
    std::vector<unsigned int> parsedAddress = parseAddress(address);
    std::vector<uint8_t> block = this->read(address);
    return block.at(parsedAddress.at(2));
}

void Cache::stats() {
    printf("-------Cache L%d Stats-------\n", this->cacheLevel);
    printf("L%d reads: %d\n", this->cacheLevel, this->reads);
    printf("L%d read misses: %d\n", this->cacheLevel, this->readMisses);
    printf("L%d writes: %d\n", this->cacheLevel, this->writes);
    printf("L%d write misses: %d\n", this->cacheLevel, this->writeMisses);
    if (this->next == nullptr) {
        printf("L%d writebacks to MEM: %d\n", this->cacheLevel, this->writeBacks);
    } else {
        printf("L%d writebacks to L%d: %d\n", this->cacheLevel, this->cacheLevel+1, this->writeBacks);
    }
}
