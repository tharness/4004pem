#include <stdio.h>
#include "emulate.h"

FILE *ROM;
unsigned char term = 0;

unsigned char RAM[RAM_SIZE] = {0};
unsigned char RAM_reg_select = 0;
unsigned char RAM_slot = 0;

unsigned char registers[16] = {0};
unsigned char acc = 0;
unsigned char carry = 0;
short PC = 0;
unsigned char ins;

//values decoded from instruction
unsigned char data_addr = 0;
unsigned char reg_select;
unsigned char immediate1;
unsigned char immediate2;
unsigned char subgroup;
short jump_addr = 0;


char *emulate(FILE *prog) {
    ROM = prog;
    unsigned char opcode;
    PC = 0;
    for (;;) {
        fetch();
        opcode = decode();
        execute(opcode);
        if (term) return RAM;
    }
}

void fetch() {
    fseek(ROM, PC, 0);
    fread(&ins, 1, 1, ROM);
    PC++;
}

unsigned char decode() {
    //extract top four bits of instruction
    unsigned char OPR = (ins & 0xf0) >> 4;
    //bottom four
    unsigned char OPA = (ins & 0x0f);
    unsigned char temp;
    switch (OPR) {
        case LDM:
            immediate1 = OPA;
            break;
        case FIM_SRC:
            subgroup = OPA & 0x1;
            switch (subgroup) {
                //two word instruction
                case FIM:
                    reg_select = (OPA >> 1) * 2;
                    fetch();
                    immediate1 = ins >> 4;
                    immediate2 = ins & 0x0f;
                    break;
                case SRC:
                    //extract the RAM register specified in the first of the register pair
                    RAM_reg_select = registers[(OPA >> 1) * 2];
                    //extract the nybble of the RAM register specified in the second of the register pair
                    RAM_slot = registers[(OPA >> 1) * 2 + 1];
                    break;
            }
            break;
        case LD:
            reg_select = OPA;
            break;
        case ADD:
            reg_select = OPA;
            break;
        case XCH:
            reg_select = OPA;
            break;
        case NOP:
            break;
        case FIN_JIN:
            subgroup = OPA & 0x1;
            switch (subgroup) {
                case FIN:
                    reg_select = (OPA >> 1) * 2;
                    break;
            }
            break;
        case ACC_GROUP:
            subgroup = OPA;
            break;
        case RW_MEM_STATUS:
            subgroup = OPA;
            break;
        case ISZ:
            reg_select = OPA;
            fetch();
            jump_addr = ins;
            break;
        case INC:
            reg_select = OPA;
            break;
        default:
            break;
    }

    return OPR;
}

void execute(unsigned char opcode) {
    unsigned char temp;
    switch (opcode) {
        case LDM:
            acc = immediate1;
            break;
        case FIM_SRC:
            switch (subgroup) {
                case FIM:
                    registers[reg_select] = immediate1;
                    registers[reg_select + 1] = immediate2;
                    break;
                case SRC:
                    break;
            }
            break;
        case LD:
            acc = registers[reg_select];
            break;
        case ADD:
            acc += registers[reg_select] + carry;
            carry = 0;
            if (acc > 15) {
                acc = acc & 0x0f;
                carry = 1;
            }
            break;
        case XCH:
            temp = acc;
            acc = registers[reg_select];
            registers[reg_select] = temp;
            break;
        case NOP:
            break;
        case FIN_JIN:
            switch (subgroup) {
                case FIN:
                    temp = registers[0] << 4;
                    temp = temp | registers[1];
                    registers[reg_select] = RAM[temp] >> 4;
                    registers[reg_select + 1] = RAM[temp] & 0x0f;
                    break;
            }
            break;
        case ACC_GROUP:
            switch (subgroup) {
                case CLB:
                    acc = 0;
                    carry = 0;
                    break;
                case KBP:
                    term = 1;
                    break;
            }
            break;
        case RW_MEM_STATUS:
            switch (subgroup) {
                case WRM:
                    write_RAM();
                    break;
                case WR0:
                    write_RAM_status(0);
                    break;
                case WR1:
                    write_RAM_status(1);
                    break;
                case WR2:
                    write_RAM_status(2);
                    break;
                case WR3:
                    write_RAM_status(3);
                    break;
            }
            break;
        case ISZ:
            registers[reg_select] = (registers[reg_select] + 1) % 16;
            if (registers[reg_select]) PC = jump_addr;
            break;
        case INC:
            registers[reg_select] = (registers[reg_select] + 1) % 16;
            break;
        default:
            break;
    }
}

unsigned char read_RAM() {
    return RAM[RAM_reg_select * 20 + RAM_slot];
}

unsigned char read_RAM_status(unsigned char s) {
    return RAM[RAM_reg_select * 20 + 16 + s];
}

void write_RAM() {
    RAM[RAM_reg_select * 20 + RAM_slot] = acc;
}

void write_RAM_status(unsigned char s) {
    RAM[RAM_reg_select * 20 + 16 + s] = acc;
}