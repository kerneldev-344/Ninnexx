#include <kernel/interrupts/intrStructs.h>
#include <kernel/logging.h>
#include <kernel/processes/process.h>
#include <stdio.h>
#include <kernel/dosvirt/dosmem.h>
#include <kernel/fskernelapi/loadfile.h>

extern struct Process* current;

void intr21(struct InterruptStackFrame* isf) {
    if (!(isf->eflags & (1 << 17))) {
        return; // not VM
    }

    struct InterruptStackFrameV8086* isfv = (struct InterruptStackFrameV8086*) isf;

    if (((isf->eax & 0x0000FF00) == 0x0200)) {
        // write character in dl to scren
        kputc((uint8_t)isfv->edx);
    } else if ((isfv->eax & 0x0000FF00) == 0x0900) {
        // print $ terminated string ds:dl
        uint32_t addr = 16*(isfv->dsHardware) + (0x0000FFFF & isfv->edx);
        while (*(char*)addr != '$') {
            kputc(*(char*)(addr++));
        }
    } else if ((isfv->eax & 0x0000FF00) == 0x4C00) {
        deleteProcess(current->procID);
        // kprint("hi");
    } else if ((isfv->eax & 0x0000FF00) == 0x0100) {
        char inpt;
        asm volatile(
            "start:\n\t"
            "xor %%eax, %%eax\n\t"
            "mov $0x02, %%eax\n\t"
            "int $0x80\n\t"
            "jc start\n\t"
            "movb %%al, %0\n\t"
            : "+m" (inpt)
            :
            : "eax"
        );

        if (inpt != '\r')
            kputc(inpt);

        isfv->eax &= 0xFFFFFF00;
        isfv->eax |= inpt;
    } else if ((isfv->eax & 0x0000FF00) == 0x0A00) {
        uint32_t addr = 16*(isfv->dsHardware) + (isfv->edx & 0x0000FFFF);
        if (addr >= 0xC0000000) {
            isfv->eflags|=1; // cf
            return;
        }
        uint8_t *len = (uint8_t*) addr;
        if (*len + addr + 2 >= 0xC0000000) {
            isfv->eflags|=1;
            return;
        }
        char *str = (char*) (addr+2);
        uint8_t i = 0;
        while (i < *len) {
            char inpt;
            asm volatile(
                "start1:\n\t"
                "xor %%eax, %%eax\n\t"
                "mov $0x02, %%eax\n\t"
                "int $0x80\n\t"
                "jc start1\n\t"
                "movb %%al, %0\n\t"
                : "+m" (inpt)
                :
                : "eax"
            );
            if (inpt == '\r') {
                break;
            }
            if (inpt == '\b') {
                i -= 1;
                *str = '\0';
                kdelprevc();
                continue;
            }
            if (inpt != '\r')
                kputc(inpt);
            *str = inpt;
            str++;
            (i)++;
        }
        *(uint8_t*)(addr+1) = i;
    } else if ((isfv->eax & 0x0000FF00) == 0x2A00) {
        // sunday, march 4, 1990
        isf->eax &= 0xFFFFFF00;
        isf->eax |= 0; 
        isf->ecx &= 0xFFFF0000;
        isf->ecx |= 1990;
        isf->edx &= 0xFFFF0000;
        isf->edx |= (3 << 8);
        isf->edx |= 4;
    } else if ((isfv->eax & 0x0000FF00) == 0x3000) {
        isf->eax &= 0xFFFF0000;
        isf->eax |= 5;
        isf->eax |= (0) << 8;
    } else if (((isfv->eax) & 0x0000FF00) == 0x0800) {
        isf->eax &= 0xFFFFFF00;
        char inpt;
        asm volatile(
            "start2:\n\t"
            "xor %%eax, %%eax\n\t"
            "mov $0x02, %%eax\n\t"
            "int $0x80\n\t"
            "jc start2\n\t"
            "movb %%al, %0\n\t"
            : "+m" (inpt)
            :
            : "eax"
        );
        isf->eax |= inpt;
    } else if ((isfv->eax & 0x0000FF00) == 0x3D00) {
        uint8_t al = (uint8_t) ((isfv->eax & 0x000000FF));
        printf("%X", al);
        char* fileName = (char*)(isfv->ds * 16 + isfv->edx);
        uint8_t handle;
        if ((al & 3) == 0) {
            // read only
            handle=loadFileDOSInterface(fileName, true, false);
        } else if ((al & 3) == 1) {
            // write only
            handle=loadFileDOSInterface(fileName, false, true);
        } else if ((al & 3) == 2) {
            // read/write
            handle=loadFileDOSInterface(fileName, true, true);
        } else {
            isfv->eflags |= 1;
            isfv->eax &= 0xFFFF0000;
            isfv->eax |= 0xC; // invalid access mode
        }
        if (handle==0) {
            isfv->eflags |= 1;
            isfv->eax &= 0xFFFF0000;
            isfv->eax |= 2; // file not found
            return;
        }
        isfv->eax &= 0xFFFF0000;
        isfv->eax |= handle;
    } else if ((isfv->eax & 0x0000FF00) == 0x4800) {
        isfv->eax = allocMem((uint16_t) isfv->ebx);
    } else if ((isfv->eax & 0x0000FF00) == 0x3E) {
        //close file
        
    } else if ((isfv->eax & 0x0000FF00) == 0x3F) {
        // read file
        uint32_t addr=isfv->ds*16 + isfv->dx;
        uint8_t num = (isfv>ebx & 0x000000FF);
        if (fhlist[num].isActive) {
            if (!fhlist[num].read) {
                isfv->eflags |= 1;
                isfv->eax &= 0xFFFF0000;
                isfv->eax |= 5; // not permssion to read
                return;
            }
        } else {
            isfv->eflags |= 1;
            isfv->eax &= 0xFFFF0000;
            isfv->eax |= 6; //invalid handle
            return;
        }
        uint32_t bytesRead = fl_fread(addr, 1, (isfv->ecx & 0x0000FFFF), fhlist[num].file);
        isfv->eax &= 0xFFFF0000;
        isfv->eax |= bytesRead;
    } else if ((isfv->eax & 0x0000FF00) == 0x40) {
        // write
        uint32_t addr=isfv->ds*16 + isfv->dx;
        uint8_t num = (isfv>ebx & 0x000000FF);
        if (fhlist[num].isActive) {
            if (!fhlist[num].write) {
                isfv->eflags |= 1;
                isfv->eax &= 0xFFFF0000;
                isfv->eax |= 5; // not permssion to read
                return;
            }
        } else {
            isfv->eflags |= 1;
            isfv->eax &= 0xFFFF0000;
            isfv->eax |= 6; //invalid handle
            return;
        }
        uint32_t bytesWritten = fl_fwrite(addr, 1, (isfv->ecx & 0x0000FFFF), fhlist[num].file);
        isfv->eax &= 0xFFFF0000;
        isfv->eax |= bytesWritten;
    } else if ((isfv->eax & 0x0000FF00) == 0x42) {
        // move file pointer
        uint8_t handle = (uint8_t) isfv->ebx;
        uint8_t al = ((uint8_t) isfv->eax & 3);
        uint32_t offset = (((uint32_t)isfv->cx) << 16) + isfv->dx;
        if (!fhlist[handle].isActive) {
            isfv->eflags |= 1;
            isfv->eax &= 0xFFFF0000;
            isfv->eax |= 6; //invalid handle
            return;
        }
        if (al == 0) {
            // relative to beginning of file
            fl_fseek(fhlist[handle].file, offset, SEEK_SET);
        } else if (al == 1) {
            // relative to current file position
            fl_fseek(fhlist[handle].file, offset, SEEK_CUR);
        } else if (al == 2) {
            // relative to end of file
            fl_fseek(fhlist[handle].file, offset, SEEK_END);
        }
        
    } else if ((isfv->eax & 0x0000FF00) == 0x41) {
        // delete file
        char* path = (char*) (isfv->dx * 16 + isfv->edx);
        uint8_t result = fl_remove(path);
        for (uint8_t i = 0; i < 256; i++) {
            if (fhlist[i].isActive && )
        }
        if (result == -1)
    } else if ((isfv->eax & 0x0000FF00) == 0x3C) {
        // create
        
    } else {
        printf("intr num: %X", ((isf->eax & 0x0000FF00) >> 8));
    }

    
}