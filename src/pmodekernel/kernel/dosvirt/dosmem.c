// #include <boolean.h>
#include <stdlib.h>
#include <kernel/dosvirt/dosmem.h>
#include <kernel/processes/process.h>
#include <kernel/memory/vmm.h>
#include <kernel/memory.h>
#include <stdint.h>

// extern struct DosMemHeader;
extern struct Process* current;

// NOTE STARTADDR IS ACTUALLY STARTSEGMENT

uint32_t allocMem(uint16_t numPG) {
    if (numPG == 0) {
        return 0;
    }

    if (current->memHead == NULL) {
        current->memHead = calloc(1, sizeof(struct DosMemHeader));

        if (current->memHead == NULL) {
            return 0;
        }

        current->memHead->startAddr = 0x1000;
        current->memHead->numPG = numPG;
        current->memHead->free = false;

        for (uint32_t i = current->memHead->startAddr;
             i < (uint32_t)current->memHead->startAddr + numPG;
             i++) {
            vmmAllocatePage(i*16, true, VMM_WRITABLE);
        }

        return current->memHead->startAddr;
    }

    struct DosMemHeader* currentH = current->memHead;

    while (currentH != NULL) {
        if (currentH->free && currentH->numPG >= numPG) {
            currentH->free = false;
            return currentH->startAddr;
        }

        if (currentH->nextHeader == NULL) {
            struct DosMemHeader* newHeader =
                calloc(1, sizeof(struct DosMemHeader));

            if (newHeader == NULL) {
                return 0;
            }

            newHeader->startAddr =
                currentH->startAddr + currentH->numPG;
            newHeader->numPG = numPG;
            newHeader->free = false;

            currentH->nextHeader = newHeader;

            for (uint32_t i = newHeader->startAddr;
                 i < newHeader->startAddr + numPG;
                 i++) {
                vmmAllocatePage(i*16, true, VMM_WRITABLE);
            }

            return newHeader->startAddr;
        }

        currentH = currentH->nextHeader;
    }

    return 0;
}
