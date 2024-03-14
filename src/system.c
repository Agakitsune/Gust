
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "system.h"
#include "world.h"

int gust_generate_query(world_t *world, query_t *query, const char *str) {
    size_t len = 0;
    uint64_t id = 0;
    while (1) {
        len = strcspn(str, " ,");
        id = gust_get_component_id_w_n(world, str, len);
        if (id == -1) {
            return -1;
        }
        if (add_query(query, id) == -1) {
            return -1;
        }
        str += len;
        if (!str[0]) {
            break;
        }
        str += strspn(str, " ,");
    }
    return 0;
}

entity_t gust_declare_system(world_t *world, void (*system)(gust_iter_t*), const char *query) {
    query_t q = {NULL, 0};
    if (gust_generate_query(world, &q, query) == -1) {
        return 0;
    }
    if (gust_new_iter(world, &q) == -1) {
        free_query(&q);
        return 0;
    }
    return gust_new_system(world, system);
}

entity_t gust_declare_system_w_query(world_t *world, void (*system)(gust_iter_t*), query_t *query) {
    if (gust_new_iter(world, query) == -1) {
        free_query(&query);
        return 0;
    }
    return gust_new_system(world, system);
}

entity_t gust_new_system(world_t *world, void (*system)(gust_iter_t*)) {
    entity_t sysindex = world->last_system++;
    uint64_t index = sysindex >> GUST_SYSTEM_BIT;
    system_t **systems = cvector_data(&world->systems);
    if (index > cvector_size(&world->systems)) {
        void *page = malloc(GUST_SYSTEM_PAGE_SIZE * sizeof(system_t));
        if (page == NULL) {
            return 0;
        }
        cvector_add(&world->allocators.pool_allocator, &world->systems, &page);
    }
    system_t *sys = &systems[index][sysindex & GUST_SYSTEM_MASK];
    sys->system = system;
    return sysindex;
}

system_t *gust_get_system(world_t *world, entity_t system) {
    uint64_t index = system >> GUST_SYSTEM_BIT;
    system_t **systems = cvector_data(&world->systems);
    return &systems[index][system & GUST_SYSTEM_MASK];
}