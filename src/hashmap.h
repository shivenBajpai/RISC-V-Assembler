#ifndef HASHMAP_H
#define HASHMAP_H

#include <stdio.h>

typedef struct hashmap hashmap;

hashmap* new_hashmap(int size);

void free_hashmap(hashmap *obj);

void* hm_get(hashmap* table, const char* key);

const char* hm_set(hashmap* table,  const char* key, void* value);

size_t hm_len(hashmap* table);

#endif