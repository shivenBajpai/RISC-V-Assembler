#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "translator.h"
#include <stdlib.h>
#include "index.h"
#include "hashmap.h"

typedef struct alias {
    const char* name;
    int value;
} alias;

const instruction_info instructions[] = {
    "add",      0b0110011,                      R_TYPE,
    "sub",      0b0110011+(0x20<<25),           R_TYPE,
    "xor",      0b0110011+(0x4<<12),            R_TYPE,
    "or",       0b0110011+(0x6<<12),            R_TYPE,
    "and",      0b0110011+(0x7<<12),            R_TYPE,
    "sll",      0b0110011+(0x1<<12),            R_TYPE,
    "srl",      0b0110011+(0x5<<12),            R_TYPE,
    "sra",      0b0110011+(0x5<<12)+(0x20<<25), R_TYPE,
    "slt",      0b0110011+(0x2<<12),            R_TYPE,
    "sltu",     0b0110011+(0x3<<12),            R_TYPE,

    "addi",     0b0010011+(0x0<<12),            I1_TYPE,
    "xori",     0b0010011+(0x4<<12),            I1_TYPE,
    "ori",      0b0010011+(0x6<<12),            I1_TYPE,
    "andi",     0b0010011+(0x7<<12),            I1_TYPE,
    "slli",     0b0010011+(0x1<<12)+(0x00<<26), I1_TYPE,
    "srli",     0b0010011+(0x5<<12)+(0x00<<26), I1_TYPE,
    "srai",     0b0010011+(0x5<<12)+(0x10<<26), I1_TYPE,
    "slti",     0b0010011+(0x2<<12),            I1_TYPE,
    "sltiu",    0b0010011+(0x3<<12),            I1_TYPE,

    "lb",       0b0000011+(0x0<<12),            I2_TYPE,
    "lh",       0b0000011+(0x1<<12),            I2_TYPE,
    "lw",       0b0000011+(0x2<<12),            I2_TYPE,
    "ld",       0b0000011+(0x3<<12),            I2_TYPE,
    "lbu",      0b0000011+(0x4<<12),            I2_TYPE,
    "lhu",      0b0000011+(0x5<<12),            I2_TYPE,
    "lwu",      0b0000011+(0x6<<12),            I2_TYPE,

    "sb",       0b0100011+(0x0<<12),            S_TYPE,
    "sh",       0b0100011+(0x1<<12),            S_TYPE,
    "sw",       0b0100011+(0x2<<12),            S_TYPE,
    "sd",       0b0100011+(0x3<<12),            S_TYPE,

    "beq",      0b1100011+(0x0<<12),            B_TYPE,      
    "bne",      0b1100011+(0x1<<12),            B_TYPE,      
    "blt",      0b1100011+(0x4<<12),            B_TYPE,      
    "bge",      0b1100011+(0x5<<12),            B_TYPE,      
    "bltu",     0b1100011+(0x6<<12),            B_TYPE,         
    "bgeu",     0b1100011+(0x7<<12),            B_TYPE,     

    "jal",      0b1101111,                      J_TYPE,
    "jalr",     0b1100111,                      I3_TYPE,
    "lui",      0b0110111,                      U_TYPE,
    "auipc",    0b0010111,                      U_TYPE,   
    "ecall",    0b1110011,                      I4_TYPE,   
    "ebreak",   0b1110011+(0X1<<20),            I4_TYPE,       
}; 

const alias registers[] = {
    "x0", 0,
    "x1", 1,
    "x2", 2,
    "x3", 3,
    "x4", 4,
    "x5", 5,
    "x6", 6,
    "x7", 7,
    "x8", 8,
    "x9", 9,
    "x10", 10,
    "x11", 11,
    "x12", 12,
    "x13", 13,
    "x14", 14,
    "x15", 15,
    "x16", 16,
    "x17", 17,
    "x18", 18,
    "x19", 19,
    "x20", 20,
    "x21", 21,
    "x22", 22,
    "x23", 23,
    "x24", 24,
    "x25", 25,
    "x26", 26,
    "x27", 27,
    "x28", 28,
    "x29", 29,
    "x30", 30,
    "x31", 31,
    "zero", 0,
    "ra", 1,
    "sp", 2,
    "gp", 3,
    "tp", 4,
    "t0", 5,
    "t1", 6,
    "t2", 7,
    "s0", 8,
    "fp", 8,
    "s1", 9,
    "a0", 10,
    "a1", 11,
    "a2", 12,
    "a3", 13,
    "a4", 14,
    "a5", 15,
    "a6", 16,
    "a7", 17,
    "s2", 18,
    "s3", 19,
    "s4", 20,
    "s5", 21,
    "s6", 22,
    "s7", 23,
    "s8", 24,
    "s9", 25,
    "s10", 26,
    "s11", 27,
    "t3", 28,
    "t4", 29,
    "t5", 30,
    "t6", 31,
};

const char* argument_type_names[] = {
    "Immediate Value",
    "Offset/Flag",
    "Register",
    "Memory Address"
}; 

int search_register(char* name) {
    for (int i = 0; i<65; i++) {
        if (strcmp(name, registers[i].name) == 0) {
            return registers[i].value;
        }
    }

    return -1;
}

const instruction_info* search_instruction(char* name) {
    for (int i = 0; i<42; i++) {
        if (strcmp(name, instructions[i].name) == 0) {
            return &instructions[i];
        }
    }

    return NULL;
}

int* args_parser(FILE** fpp, label_index* labels, int n, argument_type* types, int* line_number) {
    FILE* fp = *fpp;
    int i = 0;
    int r = 0;
    char* args = malloc(r*128*sizeof(char));
    if (!args) {
        printf("Out of memory!");
        return NULL;
    }
    char c;

    while ((c = fgetc(fp)) != EOF) {
        switch (c){
            case ' ':
            case '\t':
                break;

            case ',':
            case'(':
                if (r==(n-1)) {
                    printf("Error on line %d, Expected 3 operands, Found more than 3\n", *line_number);
                    free(args);
                    return NULL;
                }
                args[r*128 + i] = '\0';
                r++;
                i = 0;
                break;

            case ')':
                fp++;
            case '\n':
                goto exit;

            default:
                if (i==7) {
                    printf("Error on line %d, Illegal operand: %s\n", *line_number, args + r*128);
                    free(args);
                    return NULL;
                }
                args[r*128 + i++] = c;
        }
    }

    exit:
    if (r<(n-1)) {
        printf("Error on line %d, Expected 3 operands, Less operands than expected\n", *line_number);
        free(args);
        return NULL;
    }
    args[r*128 + i] = '\0';
    int* converted_args = malloc(sizeof(unsigned long)*(types[r-1]==I2_TYPE || types[r-1]==S_TYPE)?(n+1):n);
    if (!converted_args) {
        printf("Out of memory!");
        free(args);
        return NULL;
    }

    for (r=0; r<n; r++) {
        char* arg = args + r*128;
        printf("Arg %d: %s", r, arg);
        switch (types[r]) {
            case IMMEDIATE:

                char *endptr;
                
                if (args[0] == '0' && args[1] == 'x') {
                    converted_args[r] = strtol(arg, &endptr, 16);    
                } else if (args[0] == '0' && args[1] == 'o') {
                    converted_args[r] = strtol(arg, &endptr, 8);
                } else if (args[0] == '0' && args[1] == 'b') {
                    converted_args[r] = strtol(arg, &endptr, 2);
                } else {
                    converted_args[r] = strtol(arg, &endptr, 10);
                }

                if (endptr == arg || *endptr != '\0') {
                    printf("Argument %d on line %d is invalid for type %s: %s\n", r, *line_number, argument_type_names[types[r]], arg);
                    free(args);
                    return NULL;
                }

                break;

            case OFFSET:

                if (isalpha(arg[0])) {
                    char *endptr;
                    converted_args[r] = strtol(arg, &endptr, 10);

                    if (endptr == arg || *endptr != '\0') {
                        printf("Failed to interpret argument %d on line %d as numeric offset: %s\n", r, *line_number, arg);
                        free(args);
                        free(converted_args);
                        return NULL;
                    }

                } else {
                    int position = get_position(labels, arg);

                    if (position==-1) {
                        printf("Unseen label on line %d: %s\n", *line_number, arg);
                        free(args);
                        free(converted_args);
                        return NULL;
                    }

                    converted_args[r] = position - *line_number;
                }

                break;

            case REGISTER:
                converted_args[r] = search_register(arg);

                if (converted_args[r]==-1) {
                    printf("Unseen label on line %d: %s\n", *line_number, arg);
                    free(args);
                    free(converted_args);
                    return NULL;
                }
                
                break;

            case ADDRESS:
                
                break;

            default:
                printf("Error on line %d\nUnknown argument type %d for %s, did you forget to write a case?\n", *line_number, types[r], args + r*128);
				return NULL;
        }
        printf(" --> %d\n",converted_args[r]);
    }

    free(args);
    return converted_args;
}

// long R_type_parser(FILE** args, label_index* labels, int* line_number) {
//     FILE* fp = *args;
//     int i = 0;
//     int r = 0;
//     int result = 0;
//     char reg_name[3][8];
//     char c;
//     const int register_offsets[] = {7,15,20};

//     while ((c = fgetc(fp)) != EOF) {
//         switch (c){
//             case ' ':
//             case '\t':
//                 break;

//             case ',':
//                 if (r==2) {
//                     printf("Error on line %d, Expected 3 operands, Found more than 3\n", *line_number);
//                     return -1;
//                 }
//                 reg_name[r][i] = '\0';
//                 r++;
//                 i = 0;
//                 break;

//             case '\n':
//                 goto exit;

//             default:
//                 if (i==7) {
//                     printf("Error on line %d, Illegal operand: %s\n", *line_number, reg_name[r]);
//                     return -1;
//                 }
//                 reg_name[r][i++] = c;
//         }
//     }

// exit:
//     if (r<2) {
//         printf("Error on line %d, Expected 3 operands, Less operands than expected\n", *line_number);
//         return -1;
//     }
//     reg_name[r][i] = '\0';

//     for (r=0; r<3; r++) {
//         int addr = search_register(reg_name[r]);
//         if (addr!=-1) result += addr << register_offsets[r];
//         else {
//             printf("Error on line %d, Unknown alias: %s\n", *line_number, reg_name[r]);
//             return -1;
//         }
//     }

//     return result;
// }

long R_type_parser(FILE** args_raw, label_index* labels, int* line_number) {
    argument_type types[] = {REGISTER, REGISTER, REGISTER};
    int* args = args_parser(args_raw, labels, 3, types, line_number);

    if (!args) {
        return -1;
    }

    int result = (args[0] << 7) + (args[1] << 15) + (args[2] << 20);

    free(args);
    return result;
}

long I1_type_parser(FILE** args_raw, label_index* labels, int* line_number) {
    argument_type types[] = {REGISTER, REGISTER, IMMEDIATE};
    int* args = args_parser(args_raw, labels, 3, types, line_number);

    if (!args) {
        return -1;
    }

    if (args[2] >= 4096) {
        printf("Warning on line %d: Immediate value cannot exceed 12 bits. Some data will be lost...", *line_number);
    }

    int result = (args[0] << 7) + (args[1] << 15) + (args[2] << 20);
    free(args);
    return result;
}

long I2_type_parser(FILE** args_raw, label_index* labels, int* line_number) {
    argument_type types[] = {REGISTER, IMMEDIATE, REGISTER};
    int* args = args_parser(args_raw, labels, 3, types, line_number);

    if (!args) {
        return -1;
    }

    if (args[1] >= 4096) {
        printf("Warning on line %d: Immediate value cannot exceed 12 bits. Some data will be lost...", *line_number);
    }

    int result = (args[0] << 7) + (args[2] << 15) + (args[1] << 20);
    free(args);
    return result;
}

long S_type_parser(FILE** args_raw, label_index* labels, int* line_number) {
    argument_type types[] = {REGISTER, IMMEDIATE, REGISTER};
    int* args = args_parser(args_raw, labels, 3, types, line_number);

    if (!args) {
        return -1;
    }

    if (args[1] >= 4096) {
        printf("Warning on line %d: Immediate value cannot exceed 12 bits. Some data will be lost...", *line_number);
    }

    int rearranged_immediate = ((args[1] & 0x0000001F) << 7) + ((args[1] & 0x00000FE0) << 20);
    int result = (args[0] << 15) + rearranged_immediate + (args[1] << 20);
    free(args);
    return result;
}

long B_type_parser(FILE** args_raw, label_index* labels, int* line_number) {
    argument_type types[] = {REGISTER, REGISTER, OFFSET};
    int* args = args_parser(args_raw, labels, 3, types, line_number);

    if (!args) {
        return -1;
    }

    int rearranged_offset = ((args[2] & 0x0000001E) << 7) + ((args[1] & 0x00000400) >> 4) + ((args[1] & 0x00000800) << 19) + ((args[1] & 0x000007E0) << 20);
    int result = (args[0] << 15) + (args[1] << 20) + rearranged_offset;

    free(args);
    return result;
}

long U_type_parser(FILE** args_raw, label_index* labels, int* line_number) {
    argument_type types[] = {REGISTER, IMMEDIATE};
    int* args = args_parser(args_raw, labels, 2, types, line_number);

    if (!args) {
        return -1;
    }

    int result = (args[0] << 7) + (args[1] & (0xFFFFF000));

    free(args);
    return result;
}

long J_type_parser(FILE** args_raw, label_index* labels, int* line_number) {
    argument_type types[] = {REGISTER, IMMEDIATE};
    int* args = args_parser(args_raw, labels, 2, types, line_number);

    if (!args) {
        return -1;
    }

    int result = (args[0] << 7) + (args[1] & (0xFFFFF000));

    free(args);
    return result;
}

long I3_type_parser(FILE** args_raw, label_index* labels, int* line_number) {
    argument_type types[] = {REGISTER, OFFSET};
    int* args = args_parser(args_raw, labels, 2, types, line_number);

    if (!args) {
        return -1;
    }

    int rearranged_offset = (args[1] & 0x000FF000) + ((args[1] & 0x000007FE) << 20) + ((args[1] & 0x00000400) << 9) + ((args[1] & 0x00080000) << 11);
    int result = (args[0] << 7) + rearranged_offset;

    free(args);
    return result;
}


// Converts one instruction into Binary
// int translate_instruction(char* instruction, label_index* index, int line_number) {
    
//     char name[8]; // Sufficient for any valid instruction/pseudo instruction in Base class
//     int i;
//     for (i = 0; i<8; i++) {
//         if (instruction[i] == ' ' || instruction[i] == '\n') {
//             name[i] = '\0';
//             break;
//         }
//         name[i] = instruction[i];
//     }

//     if (i == 8) {
//         printf("Error on line %d\nInvalid Instruction: %s\n                     ^^^^^^^^\n", line_number, instruction);
//         return 0xFFFFFFFF;
//     }
    
//     return 0xAAAAAAAA;
// }


// const instruction_info ADD = {0x0110011,                      R_TYPE};
// const instruction_info SUB = {0x0110011+(0x20<<25),           R_TYPE};
// const instruction_info XOR = {0x0110011+(0x4<<12),            R_TYPE};
// const instruction_info OR =  {0x0110011+(0x6<<12),            R_TYPE};
// const instruction_info AND = {0x0110011+(0x7<<12),            R_TYPE};
// const instruction_info SLL = {0x0110011+(0x1<<12),            R_TYPE};
// const instruction_info SRL = {0x0110011+(0x5<<12),            R_TYPE};
// const instruction_info SRA = {0x0110011+(0x5<<12)+(0x20<<25), R_TYPE};
// const instruction_info SLT = {0x0110011+(0x2<<12),            R_TYPE};
// const instruction_info SLTU ={0x0110011+(0x3<<12),            R_TYPE};

// const hashmap* build_register_table(void) {
//     const hashmap* table = new_hashmap(256);

//     hm_set(table, "x0", (void*) 0);
//     hm_set(table, "x1", (void*) 1);
//     hm_set(table, "x2", (void*) 2);
//     hm_set(table, "x3", (void*) 3);
//     hm_set(table, "x4", (void*) 4);
//     hm_set(table, "x5", (void*) 5);
//     hm_set(table, "x6", (void*) 6);
//     hm_set(table, "x7", (void*) 7);
//     hm_set(table, "x8", (void*) 8);
//     hm_set(table, "x9", (void*) 9);
//     hm_set(table, "x10", (void*) 10);
//     hm_set(table, "x11", (void*) 11);
//     hm_set(table, "x12", (void*) 12);
//     hm_set(table, "x13", (void*) 13);
//     hm_set(table, "x14", (void*) 14);
//     hm_set(table, "x15", (void*) 15);
//     hm_set(table, "x16", (void*) 16);
//     hm_set(table, "x17", (void*) 17);
//     hm_set(table, "x18", (void*) 18);
//     hm_set(table, "x19", (void*) 19);
//     hm_set(table, "x20", (void*) 20);
//     hm_set(table, "x21", (void*) 21);
//     hm_set(table, "x22", (void*) 22);
//     hm_set(table, "x23", (void*) 23);
//     hm_set(table, "x24", (void*) 24);
//     hm_set(table, "x25", (void*) 25);
//     hm_set(table, "x26", 26);
//     hm_set(table, "x27", 27);
//     hm_set(table, "x28", 28);
//     hm_set(table, "x29", 29);
//     hm_set(table, "x30", 30);
//     hm_set(table, "x31", 31);
    
//     hm_set(table, "zero", 0);
//     hm_set(table, "ra", 1);
//     hm_set(table, "sp", 2);
//     hm_set(table, "gp", 3);
//     hm_set(table, "tp", 4);
//     hm_set(table, "t0", 5);
//     hm_set(table, "t1", 6);
//     hm_set(table, "t2", 7);
//     hm_set(table, "s0", 8);
//     hm_set(table, "fp", 8);
//     hm_set(table, "s1", 9);
//     hm_set(table, "a0", (void*) 10);
//     hm_set(table, "a1", (void*) 11);
//     hm_set(table, "a2", (void*) 12);
//     hm_set(table, "a3", (void*) 13);
//     hm_set(table, "a4", (void*) 14);
//     hm_set(table, "a5", (void*) 15);
//     hm_set(table, "a6", (void*) 16);
//     hm_set(table, "a7", (void*) 17);
//     hm_set(table, "s2", (void*) 18);
//     hm_set(table, "s3", (void*) 19);
//     hm_set(table, "s4", (void*) 20);
//     hm_set(table, "s5", (void*) 21);
//     hm_set(table, "s6", (void*) 22);
//     hm_set(table, "s7", (void*) 23);
//     hm_set(table, "s8", (void*) 24);
//     hm_set(table, "s9", (void*) 25);
//     hm_set(table, "s10", 26);
//     hm_set(table, "s11", 27);
//     hm_set(table, "t3", 28);
//     hm_set(table, "t4", 29);
//     hm_set(table, "t5", 30);
//     hm_set(table, "t6", 31);

//     return table;      
// }

// const hashmap* build_instruction_table(void) {
//     const hashmap* table = new_hashmap(32);

//     hm_set(table, "add", &ADD);
//     hm_set(table, "sub", &SUB);
//     hm_set(table, "or", &XOR);
//     hm_set(table, "xor", &OR);
//     hm_set(table, "and", &AND);
//     hm_set(table, "sll", &SLL);
//     hm_set(table, "srl", &SRL);
//     hm_set(table, "sra", &SRA);
//     hm_set(table, "slt", &SLT);
//     hm_set(table, "sltu", &SLTU);

//     return table;
// }