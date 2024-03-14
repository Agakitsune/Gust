
#include <string.h>
#include <stdlib.h>

#include "component.h"
#include "world.h"

entity_t gust_register_component(world_t *world, uint64_t size, const char *name) {
    uint64_t id = world->last_component++;
    uint64_t index = id >> GUST_COMPONENT_BIT;
    if (index >= cvector_size(&world->component_metadata_pages)) {
        void *page = malloc(GUST_COMPONENT_PAGE_SIZE * sizeof(component_t));
        if (page == NULL) {
            return 0;
        }
        cvector_add(&world->allocators.pool_allocator, &world->component_metadata_pages, &page);
    }
    component_t **pages = cvector_data(&world->component_metadata_pages);
    component_t *page = pages[index];
    component_t *comp = &page[id & GUST_COMPONENT_MASK];
    comp->size = size;
    comp->name = name;
    entity_t hash = murmur64strseedless(name, strlen(name));
    cvector_add(&world->allocators.entity_allocator, &world->component_hash, &hash);
    return id;
}

inline const component_t *gust_get_component_metadata_by_name(const world_t *world, const char *name) {
    return gust_get_component_metadata_by_id(world, gust_get_component_id(world, name));
}

const component_t *gust_get_component_metadata_by_id(const world_t *world, uint64_t id) {
    uint64_t index = id >> GUST_COMPONENT_BIT;
    component_t **pages = cvector_data(&world->component_metadata_pages);
    return &pages[index][id & GUST_COMPONENT_MASK];
}

inline uint64_t gust_get_component_id(const world_t *world, const char *name) {
    return gust_get_component_id_w_n(world, name, strlen(name));
}

uint64_t gust_get_component_id_w_n(const world_t *world, const char *name, size_t n) {
    entity_t hash = murmur64strseedless(name, n);
    entity_t *hashes = cvector_data(&world->component_hash);
    for (uint64_t index = 0; index < cvector_size(&world->component_hash); index++) {
        if (hash == hashes[index]) {
            return index;
        }
    }
    return -1;
}

inline const char *gust_get_component_name(const world_t *world, uint64_t component) {
    return gust_get_component_metadata_by_id(world, component)->name;
}

inline uint64_t gust_get_component_size(const world_t *world, uint64_t component) {
    return gust_get_component_metadata_by_id(world, component)->size;
}
