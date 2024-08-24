#ifndef VEC_H
#define VEC_H
#include <stdlib.h>

typedef struct vec {
	size_t len;
	size_t capacity;
	int* values;
} vec;

int append(vec* array, int value);

vec* new_managed_array();

void free_managed_array(vec* array);

void print_array(vec* index);

#endif