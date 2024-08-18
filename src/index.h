#ifndef INDEX_H
#define INDEX_H
#include <stdlib.h>

typedef struct label_index label_index;

int add_label(label_index* index, char* label, int position);

int get_position(label_index* index, char* label);

label_index* new_label_index();

void free_label_index(label_index* index);

void debug_print_label_index(label_index* index);

typedef struct managed_array {
	size_t len;
	size_t capacity;
	int* values;
} managed_array;

int append(managed_array* array, int value);

managed_array* new_managed_array();

void free_managed_array(managed_array* array);

void print_array(managed_array* index);

#endif
