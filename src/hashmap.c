#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#define INITIAL_CAPACITY 16
#define FNV_OFFSET 14695981039346656037UL
#define FNV_PRIME 1099511628211UL

typedef struct {
    const char* key;
    void* value;
} hm_entry;

typedef struct hashmap {
        hm_entry* entries;
        size_t capacity;
        size_t len;
} hashmap;

hashmap* new_hashmap(int size) {
    hashmap* table = malloc(sizeof(hashmap));
    if (table == NULL) {
        return NULL;
    }

    table->len = 0;
    table->capacity = size<16?INITIAL_CAPACITY:size;

    table->entries = calloc(table->capacity, sizeof(hm_entry));
    if (!table->entries) {
        free(table);
        return NULL;
    }

    return table;
};

void free_hashmap(hashmap *obj) {
    for (size_t i = 0; i < obj->capacity; i++) {
        free((void*)obj->entries[i].key);
    }

    free(obj->entries);
    free(obj);
};

static unsigned long hash_function(const char* key) {
    unsigned long hash = FNV_OFFSET;
    for (const char* p = key; *p; p++) {
        hash ^= (unsigned long)(unsigned char)(*p);
        hash *= FNV_PRIME;
    }
    return hash;
};

void* hm_get(hashmap* table, const char* key) {
    unsigned long hash = hash_function(key);
    size_t index = (size_t)(hash & (unsigned long)(table->capacity - 1));

    while (table->entries[index].key != NULL) {
        
        if (strcmp(key, table->entries[index].key) == 0) {
            return table->entries[index].value;
        }
        
        index++;
        if (index >= table->capacity) {
            index = 0;
        }
    }

    return NULL;
};

static const char* ht_set_entry(hm_entry* entries, size_t capacity, const char* key, void* value, size_t* plength) {
    unsigned long hash = hash_function(key);
    size_t index = (size_t)(hash & (unsigned long)(capacity - 1));

    // Loop till we find an empty entry.
    while (entries[index].key != NULL) {
        if (strcmp(key, entries[index].key) == 0) {
            // Found key (it already exists), update value.
            entries[index].value = value;
            return entries[index].key;
        }
        // Key wasn't in this slot, move to next (linear probing).
        index++;
        if (index >= capacity) {
            // At end of entries array, wrap around.
            index = 0;
        }
    }

    // Didn't find key, allocate+copy if needed, then insert it.
    if (plength != NULL) {
        key = strdup(key);
        if (key == NULL) {
            return NULL;
        }
        (*plength)++;
    }
    entries[index].key = (char*)key;
    entries[index].value = value;
    return key;
}

static bool ht_expand(hashmap* table) {
    
    // Allocate new entries array.
    size_t new_capacity = table->capacity * 2;
    if (new_capacity < table->capacity) {
        return false;  // overflow (capacity would be too big)
    }

    hm_entry* new_entries = calloc(new_capacity, sizeof(hm_entry));
    if (new_entries == NULL) {
        return false;
    }

    // Iterate entries, move all non-empty ones to new table's entries.
    for (size_t i = 0; i < table->capacity; i++) {
        hm_entry entry = table->entries[i];
        if (entry.key != NULL) {
            ht_set_entry(new_entries, new_capacity, entry.key,
                         entry.value, NULL);
        }
    }

    // Free old entries array and update this table's details.
    free(table->entries);
    table->entries = new_entries;
    table->capacity = new_capacity;
    return true;
}


const char* hm_set(hashmap* table,  const char* key, void* value) {
    assert(value != NULL);
    if (value == NULL) {
        return NULL;
    }
    
    if (table->len >= (table->capacity/2)) {
        if (!ht_expand(table)) {
            return NULL;
        }
    }

    return ht_set_entry(table->entries, table->capacity, key, value, &table->len);
};

size_t hm_len(hashmap* table) {
    return table->len;
};