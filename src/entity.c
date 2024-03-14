
#include <stdlib.h>

#include "entity.h"
#include "world.h"

entity_t new_entity(world_t *world) {
    if (cvector_size(&world->free_entity) > 0) {
        entity_t entity = cvector_back_type(&world->allocators.entity_allocator, &world->free_entity, entity_t);
        cvector_remove(&world->allocators.entity_allocator, &world->free_entity, cvector_size(&world->free_entity) - 1);
        return entity;
    }
    entity_t entity = world->last_entity++;
    gust_add_record(world, entity);
    return entity;
}

void new_entity_batch(world_t *world, entity_t *entities, size_t count) {
    size_t c = count;
    if (cvector_size(&world->free_entity) > c) {
        size_t size = cvector_size(&world->free_entity);
        size_t diff = size - c;
        classic_t *allocator = &world->allocators.entity_allocator;
        cvector_t *vector = &world->free_entity;
        for (size_t i = 0; i < diff; i++) {
            entities[i] = cvector_index_type(allocator, vector, size - i - 1, entity_t);
            gust_add_record(world, entities[i]);
        }
        cvector_remove_range(allocator, vector, size - diff, diff);
        c -= diff;
    }
    for (size_t i = 0; i < c; i++) {
        entities[i] = world->last_entity++;
        gust_add_record(world, entities[i]);
    }
}

void remove_entity(world_t *world, entity_t entity) {
    classic_t *allocator = &world->allocators.entity_allocator;
    cvector_add(allocator, &world->free_entity, &entity);
    gust_remove_record(world, entity);
}

void remove_entity_batch(world_t *world, entity_t *entities, size_t count) {
    classic_t *allocator = &world->allocators.entity_allocator;
    cvector_add_range(allocator, &world->free_entity, entities, count);
    for (size_t i = 0; i < count; i++) {
        gust_remove_record(world, entities[i]);
    }
}
