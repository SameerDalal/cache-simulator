#include "Memory.h"
#include <cmath>
#include <cstdio>
#include "MemoryBlock.h"


// Member Initializer List
Memory::Memory(int size, int blockSize, int numBlocks)
    :size(size), blockSize(blockSize), numBlocks(numBlocks), blocks(numBlocks, MemoryBlock(blockSize)),
    memoryTraffic(0)
{}


void Memory::randomize() {
    for (int i = 0; i < this->numBlocks; i++) {
        this->blocks.at(i).randomize();
    }
}

void Memory::print() {
    for (int i = 0; i < this->numBlocks; i++) {
        printf("Block %d:", i);
        this->blocks.at(i).print();
    }
}

std::vector<uint8_t> Memory::parseAddress(uint32_t address) {
    std::vector<uint8_t> parsedAddress;

    int numBlockOffsetBits = static_cast<int>(log2(this->blockSize));

    unsigned int blockOffset = address & ((1U << numBlockOffsetBits) - 1);
    unsigned int block = address / blockSize;

    parsedAddress.push_back(block);
    parsedAddress.push_back(blockOffset);

    return parsedAddress;
}

std::vector<uint8_t> Memory::read(uint32_t address) {
    this->memoryTraffic++;
    std::vector<uint8_t> parsedAddress = parseAddress(address);

    unsigned int block = parsedAddress.at(0);

    printf("MEM read request (block %d)\n", block);

    return this->blocks.at(block).data;
}


void Memory::write(uint32_t address, std::vector<uint8_t> data, bool fullBlock) {
    this->memoryTraffic++;
    std::vector<uint8_t> parsedAddress = parseAddress(address);
    unsigned int block = parsedAddress.at(0);
    unsigned int blockOffset = parsedAddress.at(1);

    if (fullBlock) {
        this->blocks.at(block).data = data;
    } else {
        this->blocks.at(block).data.at(blockOffset) = data.at(0);
    }
}

void Memory::stats() {
    printf("-------MEM Stats-------\n");
    printf("Total MEM Traffic: %d\n", this->memoryTraffic);
}
