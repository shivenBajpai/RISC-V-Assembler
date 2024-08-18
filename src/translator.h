#ifndef TRANSLATOR_H
#define TRANSLATOR_H
#define R_TYPE 0 

#include "index.h"
#include "hashmap.h"

typedef struct instruction_info {
    const char* name;
    int constant;
    int handler_type;
} instruction_info;

long R_type_parser(FILE** args, label_index* labels, int* line_number);
int search_register(char* name);
const instruction_info* search_instruction(char* name);
#endif