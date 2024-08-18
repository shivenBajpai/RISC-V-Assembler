#include <stdio.h>
#include "index.h"
#include "hashmap.h"

typedef struct instruction_handler {
    int constant;
    int (*handler)(FILE**, hashmap*, label_index*, int*);
} instruction_handler;

int R_type_parser(FILE** args, hashmap* registers, label_index* labels, int* line_number) {
    FILE* fp = *args;
    int i = 0;
    int r = 0;
    int result = 0;
    char* reg_name[8];
    char c;
    const int register_offsets[] = {7,15,20};

    while ((c = fgetc(fp)) != EOF) {
        switch (c){
            case ' ':
            case '\t':
                break;
            case ',':
                if (r==3) {
                    printf("Error on line %d, More operands than expected: %s\n", line_number, reg_name);
                    return -1;
                }
                int register_addr = hm_get(registers, reg_name);
                register_addr = register_addr << register_offsets[r++];
                i = 0;

                break;

            case '\n':
                goto exit;

            default:
                if (i==8) {
                    printf("Error on line %d, Illegal operand: %s\n", line_number, reg_name);
                    return -1;
                }
                reg_name[i++] = c;
        }
    }

exit:
    if (r==3) {
        printf("Error on line %d, More operands than expected: %s\n", line_number, reg_name);
        return -1;
    }
    int register_addr = hm_get(registers, reg_name);
    register_addr = register_addr << register_offsets[r++];
    i = 0;

    return result;
}

const instruction_handler ADD = {0x0110011,                      &R_type_parser};
const instruction_handler SUB = {0x0110011+(0x20<<25),           &R_type_parser};
const instruction_handler xOR = {0x0110011+(0x4<<12),            &R_type_parser};
const instruction_handler OR =  {0x0110011+(0x6<<12),            &R_type_parser};
const instruction_handler AND = {0x0110011+(0x7<<12),            &R_type_parser};
const instruction_handler SLL = {0x0110011+(0x1<<12),            &R_type_parser};
const instruction_handler SRL = {0x0110011+(0x5<<12),            &R_type_parser};
const instruction_handler SRA = {0x0110011+(0x5<<12)+(0x20<<25), &R_type_parser};
const instruction_handler SLT = {0x0110011+(0x2<<12),            &R_type_parser};
const instruction_handler SLTU ={0x0110011+(0x3<<12),            &R_type_parser};

// Converts one instruction into Binary
int translate_instruction(char* instruction, label_index* index, int line_number) {
    
    char name[8]; // Sufficient for any valid instruction/pseudo instruction in Base class
    int i;
    for (i = 0; i<8; i++) {
        if (instruction[i] == ' ' || instruction[i] == '\n') {
            name[i] = '\0';
            break;
        }
        name[i] = instruction[i];
    }

    if (i == 8) {
        printf("Error on line %d\nInvalid Instruction: %s\n                     ^^^^^^^^\n", line_number, instruction);
        return 0xFFFFFFFF;
    }
    
    return 0xAAAAAAAA;
}

const hashmap* build_register_table(void) {
    const hashmap* table = new_hashmap(256);

    hm_set(table, "x0", (void*) 0);
    hm_set(table, "x1", (void*) 1);
    hm_set(table, "x2", (void*) 2);
    hm_set(table, "x3", (void*) 3);
    hm_set(table, "x4", (void*) 4);
    hm_set(table, "x5", (void*) 5);
    hm_set(table, "x6", (void*) 6);
    hm_set(table, "x7", (void*) 7);
    hm_set(table, "x8", (void*) 8);
    hm_set(table, "x9", (void*) 9);
    hm_set(table, "x10", (void*) 10);
    hm_set(table, "x11", (void*) 11);
    hm_set(table, "x12", (void*) 12);
    hm_set(table, "x13", (void*) 13);
    hm_set(table, "x14", (void*) 14);
    hm_set(table, "x15", (void*) 15);
    hm_set(table, "x16", (void*) 16);
    hm_set(table, "x17", (void*) 17);
    hm_set(table, "x18", (void*) 18);
    hm_set(table, "x19", (void*) 19);
    hm_set(table, "x20", (void*) 20);
    hm_set(table, "x21", (void*) 21);
    hm_set(table, "x22", (void*) 22);
    hm_set(table, "x23", (void*) 23);
    hm_set(table, "x24", (void*) 24);
    hm_set(table, "x25", (void*) 25);
    hm_set(table, "x26", 26);
    hm_set(table, "x27", 27);
    hm_set(table, "x28", 28);
    hm_set(table, "x29", 29);
    hm_set(table, "x30", 30);
    hm_set(table, "x31", 31);
    
    hm_set(table, "zero", 0);
    hm_set(table, "ra", 1);
    hm_set(table, "sp", 2);
    hm_set(table, "gp", 3);
    hm_set(table, "tp", 4);
    hm_set(table, "t0", 5);
    hm_set(table, "t1", 6);
    hm_set(table, "t2", 7);
    hm_set(table, "s0", 8);
    hm_set(table, "fp", 8);
    hm_set(table, "s1", 9);
    hm_set(table, "a0", (void*) 10);
    hm_set(table, "a1", (void*) 11);
    hm_set(table, "a2", (void*) 12);
    hm_set(table, "a3", (void*) 13);
    hm_set(table, "a4", (void*) 14);
    hm_set(table, "a5", (void*) 15);
    hm_set(table, "a6", (void*) 16);
    hm_set(table, "a7", (void*) 17);
    hm_set(table, "s2", (void*) 18);
    hm_set(table, "s3", (void*) 19);
    hm_set(table, "s4", (void*) 20);
    hm_set(table, "s5", (void*) 21);
    hm_set(table, "s6", (void*) 22);
    hm_set(table, "s7", (void*) 23);
    hm_set(table, "s8", (void*) 24);
    hm_set(table, "s9", (void*) 25);
    hm_set(table, "s10", 26);
    hm_set(table, "s11", 27);
    hm_set(table, "t3", 28);
    hm_set(table, "t4", 29);
    hm_set(table, "t5", 30);
    hm_set(table, "t6", 31);

    return table;      
}

const hashmap* build_instruction_table(void) {
    const hashmap* table = new_hashmap(32);

    hm_set(table, "add", &ADD);
    hm_set(table, "sub", &SUB);
    hm_set(table, "or", &xOR);
    hm_set(table, "xor", &OR);
    hm_set(table, "and", &AND);
    hm_set(table, "sll", &SLL);
    hm_set(table, "srl", &SRL);
    hm_set(table, "sra", &SRA);
    hm_set(table, "slt", &SLT);
    hm_set(table, "sltu", &SLTU);

    return table;
}