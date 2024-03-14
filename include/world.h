
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

typedef struct allocator_t {
    classic_t entity_allocator;
    classic_t pool_allocator;
} allocator_t;

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

#ifdef __cplusplus
}
#endif

#endif // GUST_H
