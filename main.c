
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "world.h"
#include "entity.h"
#include "iter.h"
#include "table.h"
#include "component.h"
#include "system.h"

// #include "allocator.h"
#include "allocator/pool.h"

#include "vector/classic.h"

#include "hash/hash.h"
#include "hash/table/probe.h"

typedef struct Velocity {
    float x;
    float y;
} Velocity;

typedef struct Position {
    float x;
    float y;
} Position;

void move(gust_iter_t *it) {
    Velocity *v = gust_field(it, Velocity);
    Position *p = gust_field(it, Position);

    printf("------\n");
    printf("Pos: %f %f\n", p->x, p->y);
    printf("Vel: %f %f\n", v->x, v->y);
}

void test(gust_iter_t *it) {
    Position *p = gust_field(it, Position);

    printf("------\n");
    printf("Pos: %f %f\n", p->x, p->y);
}

int main() {
    SEED = time(NULL);

    world_t world;
    gust_init(&world);
    Velocity vel = { 1.0f, 2.0f };
    Position pos = { 3.0f, 4.0f };


    uint64_t id = GUST_COMPONENT(&world, Velocity);
    uint64_t id2 = GUST_COMPONENT(&world, Position);

    entity_t system = GUST_SYSTEM(&world, move, Position, Velocity);
    GUST_SYSTEM(&world, test, Position);

    for (int i = 0; i < 5; i++) {
        entity_t entity = new_entity(&world);
        gust_set_component(&world, entity, id, &vel);
        gust_set_component(&world, entity, id2, &pos);
    }

    gust_update(&world);
    remove_entity(&world, 1);
    entity_t entity = new_entity(&world);
    gust_set_component(&world, entity, id2, &vel);
    gust_update(&world);

    gust_fini(&world);
    return 0;
}
