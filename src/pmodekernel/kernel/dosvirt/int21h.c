#include <kernel/interrupts/intrStructs.h>
#include <kernel/logging.h>
#include <kernel/processes/process.h>
#include <stdio.h>
#include <kernel/dosvirt/dosmem.h>

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
        char* fileName = (char*)(isfv->ds * 16 + isfv->edx);
        if (al == 0) {
            // read only
        } else if (al == 1) {
            // write only
        } else if (al == 2) {
            // read/write
        }
    } else if ((isfv->eax & 0x0000FF00) == 0x4800) {
        isfv->eax = allocMem((uint16_t) isfv->ebx);
    } else {
        printf("intr num: %X", ((isf->eax & 0x0000FF00) >> 8));
    }

    
}