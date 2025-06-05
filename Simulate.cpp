#include "Simulate.h"
#include <fstream>
#include <iostream>
#include <string>
#include <cmath>


void customSetup() {
    std::ifstream inputFile("../custom_setup.txt");
    if (!inputFile) {
        std::cerr << "Failed to open custom_setup.txt file." << std::endl;
        return;
    }

    printf("Number of Caches:\n");
    int numCaches;
    inputFile >> numCaches;
    printf("%d Caches\n", numCaches);

    printf("BLOCK_SIZE(2^N bytes): Same for all caches and memory\n");
    int blockSize;
    inputFile >> blockSize;
    printf("2^%d bytes\n", blockSize);
    blockSize = pow(2, blockSize);

    printf("MEMORY_SIZE(2^N bytes)\n");
    int memorySize;
    inputFile >> memorySize;
    printf("2^%d bytes\n", memorySize);
    memorySize = pow(2, memorySize);
    int numMemoryBlocks = memorySize/blockSize;

    // memory object
    Memory memory = Memory(memorySize, blockSize, numMemoryBlocks);
    memory.randomize();

    // cache objects
    std::vector<Cache> caches;

    for (int i = 1; i <= numCaches; i++) {
        printf("L%d Cache: CACHE_SIZE(2^N bytes) ASSOCIATIVITY(2^N ways) REPLACEMENT_POLICY WRITE_POLICY\n", i);
        int cacheSize;
        int associativity;
        std::string replacementPolicy;
        std::string writePolicy;

        inputFile >> cacheSize;
        printf("2^%d bytes\n", cacheSize);
        inputFile >> associativity;
        printf("2^%d ways\n", associativity);
        inputFile >> replacementPolicy;
        printf("%s\n", replacementPolicy.c_str());
        inputFile >> writePolicy;
        printf("%s\n", writePolicy.c_str());

        cacheSize = pow(2, cacheSize);
        associativity = pow(2, associativity);

        int numCacheSets = cacheSize/(associativity*blockSize);

        CacheType cacheType;
        if (associativity == 1) {
            cacheType = DIRECT_MAPPED;
        } else if (numCacheSets == 1) {
            cacheType = FULLY_ASSOCIATIVE;
        } else {
            cacheType = SET_ASSOCIATIVE;
        }

        ReplacementPolicy cacheReplacementPolicy;

        if (replacementPolicy == "LRU") {
            cacheReplacementPolicy = LRU;
        } else {
            cacheReplacementPolicy = RAND;
        }

        WritePolicy cacheWritePolicy;

        if (writePolicy == "WTWA") {
            cacheWritePolicy = WTWA;
        } else if (writePolicy == "WBWA") {
            cacheWritePolicy = WBWA;
        } else if (writePolicy == "WTNA") {
            cacheWritePolicy = WTNA;
        } else {
            cacheWritePolicy = WBNA;
        }

        caches.push_back(Cache(i, cacheSize, associativity, blockSize, numCacheSets, cacheType, cacheReplacementPolicy, cacheWritePolicy, blockSize));

    }

    for (int i = 0; i < numCaches; i++) {
        if (i != 0) {
            //the last cache's next points to this cache
            caches.at(i-1).next = &caches.at(i);
        }
        if (i+1 == numCaches) {
            // the last cache (lowest in the hierarchy) points to memory.
            caches.at(i).memory = &memory;
        }
    }

    inputFile.close();

    printf("Which trace file?\n");

    std::string traceFileNumber;
    std::cin >> traceFileNumber;

    std::string traceFile = "trace" + traceFileNumber + ".txt";

    simulate(memory, caches, traceFile);
}



void simulate(Memory& memory, std::vector<Cache>& caches, std::string traceFile) {

    std::ifstream inputFile("../traces/" + traceFile);

    if (!inputFile) {
        std::cerr << "Failed to open " << traceFile << "file.\n" << std::endl;
        return;
    }

    std::string command;
    while (inputFile >> command) {
        printf("\n");
        if (command == "readmem") {
            uint32_t address;
            if (inputFile >> std::hex >> address) {
                printf("Reading value at address %x\n", address);
                //in the vector the first element will always be the highest cache
                uint8_t value = caches.at(0).readByte(address);
                printf("Value at address %x: %d\n", address, value);
            } else {
                std::cerr << "Failed to read address after readmem\n" << std::endl;
                break;
            }
            continue;
        }

        if (command == "writemem") {
            uint32_t address;
            uint32_t value = 32; //writing some random value
            if (inputFile >> std::hex >> address) {
                printf("Writing %d at address %x\n", static_cast<uint8_t>(value), address);
                caches.at(0).write(address, std::vector{static_cast<uint8_t>(value)}, false);
                printf("Wrote %d at address %x\n", static_cast<uint8_t>(value), address);
            } else {
                std::cerr << "Failed to read address after writemem\n" << std::endl;
                break;
            }
            continue;
        }
    }

    while (true) {
        std::cin >> command;
        if (command == "quit") {
            break;
        }
        if (command == "stats") {
            for (auto & cache : caches) {
                cache.stats();
            }
            memory.stats();
            continue;
        }
        if (command == "save") {
            save(memory, caches);
            continue;
        }
        if (command == "printmem") {
            memory.print();
            continue;
        }
        if (command == "printcache") {
            for (auto & cache : caches) {
                cache.print();
            }
            continue;
        }
    }
}

void save(Memory& memory, std::vector<Cache>& caches) {
    std::ofstream outputFile("../data/custom_setup_results.csv", std::ios::app);

    if (!outputFile) {
        std::cerr << "Failed to open custom_setup_results.csv file.\n" << std::endl;
        return;
    }

    double missRate = 0.0;
    int totalOperations = caches.at(0).writes + caches.at(0).reads;
    if (totalOperations > 0) {
        missRate = static_cast<double>(caches.at(0).writeMisses + caches.at(0).readMisses) / totalOperations;
    }

    printf("Saving results: miss_rate=%.3f, cache_size=%d\n", missRate, caches.at(0).cacheSize);
    outputFile << missRate << "," << caches.at(0).cacheSize << std::endl;
    outputFile.close();
}