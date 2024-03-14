
#ifndef GUST_ENTITY_H
#define GUST_ENTITY_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct world_t world_t;

typedef uint64_t entity_t;

entity_t new_entity(world_t *world);
void new_entity_batch(world_t *world, entity_t *entities, uint64_t count);

void remove_entity(world_t *world, entity_t entity);
void remove_entity_batch(world_t *world, entity_t *entities, uint64_t count);

#ifdef __cplusplus
}
#endif

#endif // GUST_H