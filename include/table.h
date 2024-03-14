
#ifndef GUST_TABLE_H
#define GUST_TABLE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "type.h"

// typedef struct world_t world_t;

typedef struct table_t {
    type_t type; // What types of data are in the table

    uint64_t block_size;
    uint64_t block_count;
    uint64_t row_size; // How many bytes are in a row
    uint64_t count; // How many rows are in the table
    uint64_t capacity; // How many rows can fit in the table

    void *data;
} table_t;

table_t *gust_add_table(world_t *world, type_t *type, uint64_t block_size);
int gust_init_table(world_t *world, table_t *table, type_t *type, uint64_t block_size);
void gust_fini_table(table_t *table);

table_t *gust_get_table(world_t *world, type_t *type);
int gust_add_to_table(table_t *table, void *data);
void *gust_get_data_from_table_w_id(const world_t *world, const table_t *table, uint64_t row, uint64_t id);
void *gust_get_data_from_table_w_shift(const table_t *table, uint64_t row, uint64_t shift);
int gust_clear_row_from_table(table_t *table, uint64_t row);
void *gust_get_row(const table_t *table, uint64_t row);
void *gust_get_last_row(const table_t *table);
void *gust_get_last_row_for_manual_insert(table_t *table);

#ifdef __cplusplus
}
#endif

#endif // GUST_TABLE_H
