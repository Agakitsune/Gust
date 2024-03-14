
#include "world.hpp"
#include "component.h"

#include <cstdlib>

namespace gust {

    World::World() {
        gust_init(&world);
    }

    World::~World() {
        for (Entity c = 0; c < world.last_component; c++) {
            std::free((void*)gust_get_component_name(&world, c));
        }
        gust_fini(&world);
    }

    Entity World::newEntity() {
        return new_entity(&world);
    }

    void World::newEntity(Entity &entity) {
        entity = new_entity(&world);
    }

    void World::newEntityBatch(Entity *entities, size_t count) {
        new_entity_batch(&world, entities, count);
    }

    void World::removeEntity(Entity entity) {
        remove_entity(&world, entity);
    }

    void World::removeEntityBatch(Entity *entities, size_t count) {
        remove_entity_batch(&world, entities, count);
    }

    bool World::update() {
        return gust_update(&world);
    }

    world_t *World::handle() {
        return &world;
    }

    const world_t *World::handle() const {
        return &world;
    }

}
