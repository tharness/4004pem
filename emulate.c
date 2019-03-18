#include <stdio.h>
#include "emulate.h"

#define RAM_SIZE 1280

FILE *ROM;
char RAM[RAM_SIZE] = {0};
char registers[16] = {0};
char acc = 0;
char carry = 0;
short PC = 0;
char ins;

char data_addr = 0;
char reg_select;
char immediate;


void emulate(FILE *prog) {
    ROM = prog;
    char opcode;
    int temp = 0;
    PC = 0;
    for (;;) {
        if (temp == 5) return;
        fetch();
        opcode = decode();
        execute(opcode);
        printf("%x\n", ins);
        PC++;
        temp++;
    }
}

void fetch() {
    fseek(ROM, PC, 0);
    fread(&ins, 1, 1, ROM);
}

char decode() {
    //extract top four bits of instruction
    char OPR = (ins & 0xf0) >> 4;
    //bottom four
    char OPA = (ins & 0x0f);
    switch (OPR) {
        case LDM:
            immediate = OPA;
            break;
        default:
            break;
    }

    return OPR;
}

void execute(char opcode) {
    switch (opcode) {
        case LDM:
            acc = immediate;
        default:
            break;
    }
}