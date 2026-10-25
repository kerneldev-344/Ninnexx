#include <stdint.h>
#include <kernel/filesystem/fat_filelib.h>

uint8_t currentFileHandleNum = 5; // meow

struct FileHandle {
    bool isActive;
    FL_FILE *file;
    bool read;
    bool write;
};

struct FileHandle fhlist[256];