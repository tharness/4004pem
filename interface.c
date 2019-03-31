#include <stdio.h>
#include <string.h>
#include "emulate.h"

void print_ram();
void print_regs();

extern unsigned char RAM[RAM_SIZE];
extern unsigned char registers[16];
extern unsigned char acc;
FILE *ROM;

int main(int argc, char **argv) {

    int pipelined = 0;
    
    if (argc == 2) {
        ROM = fopen(argv[1], "rb");
        if (ROM == NULL) {
            fprintf(stderr, "File %s does not exist\n", argv[1]);
            return 1;
        }
    }
    else if (argc == 3) {
        if (strncmp(argv[1], "-p", 3) == 0) {
            printf("Pipelined\n");
            pipelined = 1;
        }
        else {
            fprintf(stderr, "%s is not recognised as a parameter\n", argv[1]);
            return 1;
        }
        ROM = fopen(argv[2], "rb");
        if (ROM == NULL) {
            fprintf(stderr, "File %s does not exist\n", argv[2]);
            return 1;
        }
    }
    else {
        fprintf(stderr, "Invalid parameters: Please use \"4004pem [-p] program\", where program is an assembled 4004 program\n");
    }
    
    int cycles = emulate(pipelined);
    printf("Number of cycles: %d\n", cycles);
    print_regs();
    print_ram();

    fclose(ROM);
    return 0;
}

void print_ram(){
    printf("\t\t\tMAIN MEMORY\n");
    int i = 0;
    int j = 16;
    int reg;

    for (reg = 0; reg < 4; reg++){
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

void print_regs() {
        printf("\t\t\tREGISTERS\n");
        int i;
        for (i = 0; i < 16; i+=4) {
            printf(" r%02d to r%02d:", i, i + 3);
            printf(" %x", registers[i]);
            printf(" %x", registers[i + 1]);
            printf(" %x", registers[i + 2]);
            printf(" %x\n", registers[i + 3]);
        }
        printf("Accumulator: %x\n", acc);
    }