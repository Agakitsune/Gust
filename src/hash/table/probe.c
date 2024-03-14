
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdio.h>

#include "hash/table/probe.h"

int hash_table_uint64_linear_init(hash_table_uint64_linear_t *table, uint64_t initial_capacity) {
    table->entries = malloc(sizeof(hash_table_uint64_linear_entry_t) * initial_capacity);
    if (table->entries == NULL) {
        return -1;
    }
    memset(table->entries, 0, sizeof(hash_table_uint64_linear_entry_t) * initial_capacity);
    table->capacity = initial_capacity;
    table->load = 0;
    if (isPower2(initial_capacity)) {
        table->index = hash_table_uint64_linear_optimize_index;
    } else {
        table->index = hash_table_uint64_linear_normal_index;
    }
    return 0;
}

void hash_table_uint64_linear_free(const hash_table_uint64_linear_t *table) {
    free(table->entries);
}

uint64_t hash_table_uint64_linear_optimize_index(const hash_table_uint64_linear_t *table, uint64_t key) {
    return key & (table->capacity - 1);
}

uint64_t hash_table_uint64_linear_normal_index(const hash_table_uint64_linear_t *table, uint64_t key) {
    return key % table->capacity;
}

uint64_t hash_table_uint64_linear_find_index(const hash_table_uint64_linear_t *table, uint64_t key) {
    uint64_t index = table->index(table, key);
    uint64_t lock = index;
    while ((table->entries[index].value != 0) && (table->entries[index].key != key)) {
        index = table->index(table, index + 1);
        if (index == lock) { // avoid infinite loop
            return -1;
        }
    }
    return index;
}

int hash_table_uint64_linear_insert(hash_table_uint64_linear_t *table, uint64_t key, uint64_t value) {
    uint64_t index = hash_table_uint64_linear_find_index(table, key);
    if (index == -1) {
        return -1;
    }
    table->entries[index].key = key;
    table->entries[index].value = value;
    table->load++;
    if (table->load > (table->capacity >> 1)) {
        hash_table_uint64_linear_entry_t *old_entries = table->entries;
        uint64_t old_capacity = table->capacity;
        table->capacity <<= 1;
        table->entries = malloc(sizeof(hash_table_uint64_linear_entry_t) * table->capacity);
        if (table->entries == NULL) {
            table->entries = old_entries;
            table->capacity = old_capacity;
            return -1;
        }
        memset(table->entries, 0, sizeof(hash_table_uint64_linear_entry_t) * table->capacity);
        table->load = 0;
        for (uint64_t i = 0; i < old_capacity; ++i) {
            if (old_entries[i].value != 0) {
                hash_table_uint64_linear_insert(table, old_entries[i].key, old_entries[i].value);
            }
        }
        free(old_entries);
    }
    return 0;
}

uint64_t hash_table_uint64_linear_lookup(const hash_table_uint64_linear_t *table, uint64_t key) {
    uint64_t index = hash_table_uint64_linear_find_index(table, key);
    if (index == -1) {
        return NULL;
    }
    return table->entries[index].value;
}

uint64_t hash_table_uint64_linear_remove(hash_table_uint64_linear_t *table, uint64_t key) {
    uint64_t index = hash_table_uint64_linear_find_index(table, key);
    if (index == -1) {
        return NULL;
    }
    uint64_t value = table->entries[index].value;
    table->entries[index].value = 0;
    table->load--;
    return value;
}

void hash_table_uint64_linear_foreach(hash_table_uint64_linear_t *table, void (*callback)()) {
    for (uint64_t i = 0; i < table->capacity; ++i) {
        if (table->entries[i].value != 0) {
            callback(table->entries[i].key, table->entries[i].value);
        }
    }
}

void hash_table_uint64_linear_clear(hash_table_uint64_linear_t *table) {
    memset(table->entries, 0, sizeof(hash_table_uint64_linear_entry_t) * table->capacity);
    table->load = 0;
}
