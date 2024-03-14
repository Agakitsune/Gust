
#ifndef GUST_HASH_TABLE_PROBE_H
#define GUST_HASH_TABLE_PROBE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "hash/hash.h"

typedef struct hash_table_uint64_linear_entry_t {
    uint64_t key;
    uint64_t value;
} hash_table_uint64_linear_entry_t;

typedef struct hash_table_uint64_linear_t {
    hash_table_uint64_linear_entry_t *entries;
    uint64_t capacity;
    uint64_t load;
    uint64_t (*index)(const struct hash_table_uint64_linear_t *, uint64_t);
} hash_table_uint64_linear_t;

int hash_table_uint64_linear_init(hash_table_uint64_linear_t *table, uint64_t initial_capacity);

void hash_table_uint64_linear_free(const hash_table_uint64_linear_t *table);

uint64_t hash_table_uint64_linear_optimize_index(const hash_table_uint64_linear_t *table, uint64_t key);
uint64_t hash_table_uint64_linear_normal_index(const hash_table_uint64_linear_t *table, uint64_t key);

uint64_t hash_table_uint64_linear_find_index(const hash_table_uint64_linear_t *table, uint64_t key);
int hash_table_uint64_linear_insert(hash_table_uint64_linear_t *table, uint64_t key, uint64_t value);

uint64_t hash_table_uint64_linear_lookup(const hash_table_uint64_linear_t *table, uint64_t key);

uint64_t hash_table_uint64_linear_remove(hash_table_uint64_linear_t *table, uint64_t key);

void hash_table_uint64_linear_clear(hash_table_uint64_linear_t *table);

void hash_table_uint64_linear_foreach(hash_table_uint64_linear_t *table, void (*callback)());

#define hash_table_uint64_linear_lookup_type(table, key, type) ((type)hash_table_uint64_linear_lookup((table), (key)))

#define hash_table_uint64_linear_remove_type(table, key, type) ((type)hash_table_uint64_linear_remove((table), (key)))



typedef struct hash_table_ptr_linear_entry_t {
    uint64_t key;
    void *ptr;
} hash_table_ptr_linear_entry_t;

typedef struct hash_table_ptr_linear_t {
    hash_table_ptr_linear_entry_t *entries;
    uint64_t capacity;
    uint64_t load;
    uint64_t (*index)(const struct hash_table_ptr_linear_t *, uint64_t);
} hash_table_ptr_linear_t;

#define hash_table_ptr_linear_init(table, initial_capacity) hash_table_uint64_linear_init((hash_table_uint64_linear_t*)(table), (initial_capacity))

#define hash_table_ptr_linear_free(table) hash_table_uint64_linear_free((const hash_table_uint64_linear_t*)(table))

#define hash_table_ptr_linear_find_index(table, key) hash_table_uint64_linear_find_index((const hash_table_uint64_linear_t*)(table), (key))
#define hash_table_ptr_linear_insert(table, key, ptr) hash_table_uint64_linear_insert((hash_table_uint64_linear_t*)(table), (key), (uint64_t)(ptr))

#define hash_table_ptr_linear_lookup(table, key) ((void*)hash_table_uint64_linear_lookup((const hash_table_uint64_linear_t*)(table), (key)))

#define hash_table_ptr_linear_remove(table, key) ((void*)hash_table_uint64_linear_remove((hash_table_uint64_linear_t*)(table), (key)))

#define hash_table_ptr_linear_clear(table) hash_table_uint64_linear_clear((hash_table_uint64_linear_t*)(table))

#define hash_table_ptr_linear_foreach(table, callback) hash_table_uint64_linear_foreach((hash_table_uint64_linear_t*)(table), (callback))

#define hash_table_ptr_linear_lookup_type(table, key, type) ((type)hash_table_ptr_linear_lookup((table), (key)))

#define hash_table_ptr_linear_remove_type(table, key, type) ((type)hash_table_ptr_linear_remove((table), (key)))

#ifdef __cplusplus
}
#endif

#endif // GUST_HASH_TABLE_PROBE_H
