#include <stdio.h>
#include "emulate.h"
#include <pthread.h>
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


int pipelined;

// Pipeline Buffers
unsigned char registers_buff[16] = {0};
unsigned char opcode_buff;
unsigned char reg_select_buff;
unsigned char immediate1_buff;
unsigned char immediate2_buff;
unsigned char subgroup_buff;
short PC_buff = 0;
unsigned char RAM_reg_select_buff = 0;
short jump_addr_buff = 0;
unsigned char RAM_slot_buff = 0;

//Threads for Pipeline
int branch_hazard = 0;
pthread_t thread_fetch_decode;
pthread_t thread_execute_wb;

int emulate(int p) {
    pipelined = p;
    int cycles = 0;
    PC = 0;
    if (pipelined){
        pthread_create(&thread_fetch_decode,NULL,fetch_decode,NULL);
        pthread_join(thread_fetch_decode,NULL);
    }
    for(;;){
        if(pipelined){
            buffer_transfer();
            int e1 = pthread_create(&thread_fetch_decode,NULL,fetch_decode,NULL);
            int e2 = pthread_create(&thread_execute_wb,NULL,execute_wb,NULL);
            e2 = pthread_join(thread_fetch_decode,NULL);
            e1 = pthread_join(thread_execute_wb,NULL);
            reg_buffer_transfer();

    //      Check for branch Hazard
            if(branch_hazard == 1){
                branch_hazard =0;
                PC = PC_buff;
                pthread_create(&thread_fetch_decode,NULL,fetch_decode,NULL);
                pthread_join(thread_fetch_decode,NULL);
            }
        }
        else{
            fetch_decode();
            buffer_transfer();
            execute_wb();
            reg_buffer_transfer();
        }
        cycles++;
        if (term) return cycles;
    }
}

void reg_buffer_transfer() {
    int i;
    for(i = 0; i < 16 ; i++) {
        registers[i] = registers_buff[i];
    }
}

void buffer_transfer() {
    opcode_buff = opcode;
    reg_select_buff = reg_select;
    immediate1_buff = immediate1;
    immediate2_buff = immediate2;
    subgroup_buff = subgroup;
    RAM_slot_buff = RAM_slot;
    jump_addr_buff = jump_addr;
    RAM_reg_select_buff = RAM_reg_select;
}

void *fetch_decode(){
    fetch();
    opcode = decode();
    if(pipelined) pthread_exit(NULL);
}

void *execute_wb(){
    execute();
    if(pipelined) pthread_exit(NULL);

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
        case SUB:
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
        case JUN:
            jump_addr = OPA << 8;
            fetch();
            jump_addr |= ins;
            break;
        case INC:
            reg_select = OPA;
            break;
        default:
            break;
    }

    return OPR;
}

void execute() {
        unsigned char temp;
        switch (opcode_buff) {
            case LDM:
                acc = immediate1_buff;
                break;
            case FIM_SRC:
                switch (subgroup_buff) {
                    case FIM:
                        registers_buff[reg_select_buff] = immediate1_buff;
                        registers_buff[reg_select_buff+ 1] = immediate2_buff;
                        break;
                    case SRC:
                        break;
                }
                break;
            case LD:
                acc = registers_buff[reg_select_buff];
                break;
            case ADD:
                acc += registers_buff[reg_select_buff] + carry;
                carry = 0;
                if (acc > 15) {
                    acc = acc & 0x0f;
                    carry = 1;
                }
                break;
            case SUB:
                acc -= registers_buff[reg_select_buff] + carry;
                carry = 1;
                if (acc > 15) {
                    carry = 0;
                }
                acc = acc & 0x0f;
                break;
            case XCH:
                temp = acc;
                acc = registers_buff[reg_select_buff];
                registers_buff[reg_select_buff] = temp;
                break;
            case NOP:
                break;
            case FIN_JIN:
                switch (subgroup_buff) {
                    case FIN:
                        temp = registers_buff[0] << 4;
                        temp = temp | registers_buff[1];
                        registers_buff[reg_select_buff] = RAM[temp] >> 4;
                        registers_buff[reg_select_buff + 1] = RAM[temp] & 0x0f;
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
                    case IAC:
                        acc = (acc + 1) % 16;
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
                registers_buff[reg_select_buff] = (registers_buff[reg_select_buff] + 1) % 16;
                if (registers_buff[reg_select_buff]){    
                    PC_buff = jump_addr_buff;
                    branch_hazard = 1;
                    if(!pipelined) PC = jump_addr_buff;
                }
                break;
            case JUN:
                PC_buff = jump_addr_buff;
                branch_hazard = 1;
                if (!pipelined) PC = jump_addr_buff;
                break;
            case INC:
                registers_buff[reg_select_buff] = (registers_buff[reg_select_buff] + 1) % 16;
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
