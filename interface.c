#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "emulate.h"

void print_ram(unsigned char*);
void print_regs(unsigned char*, unsigned char);

extern unsigned char registers[16];
extern unsigned char acc;

int main(int argc, char **argv) {

    int pipelined = 0;
    FILE *prog;

    if (argc == 2) {
        prog = fopen(argv[1], "rb");
        if (prog == NULL) {
            fprintf(stderr, "File %s does not exist\n", argv[1]);
            return 1;
        }
    }
    else if (argc == 3) {
        if (strncmp(argv[1], "-p", 3) == 0) {
            pipelined = 1;
        }
        else {
            fprintf(stderr, "%s is not recognised as a parameter\n", argv[1]);
            return 1;
        }
        prog = fopen(argv[2], "rb");
        if (prog == NULL) {
            fprintf(stderr, "File %s does not exist\n", argv[2]);
            return 1;
        }
    }
    else {
        fprintf(stderr, "Invalid parameters: Please use \"4004pem [-p] program\", where program is an assembled 4004 program\n");
    }
    
    char *RAM = emulate(prog);
    print_regs(registers, acc);
    print_ram(RAM);

    fclose(prog);
    return 0;
}

void print_ram(unsigned char *RAM){
    printf("\t\t\tMAIN MEMORY\n");
    int i = 0;
    int j = 16;
    

    for (int reg = 0;reg<4;reg++){
        printf("Register:%d ",reg);

        for(i;i<j;i++){
            printf(" %x",RAM[i]);
        }
        j = j+4;
        printf(" Status: ");
        for(i;i<j;i++){
            printf(" %x",RAM[i]);
        }
        j = j +16;
        printf("\n");

    }
}

void print_regs(unsigned char *regs, unsigned char a) {
        printf("\t\t\tREGISTERS\n");
        int i;
        for (i = 0; i < 16; i+=4) {
            printf("  r%02d - r%02d:", i, i + 3);
            printf(" %x", regs[i]);
            printf(" %x", regs[i + 1]);
            printf(" %x", regs[i + 2]);
            printf(" %x\n", regs[i + 3]);
        }
        printf("Accumulator: %x\n", a);
    }