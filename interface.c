#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "emulate.h"
extern unsigned char RAM[RAM_SIZE];
void print_ram();

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
    
    print_ram();
    emulate(prog);
    print_ram();

    fclose(prog);
    return 0;
}

void print_ram(){
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



