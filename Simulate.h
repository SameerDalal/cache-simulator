

#ifndef SIMULATE_H
#define SIMULATE_H

#include <string>

#include "Cache.h"
#include "Memory.h"

void simulate(Memory& memory, std::vector<Cache>& caches, std::string traceFile);

void save(Memory& memory, std::vector<Cache>& caches);

void customSetup();

#endif //SIMULATE_H
