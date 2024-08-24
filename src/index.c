#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

int label_to_position(label_index* index, char* label) {
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

	for(int i=0; i<index->len; i++) {
		printf("%s: %d\n", index->labels[i], index->positions[i]);
	}
	
}