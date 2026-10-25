#ifndef FILEHANDLES_H
#define FILEHANDLES_H
extern uint8_t currentFileHandleNum; // meow

struct FileHandle {
    bool isActive;
    FL_FILE *file;
    bool read;
    bool write;
    uint32_t cursor;
};
extern struct FileHandle fhlist[256];

#endif