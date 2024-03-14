
#ifndef GUST_RECORD_H
#define GUST_RECORD_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "utils.h"
#include "entity.h"
#include "table.h"

typedef struct entity_record_t {
    table_t *table; // The table the entity is in
    uint64_t row; // The row the entity is in
} entity_record_t;

entity_record_t *gust_add_record(world_t *world, entity_t entity);
int gust_init_record(entity_record_t *record, table_t *table);
entity_record_t *gust_get_record(world_t *world, entity_t entity);
void gust_remove_record(world_t *world, entity_t entity);

#define GUST_ENTITY_BIT 8
#define GUST_ENTITY_MASK MASK(GUST_ENTITY_BIT)
#define GUST_ENTITY_PAGE_SIZE PAGE_SIZE(GUST_ENTITY_BIT)

#ifdef __cplusplus
}
#endif

#endif // GUST_RECORD_H
