#include <stdio.h>
#include <stdlib.h>

typedef struct label_index {
	size_t len;
	size_t capacity;
	char** labels;
	int* positions;
} label_index;

int add_label(label_index* index, char* in_label, int position) {
	
	int label_length = strlen(in_label);
	char* label = malloc(label_length * sizeof(char));
	strcpy(label, in_label);

	index->labels[index->len] = label;
	index->positions[index->len] = position;
	index->len++;

	if (index->len == index->capacity) {
		index->capacity *= 2;
		char** labels_new = realloc(index->labels, index->capacity * sizeof(char*));
		int* positions_new = realloc(index->positions, index->capacity * sizeof(int));
		if (!labels_new || !positions_new) {
			printf("Failed to allocate memory for label index\n");
			return 1;
		}

		index->labels = labels_new;
		index->positions = positions_new;
	}
	return 0;
}

int get_position(label_index* index, char* label) {
	for(int i=0; i<index->len; i++) {
		if (strcmp(index->labels[i], label) == 0) {
			return index->positions[i];
		}
	}
	
	return -1;
}

label_index* new_label_index() {
	label_index* index = malloc(sizeof(label_index)); 
	
	if (!index) {
		return NULL;
	}

	index->len = 0;
	index->capacity = 4;
	index->labels = malloc(4*sizeof(char*));

	if (!index->labels) {
		free(index);
		return NULL;
	}

	index->positions = malloc(4*sizeof(int));

	if (!index->positions) {
		free(index->labels);
		free(index);
		return NULL;
	}

	return index;
}

void free_label_index(label_index* index) {

	size_t len = index->len;
	for (int i=0; i<len; i++) {
		free(index->labels[i]);
	}

	free(index->labels);
	free(index->positions);
	free(index);
}

void debug_print_label_index(label_index* index) {

	printf("==START==\n");
	for(int i=0; i<index->len; i++) {
		printf("%s: %d\n", index->labels[i], index->positions[i]);
	}
	printf("==END==\n");
	
}

typedef struct managed_array {
	size_t len;
	size_t capacity;
	int* values;
} managed_array;

managed_array* new_managed_array() {
	managed_array* array = malloc(sizeof(managed_array)); 
	
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

int append(managed_array* array, int value) {

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

void free_managed_array(managed_array* array) {
	free(array->values);
	free(array);
}

void print_array(managed_array* index) {

	printf("[");
	for(int i=0; i<index->len; i++) {
		printf("%d,", index->values[i]);
	}
	printf("]\n");
	
}