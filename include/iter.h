
#ifndef GUST_ITER_H
#define GUST_ITER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "utils.h"
#include "query.h"
#include "table.h"

// typedef struct world_t world_t;

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

gust_iter_t *gust_get_iter(const world_t *world, entity_t iter);
void prepare_iter(world_t *world, gust_iter_t *it);
int gust_iter_next(world_t *world, gust_iter_t *it);

void *gust_get_data(gust_iter_t *it, uint64_t index);
void *gust_get_data_from_name(gust_iter_t *it, const char *name);

#define gust_field(it, type) (type*)gust_get_data_from_name((it), #type)

#define GUST_ITER_BIT 6
#define GUST_ITER_MASK MASK(GUST_ITER_BIT)
#define GUST_ITER_PAGE_SIZE PAGE_SIZE(GUST_ITER_BIT)

#ifdef __cplusplus
}
#endif

#endif // GUST_ITER_H
