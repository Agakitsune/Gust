
#ifndef GUST_WORLD_H
#define GUST_WORLD_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

#include "asm.h"

#include "entity.h"
#include "allocator/classic.h"
#include "allocator/pool.h"
#include "vector/classic.h"

#include "hash/hash.h"
#include "hash/table/probe.h"

#include "type.h"

uint64_t shift_query(world_t *world, type_t *type, uint64_t id);

typedef struct allocator_t {
    classic_t entity_allocator;
    classic_t pool_allocator;
} allocator_t;


typedef struct component_t {
    uint64_t size;
    const char *name;
} component_t;

entity_t gust_register_component(world_t *world, uint64_t size, const char *name);
const component_t *gust_get_component_metadata_by_name(const world_t *world, const char *name);
const component_t *gust_get_component_metadata_by_id(const world_t *world, uint64_t component);
uint64_t gust_get_component_id(const world_t *world, const char *name);
uint64_t gust_get_component_id_w_n(const world_t *world, const char *name, size_t n);

const char *gust_get_component_name(const world_t *world, uint64_t component);
uint64_t gust_get_component_size(const world_t *world, uint64_t component);


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
void *gust_get_data_from_table_w_id(world_t *world, table_t *table, uint64_t row, uint64_t id);
void *gust_get_data_from_table_w_shift(table_t *table, uint64_t row, uint64_t shift);
int gust_clear_row_from_table(table_t *table, uint64_t row);
void *gust_get_row(table_t *table, uint64_t row);
void *gust_get_last_row(table_t *table);
void *gust_get_last_row_for_manual_insert(table_t *table);


typedef struct entity_record_t {
    table_t *table; // The table the entity is in
    uint64_t row; // The row the entity is in
} entity_record_t;

entity_record_t *gust_add_record(world_t *world, entity_t entity);
int gust_init_record(entity_record_t *record, table_t *table);
entity_record_t *gust_get_record(world_t *world, entity_t entity);
void gust_remove_record(world_t *world, entity_t entity);

typedef struct query_t {
    entity_t *ids;
    uint64_t count;
} query_t;

int new_query(query_t *query, entity_t component);
int copy_query(query_t *dest, const query_t *src);
int add_query(query_t *query, entity_t component);
int remove_query(query_t *query, entity_t component);
int has_query(const query_t *query, entity_t component);
int compare_query(const query_t *a, const query_t *b);
uint64_t index_query(const query_t *query, entity_t component);
void free_query(query_t *query);
void display_query(const query_t *query);

int query_match_type(const query_t *query, const type_t *type);


typedef struct gust_iter_t {
    query_t query;

    world_t *world;

    uint64_t row; // The current row
    uint64_t *shifts; // The shifts of the current row

    table_t *actual; // The table the iterator is currently on
    table_t *next; // The table the iterator will be on next
    uint64_t index; // The index of the next table

    void **data; // The data of the current row
} gust_iter_t;

entity_t gust_new_iter(world_t *world, query_t *query);
int gust_iter_init(world_t *world, gust_iter_t *it, query_t *query);
void gust_iter_fini(gust_iter_t *it);

gust_iter_t *gust_get_iter(world_t *world, entity_t iter);
void prepare_iter(world_t *world, gust_iter_t *it);
int gust_iter_next(world_t *world, gust_iter_t *it);

void *gust_get_data(gust_iter_t *it, uint64_t index);
void *gust_get_data_from_name(gust_iter_t *it, const char *name);

#define gust_field(it, type) (type*)gust_get_data_from_name((it), #type)

typedef struct system_t {
    void (*system)(gust_iter_t*);
} system_t;

int gust_generate_query(world_t *world, query_t *query, const char *str);
entity_t gust_declare_system(world_t *world, void (*system)(gust_iter_t*), const char *query);
entity_t gust_new_system(world_t *world, void (*system)(gust_iter_t*));

system_t *gust_get_system(world_t *world, entity_t system);

struct world_t {
    allocator_t allocators;

    cvector_t free_entity;
    cvector_t entity_atlas;
    
    cvector_t component_metadata_pages;
    cvector_t component_hash;

    cvector_t tables;

    cvector_t iterators;
    cvector_t systems;

    entity_t last_entity;
    entity_t last_component;
    entity_t last_iter;
    entity_t last_system;
};

world_t *gust_init(world_t *world);
void gust_fini(const world_t *world);
int gust_update(world_t *world);

int gust_set_component(world_t *world, entity_t entity, uint64_t component, void *data);

#define GUST_COMPONENT(world, type) gust_register_component((world), sizeof(type), #type)
#define GUST_SYSTEM(world, func, ...) gust_declare_system((world), (func), #__VA_ARGS__)

#define MASK(bit) ((1 << (bit)) - 1)
#define PAGE_SIZE(bit) (1 << (bit))

#define GUST_COMPONENT_BIT 8
#define GUST_COMPONENT_MASK MASK(GUST_COMPONENT_BIT)
#define GUST_COMPONENT_PAGE_SIZE PAGE_SIZE(GUST_COMPONENT_BIT)

#define GUST_ENTITY_BIT GUST_COMPONENT_BIT
#define GUST_ENTITY_MASK GUST_COMPONENT_MASK
#define GUST_ENTITY_PAGE_SIZE GUST_COMPONENT_PAGE_SIZE

#define GUST_ITER_BIT 6
#define GUST_ITER_MASK MASK(GUST_ITER_BIT)
#define GUST_ITER_PAGE_SIZE PAGE_SIZE(GUST_ITER_BIT)

#define GUST_SYSTEM_BIT 9
#define GUST_SYSTEM_MASK MASK(GUST_SYSTEM_BIT)
#define GUST_SYSTEM_PAGE_SIZE PAGE_SIZE(GUST_SYSTEM_BIT)

#ifdef __cplusplus
}
#endif

#endif // GUST_H
