#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#include "index.h"
#include "hashmap.h"

typedef struct instruction_info {
    const char* name;
    int constant;
    int handler_type;
} instruction_info;

typedef enum instruction_type {
    R_TYPE,
    I1_TYPE,
    I2_TYPE,
    S_TYPE,
    B_TYPE,
    U_TYPE,
    J_TYPE,
    I3_TYPE,
    I4_TYPE
} instruction_type;

typedef enum argument_type {
    IMMEDIATE,
    OFFSET,
    REGISTER,
    ADDRESS,
} argument_type;

long R_type_parser(FILE** args_raw, label_index* labels, int* line_number);
long I1_type_parser(FILE** args_raw, label_index* labels, int* line_number);
long I2_type_parser(FILE** args_raw, label_index* labels, int* line_number);
long S_type_parser(FILE** args_raw, label_index* labels, int* line_number);
long B_type_parser(FILE** args_raw, label_index* labels, int* line_number);
long U_type_parser(FILE** args_raw, label_index* labels, int* line_number);
long J_type_parser(FILE** args_raw, label_index* labels, int* line_number);
long I3_type_parser(FILE** args_raw, label_index* labels, int* line_number);

int search_register(char* name);
const instruction_info* search_instruction(char* name);
#endif