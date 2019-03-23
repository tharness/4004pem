/*
* Erik Yu
* V00865663
* This is for writing the hex values of Demo1 to a binary file named Demo1_hex.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
   char program[21] = {0xF0, 0xFD, 0x20, 0x00, 0x24, 0xC0, 0xDF, 0x21,
    0xE0, 0x71, 0x07, 0xE4, 0xE5, 0xE6, 0xE7, 0x60,
    0x74, 0x07, 0xE1, 0xE2, 0x00,
    };
    FILE *out = fopen("Demo1_hex", "wb");
    fwrite(&program, 21, 1, out);
    fclose(out);

}