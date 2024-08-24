#include <stdlib.h>
#include <stdio.h>

typedef struct vec {
	size_t len;
	size_t capacity;
	int* values;
} vec;

vec* new_managed_array() {
	vec* array = malloc(sizeof(vec)); 
	
	if (!array) {
		return NULL;
	}

	array->len = 0;
	array->capacity = 4;
	array->values = malloc(4*sizeof(int));

	if (!array->values) {
		free(array);
		return NULL;
	}

	return array;
}

int append(vec* array, int value) {

	array->values[array->len] = value;
	array->len++;

	if (array->len == array->capacity) {
		array->capacity *= 2;
		int* values_new = realloc(array->values, array->capacity * sizeof(char*));
		if (!values_new) {
			printf("Failed to allocate memory for label index\n");
			return 1;
		}
		array->values = values_new;
	}

	return 0;
}

void free_managed_array(vec* array) {
	free(array->values);
	free(array);
}

void print_array(vec* index) {

	printf("[");
	for(int i=0; i<index->len; i++) {
		printf("%d,", index->values[i]);
	}
	printf("]\n");
	
}