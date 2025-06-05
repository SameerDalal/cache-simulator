

#ifndef CACHETYPES_H
#define CACHETYPES_H

typedef enum {
    DIRECT_MAPPED,
    SET_ASSOCIATIVE,
    FULLY_ASSOCIATIVE
} CacheType;

typedef enum {
    LRU,
    RAND
} ReplacementPolicy;

typedef enum {
    WTWA,
    WBWA,
    WTNA,
    WBNA
} WritePolicy;


#endif //CACHETYPES_H
