
#include <string.h>
#include <stdlib.h>

#include "table.h"
#include "world.h"

table_t *gust_add_table(world_t *world, type_t *type, uint64_t block_size) {
    // table_t **tables = cvector_data(&world->tables);
    table_t *table = malloc(sizeof(table_t));
    if (table == NULL) {
        return NULL;
    }
    cvector_add(&world->allocators.pool_allocator, &world->tables, &table);
    if (gust_init_table(world, table, type, block_size) == -1) {
        free(table);
        return NULL;
    }
    return table;
}

int gust_init_table(world_t *world, table_t *table, type_t *type, uint64_t block_size) {
    uint64_t row_size = 0;
    for (uint64_t i = 0; i < type->count; i++) {
        row_size += gust_get_component_size(world, type->ids[i]);
    }
    copy_type(&table->type, type);
    table->block_size = block_size;
    table->block_count = 1;
    table->row_size = row_size;
    table->count = 0;
    uint64_t size = block_size;
    if (isPower2(row_size)) {
        table->capacity = size >> log2i(row_size);
    } else {
        table->capacity = size / row_size;
    }
    table->data = malloc(size);
    if (table->data == NULL) {
        return -1;
    }
    return 0;
}

void gust_fini_table(table_t *table) {
    free(table->data);
    free_type(&table->type);
}

table_t *gust_get_table(world_t *world, type_t *type) {
    table_t **tables = cvector_data(&world->tables);
    for (uint64_t i = 0; i < cvector_size(&world->tables); i++) {
        if (compare_type(&tables[i]->type, type)) {
            return tables[i];
        }
    }
    return NULL;
}

int gust_add_to_table(table_t *table, void *data) {
    if (table->count == table->capacity) {
        uint64_t size = table->block_size * ++table->block_count;
        if (isPower2(table->row_size)) {
            table->capacity = size >> log2i(table->row_size);
        } else {
            table->capacity = size / table->row_size;
        }
        table->data = realloc(table->data, size);
        if (table->data == NULL) {
            return -1;
        }
    }
    memcpy((char*)table->data + table->count * table->row_size, data, table->row_size);
    table->count++;
    return 0;
}

void *gust_get_data_from_table_w_id(const world_t *world, const table_t *table, uint64_t row, uint64_t id) {
    uint64_t shift = shift_type(world, &table->type, id);
    if (shift == -1) {
        return NULL;
    }
    return gust_get_data_from_table_w_shift(table, row, shift);
}

inline void *gust_get_data_from_table_w_shift(const table_t *table, uint64_t row, uint64_t shift) {
    return (char*)table->data + row * table->row_size + shift;
}

int gust_clear_row_from_table(table_t *table, uint64_t row) {
    if (row >= table->count) {
        return -1;
    }
    void *last = gust_get_last_row(table);
    void *current = gust_get_row(table, row);
    memmove(current, (char*)current + table->row_size, (char*)last - (char*)current - table->row_size);
    table->count--;
    return 0;
}

inline void *gust_get_row(const table_t *table, uint64_t row) {
    return (char*)table->data + row * table->row_size;
}

inline void *gust_get_last_row(const table_t *table) {
    return (char*)table->data + table->count * table->row_size;
}

void *gust_get_last_row_for_manual_insert(table_t *table) {
    if (table->count == table->capacity) {
        uint64_t size = table->block_size * ++table->block_count;
        if (isPower2(table->row_size)) {
            table->capacity = size >> log2i(table->row_size);
        } else {
            table->capacity = size / table->row_size;
        }
        table->data = realloc(table->data, size);
        if (table->data == NULL) {
            return NULL;
        }
    }
    return gust_get_last_row(table);
}
