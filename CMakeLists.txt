cmake_minimum_required(VERSION 3.30)
project(cache_simulator_new)

set(CMAKE_CXX_STANDARD 20)

option(NO_OUTPUT "Disable all terminal output" OFF)

if(NO_OUTPUT)
    add_compile_definitions(NO_OUTPUT)
endif()


add_executable(cache_simulator_new main.cpp
        Memory.cpp
        Memory.h
        MemoryBlock.cpp
        MemoryBlock.h
        Cache.cpp
        Cache.h
        CacheSet.cpp
        CacheSet.h
        CacheLine.cpp
        CacheLine.h
        Simulate.cpp
        Simulate.h
        CacheTypes.h
        Tester.cpp
        Tester.h
)
