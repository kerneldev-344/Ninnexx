// a command prompt in kernel mode (i know peak cybersecurity!!) cause i dont wanna implement all the bs user space interrupts

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <kernel/logging.h>
#include <kernel/time/time.h>
#include <kernel.h>
#include <kernel/fskernelapi/loadfile.h>
#include <kernel/processes/process.h>
#include <kernel/memory.h>
#include <kernel/memory/pmm.h>
#include <kernel/dosvirt/loadcom.h>
#include <kernel/time/time.h>


int getCmdAndParse(char* cmd);

extern struct Process* current;

void cmd() {
    char* cmd = (char*) calloc(128, sizeof(char));
    printf("Welcome to Ninnexx!\n");
    while (1) {
        printf("\nCommand Parser: ");
        uint32_t len = getCmdAndParse(cmd);
        if (strncmp(cmd, "clear", 5) == 0) {
            kclear();
        } else if (strncmp(cmd, "lc", 2) == 0) {
            if (len < 4) {
                printf("\nPlease list a file!\n");
                continue;
            }
            printf("\nList the arguments (no preceding space needed): ");
            char* arg = calloc(128, 1);
            
            uint32_t len = getCmdAndParse((char*)((uint32_t) arg + 1));
            if (len != 0) {
                len++;
                arg[0] = ' ';
            }

            uint32_t result = loadCom((char*)(((uint32_t)cmd)+3), (uint8_t) len, arg);
            if (result == 1) {
                printf("\nFile not found!\n");
                continue;
            } else if (result == 2) {
                printf("\nMemory allocation error!\n");
                continue;
            } else if (result == 3) {
                printf("\nFailed to read file!\n");
                continue;
            } else if (result == 4) {
                printf("\nFailed to start process!\n");
                continue;
            }
            free(arg);
        } else if (strncmp(cmd, "ep", 2) == 0) {
            if (len < 4) {
                printf("\nPlease enter the UUID of the program you would like to end\n");
                continue;
            } else {
                switch (cmd[3]) {
                    case '0':
                    case '1':
                        printf("\nNeither the Swapper nor Command Prompt can be ended! Sorry :D\n");
                        break;
                    case '2':
                        deleteProcess(2);
                        printf("\ndeleted proc 2\n");
                        break;
                    case '3':
                        deleteProcess(3);
                        break;
                    case '4':
                        deleteProcess(4);
                        break;
                    case '5':
                        deleteProcess(5);
                        break;
                    case '6':
                        deleteProcess(6);
                        break;
                    case '7':
                        deleteProcess(7);
                        break;
                    case '8':
                        deleteProcess(8);
                        break;
                    case '9':
                        deleteProcess(9);
                        break;
                }
            }
        } else if (strncmp("upt", cmd, 3) == 0) {
            printf("\nUptime: %X seconds\n", ticks/freq);
        } else {
            printf("\nCommand not found!\n");
        }
        memset(cmd, '\0', 100);
    }

    while (1) {}
}

int getCmdAndParse(char* cmd) {
    uint32_t idx = 0;
    while (idx < 127) {
        asm volatile (
            "startOfLoop: \n\t"
            "xor %%eax, %%eax\n\t"
            "xor %%ebx, %%ebx\n\t"
            "mov $0x02, %%eax\n\t"
            "int $0x80\n\t"
            "jc startOfLoop\n\t"
            "movb %%al, %0\n\t"
            : "+m" (cmd[idx])
            : 
            : "eax", "ebx", "memory"
        );
        if (cmd[idx] == '\r') {
            cmd[idx] = '\0';
            // idx++;
            break;
        }
        if (cmd[idx] == '\b') {
            idx -= 1;
            cmd[idx] = '\0';
            kdelprevc();
            continue;
            // break;
        }
        kputc(cmd[idx]);
        idx++;
    } 
    return idx;
}