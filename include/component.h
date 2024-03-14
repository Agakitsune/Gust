
#ifndef GUST_COMPONENT_H
#define GUST_COMPONENT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

#include "utils.h"
#include "entity.h"

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

#define GUST_COMPONENT(world, type) gust_register_component((world), sizeof(type), #type)

#define GUST_COMPONENT_BIT 8
#define GUST_COMPONENT_MASK MASK(GUST_COMPONENT_BIT)
#define GUST_COMPONENT_PAGE_SIZE PAGE_SIZE(GUST_COMPONENT_BIT)

#ifdef __cplusplus
}
#endif

#endif // GUST_COMPONENT_H
