#include "Tester.h"
#include "Cache.h"
#include "Memory.h"
#include "Simulate.h"
#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>


bool isValidCacheConfiguration(int cacheSize, int associativity, int blockSize) {

    if (cacheSize < (associativity * blockSize)) {
        return false;
    }

    int numSets = cacheSize / (associativity * blockSize);
    if (numSets < 1) {
        return false;
    }

    if (cacheSize % (associativity * blockSize) != 0) {
        return false;
    }

    if ((associativity & (associativity - 1)) != 0) {
        return false;
    }

    return true;
}


void runTest(int testNum, int cacheSize, int cacheSizePower, int blockSize, int blockSizePower, int associativity, std::string traceFileName) {
    int memorySize = pow(2,20);
    int numMemoryBlocks = memorySize / blockSize;

    Memory memory = Memory(memorySize, blockSize, numMemoryBlocks);
    memory.randomize();

    int numCacheSets = cacheSize / (associativity * blockSize);

    CacheType cacheType;
    if (associativity == 1) {
        cacheType = DIRECT_MAPPED;
    } else if (numCacheSets == 1) {
        cacheType = FULLY_ASSOCIATIVE;
    } else {
        cacheType = SET_ASSOCIATIVE;
    }

    Cache cache(1, cacheSize, associativity, blockSize, numCacheSets, cacheType, LRU, WBWA, blockSize);
    cache.memory = &memory;

    std::vector<Cache> caches;
    caches.push_back(cache);

    std::ifstream traceFile("../traces/" + traceFileName);
    if (!traceFile) {
        std::cerr << "Error opening file " << traceFileName << std::endl;
        return;
    }

    std::string command;
    while (traceFile >> command) {
        if (command == "readmem") {
            uint32_t address;
            if (traceFile >> std::hex >> address) {
                caches.at(0).readByte(address);
            }
        } else if (command == "writemem") {
            uint32_t address;
            uint32_t value = 32;
            if (traceFile >> std::hex >> address) {
                caches.at(0).write(address, std::vector{static_cast<uint8_t>(value)}, false);
            }
        }
    }

    traceFile.close();

    // saving the results
    if (testNum == 1) {
        std::ofstream outputFile("../data/miss_rate_vs_cache_size.csv", std::ios::app);

        if (!outputFile) {
            std::cerr << "Error opening file miss_rate_vs_cache_size.csv" << std::endl;
            return;
        }
        int totalOperations = caches.at(0).writes + caches.at(0).reads;
        int misses = caches.at(0).writeMisses + caches.at(0).readMisses;
        double missRate = static_cast<double>(misses)/totalOperations;
        outputFile << missRate << "," << caches.at(0).cacheSize << "," << associativity << "," << cacheSizePower << std::endl;
        outputFile.close();
    } else {

        std::ofstream outputFile("../data/miss_rate_vs_block_size.csv", std::ios::app);

        if (!outputFile) {
            std::cerr << "Error opening file miss_rate_vs_block_size.csv" << std::endl;
            return;
        }
        int totalOperations = caches.at(0).writes + caches.at(0).reads;
        int misses = caches.at(0).writeMisses + caches.at(0).readMisses;
        double missRate = static_cast<double>(misses)/totalOperations;
        outputFile << missRate << "," << caches.at(0).cacheSize << "," << associativity << "," << cacheSizePower << "," << blockSize << "," << blockSizePower << std::endl;
        outputFile.close();
    }
}

void runTests() {
    printf("Starting cache tests\n");

    printf("Which tests?\n");
    printf("1. Miss rate vs Cache Size (Varying associativites)\n");
    printf("2. Miss rate vs Block Size (Varying cache sizes)\n");
    int testNumber;
    std::cin >> testNumber;

    printf("Which trace file?\n");
    std::string traceFileNumber;
    std::cin >> traceFileNumber;
    std::string traceFileName = "trace" + traceFileNumber + ".txt";

    if (testNumber == 1) {
        std::ofstream clearFile("../data/miss_rate_vs_cache_size.csv");
        clearFile << "miss_rate,cache_size,associativity,cache_size_power\n";
        clearFile.close();

        std::vector<int> associativities = {1, 2, 4, 8, 16};
        int blockSizePower = 3;

        for (int associativity : associativities) {
            for (int cacheSizePower = 6; cacheSizePower <= 18; cacheSizePower++) {
                int cacheSize = pow(2,cacheSizePower);
                int blockSize = pow(2,blockSizePower);
                if (isValidCacheConfiguration(cacheSize, associativity, blockSize)) {
                    runTest(testNumber, cacheSize, cacheSizePower, blockSize, blockSizePower, associativity, traceFileName);
                }
            }
        }
    } else {
        std::ofstream clearFile("../data/miss_rate_vs_block_size.csv");
        clearFile << "miss_rate,cache_size,associativity,cache_size_power,block_size,block_size_power\n";
        clearFile.close();
        int associativity = 4;
        for (int cacheSizePower = 10; cacheSizePower <= 15; cacheSizePower++) {
            for (int blockSizePower = 4; blockSizePower <= 8; blockSizePower++) {
                int cacheSize = pow(2,cacheSizePower);
                int blockSize = pow(2,blockSizePower);
                if (isValidCacheConfiguration(cacheSize, associativity, blockSize)) {
                    runTest(testNumber, cacheSize, cacheSizePower, blockSize, blockSizePower, associativity, traceFileName);
                }
            }
        }
    }

    printf("All cache tests completed\n");
}

