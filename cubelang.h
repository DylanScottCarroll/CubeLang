#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>

struct volptr {
    int x;
    int y;
    int z;
};
typedef struct volptr volptr; 

int main(int argc, char** argv);

int16_t* parse(FILE* f, int* N);

int16_t to_hex(char* str);

int16_t parse_asm(char* str);

int16_t dir_to_int(char dir);

int16_t inst_name_to_int(char* inst);

int ptr_to_index(volptr ptr, int N, int N2);

void execute(int16_t* mem, int N);

void move_ptr(volptr* ptr, int16_t dir, int N);
