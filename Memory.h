#include "MemoryBlock.h"

#ifndef MEMORY_H
#define MEMORY_H



class Memory {
public:
   int size;
   int blockSize;
   int numBlocks;
   std::vector<MemoryBlock> blocks;

   int memoryTraffic;

   Memory(int size, int numBlocks, int blockSize);

   void print();

   std::vector<uint8_t> read(uint32_t address);

   void write(uint32_t address, std::vector<uint8_t> value, bool fullBlock);

   std::vector<uint8_t> parseAddress(uint32_t address);

   void randomize();

   void stats();

};



#endif //MEMORY_H
