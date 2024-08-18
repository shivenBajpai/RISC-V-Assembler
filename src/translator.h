#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#include "index.h"
#include "hashmap.h"

typedef struct instruction_handler {
    int constant;
    int (*handler)(FILE**, hashmap*, label_index*, int*);
} instruction_handler;

int translate_instruction(char* instruction, label_index* index, int line_number);
hashmap* build_register_table(void);
hashmap* build_instruction_table(void);
#endif