#include <stdio.h>
#include "emulate.h"
#include <pthread.h>
#include <string.h>
#include <stdlib.h>

extern FILE *ROM;
unsigned char term = 0;

unsigned char RAM[RAM_SIZE] = {0};
unsigned char RAM_reg_select = 0;
unsigned char RAM_slot = 0;

unsigned char registers[16] = {0};
unsigned char acc = 0;
unsigned char carry = 0;
short PC = 0;
unsigned char ins;
unsigned char opcode;  

//values decoded from instruction
unsigned char data_addr = 0;
unsigned char reg_select;
unsigned char immediate1;
unsigned char immediate2;
unsigned char subgroup;
short jump_addr = 0;


int fetch_decode_count = 0;
int execute_wb_count = 0;

// Buffers for pipeline
unsigned char opcode_buff;
unsigned char reg_select_buff;
unsigned char immediate1_buff;
unsigned char immediate2_buff;
unsigned char subgroup_buff;
short PC_buff = 0;
unsigned char RAM_reg_select_buff = 0;
short jump_addr_buff = 0;
unsigned char RAM_slot_buff = 0;



//Threads
pthread_t thread_fetch_decode;
pthread_t thread_execute_wb;


int emulate(int pipelined) {
    int cycles = 0;
    PC = 0;
    
    pthread_create(&thread_fetch_decode,NULL,fetch_decode,NULL);
    pthread_join(thread_fetch_decode,NULL);
    copy();

    for(;;){
      
        int e1 = pthread_create(&thread_fetch_decode,NULL,fetch_decode,NULL);
        int e2 = pthread_create(&thread_execute_wb,NULL,execute_wb,NULL);
        if(e1 | e2){
            printf("Error\n");
            exit(EXIT_FAILURE);

        }
    // //     // join threads
        e2 = pthread_join(thread_fetch_decode,NULL);
        e1 = pthread_join(thread_execute_wb,NULL);
        if(e1 | e2){
            printf("Error\n");
            exit(EXIT_FAILURE);

        }
        copy();

        cycles++;
        if (term) return cycles;
    }
}

void print(){

    printf("Test copy--------\n");
    printf("Opcode %x  %x\n",opcode,opcode_buff);
    printf("reg_select %x  %x\n",reg_select,reg_select_buff);
    printf("immidiate1 %x  %x\n",immediate1,immediate1_buff);
    printf("immidiate2 %x  %x\n",immediate2,immediate2_buff);
    printf("pc %x  %x\n",PC,PC_buff);
    printf("subgroup %x  %x\n",subgroup,subgroup_buff);
    printf("RAM_slot_buff %x  %x\n",RAM_slot,RAM_slot_buff);
    printf("jump_addr %x  %x\n",jump_addr,jump_addr_buff);
    printf("RAM_reg_select %x  %x\n",RAM_reg_select,RAM_reg_select_buff);
}
void copy(){
    
    opcode_buff = opcode;
    reg_select_buff = reg_select;
    immediate1_buff = immediate1;
    immediate2_buff = immediate2;
    PC_buff = PC;
    subgroup_buff = subgroup;
    RAM_slot_buff = RAM_slot;
    jump_addr_buff = jump_addr;
    RAM_reg_select_buff = RAM_reg_select;

}


void *fetch_decode(){

        fetch();
        opcode = decode();
        fetch_decode_count++;
        pthread_exit(NULL);
}

void *execute_wb(){

        execute();
        execute_wb_count++;
        pthread_exit(NULL);

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


/*In both Fetch/Decode and Execute/Wb
1)opcode
2,3)immediate1,immediate2
4)subgroup
5)reg_select
6)PC
7,8)RAM_reg_select,RAM_slot
9) jump_addr
*/
void execute() {
        unsigned char temp;
        switch (opcode_buff) {
            case LDM:
                acc = immediate1_buff;
                break;
            case FIM_SRC:
                switch (subgroup_buff) {
                    case FIM:
                        registers[reg_select_buff] = immediate1_buff;
                        registers[reg_select_buff+ 1] = immediate2_buff;
                        break;
                    case SRC:
                        break;
                }
                break;
            case LD:
                acc = registers[reg_select_buff];
                break;
            case ADD:
                acc += registers[reg_select_buff] + carry;
                carry = 0;
                if (acc > 15) {
                    acc = acc & 0x0f;
                    carry = 1;
                }
                break;
            case XCH:
                temp = acc;
                acc = registers[reg_select_buff];
                registers[reg_select_buff] = temp;
                break;
            case NOP:
                break;
            case FIN_JIN:
                switch (subgroup_buff) {
                    case FIN:
                        temp = registers[0] << 4;
                        temp = temp | registers[1];
                        registers[reg_select_buff] = RAM[temp] >> 4;
                        registers[reg_select_buff + 1] = RAM[temp] & 0x0f;
                        break;
                }
                break;
            case ACC_GROUP:
                switch (subgroup_buff) {
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
                switch (subgroup_buff) {
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
                registers[reg_select_buff] = (registers[reg_select_buff] + 1) % 16;
                if (registers[reg_select_buff]) PC = jump_addr_buff;
                break;
            case INC:
                registers[reg_select_buff] = (registers[reg_select_buff] + 1) % 16;
                break;
            default:
                break;
        }
  
}

unsigned char read_RAM() {
    return RAM[RAM_reg_select_buff * 20 + RAM_slot_buff];
}

unsigned char read_RAM_status(unsigned char s) {
    return RAM[RAM_reg_select_buff * 20 + 16 + s];
}

void write_RAM() {
    RAM[RAM_reg_select_buff * 20 + RAM_slot_buff] = acc;
}

void write_RAM_status(unsigned char s) {
    RAM[RAM_reg_select_buff * 20 + 16 + s] = acc;
}
