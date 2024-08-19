#ifndef VEC_H
#define VEC_H
#include <stdlib.h>

typedef struct Vec {
	size_t len;
	size_t capacity;
	int* values;
} Vec;

int append(Vec* array, int value);

Vec* new_managed_array();

void free_managed_array(Vec* array);

void print_array(Vec* index);

#endif