// #include <boolean.h>
// #include <memory.h>
#include <stdint.h>


#ifndef DOSMEM_H
#define DOSMEM_H

struct DosMemHeader {
    bool free;
    uint32_t startAddr;
    uint16_t numPG;
    struct DosMemHeader* nextHeader;
};

uint32_t allocMem(uint16_t numPG);

#endif