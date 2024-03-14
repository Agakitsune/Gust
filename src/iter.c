
#include "iter.h"
#include "world.h"

entity_t gust_new_iter(world_t *world, query_t *query) {
    entity_t iter = world->last_iter++;
    uint64_t index = iter >> GUST_ITER_BIT;
    gust_iter_t **iters = cvector_data(&world->iterators);
    if (index > cvector_size(&world->iterators)) {
        void *page = malloc(GUST_ITER_PAGE_SIZE * sizeof(gust_iter_t));
        if (page == NULL) {
            return 0;
        }
        cvector_add(&world->allocators.pool_allocator, &world->iterators, &page);
    }
    gust_iter_t *it = &iters[index][iter & GUST_ITER_MASK];
    if (gust_iter_init(world, it, query) == -1) {
        return -1;
    }
    return iter;
}

int gust_iter_init(world_t *world, gust_iter_t *it, query_t *query) {
    it->query = *query;
    it->world = world;
    it->row = 0;
    it->shifts = malloc(query->count * sizeof(uint64_t));
    if (it->shifts == NULL) {
        return -1;
    }
    it->actual = NULL;
    it->next = NULL;
    it->index = 0;
    it->data = malloc(query->count * sizeof(void*));
    return 0;
}

void gust_iter_fini(gust_iter_t *it) {
    free_query(&it->query);
    free(it->shifts);
    free(it->data);
}

gust_iter_t *gust_get_iter(const world_t *world, entity_t iter) {
    uint64_t index = iter >> GUST_ITER_BIT;
    gust_iter_t **iters = cvector_data(&world->iterators);
    return &iters[index][iter & GUST_ITER_MASK];
}

void estimate_shifts(world_t *world, uint64_t *shifts, query_t *query, table_t *table) {
    for (uint64_t i = 0; i < query->count; i++) {
        shifts[i] = shift_query(world, &table->type, query->ids[i]);
    }
}

void put_data(gust_iter_t *it) {
    if (it->actual == NULL) {
        return;
    }
    for (uint64_t i = 0; i < it->query.count; i++) {
        it->data[i] = gust_get_data_from_table_w_shift(it->actual, it->row, it->shifts[i]);
    }
}

void prepare_iter(world_t *world, gust_iter_t *it) {
    uint64_t i = 0;
    table_t **tables = cvector_data(&world->tables);
    if (it->actual == NULL) {
        for (; i < cvector_size(&world->tables); i++) {
            if (query_match_type(&it->query, &(tables[i])->type)) {
                it->actual = tables[i];
                it->row = 0;
                estimate_shifts(world, it->shifts, &it->query, it->actual);
                break;
            }
        }
    }
    i++;
    if (it->next == NULL) {
        for (; i < cvector_size(&world->tables); i++) {
            if (query_match_type(&it->query, &(tables[i])->type)) {
                it->next = tables[i];
                it->index = i;
                break;
            }
        }
    }
    put_data(it);
}

void gust_iter_get_next_table(world_t *world, gust_iter_t *it) {
    uint64_t i = it->index + 1;
    table_t **tables = cvector_data(&world->tables);
    for (; i < cvector_size(&world->tables); i++) {
        if (query_match_type(&it->query, &(tables[i])->type)) {
            it->next = tables[i];
            it->index = i;
            return;
        }
    }
    it->next = NULL;
}

int gust_iter_next(world_t *world, gust_iter_t *it) {
    if (it->actual == NULL) {
        return 0;
    }
    it->row++;
    if (it->row == it->actual->count) {
        it->actual = it->next;
        if (it->actual == NULL) {
            return 0;
        }
        it->row = 0;
        estimate_shifts(world, it->shifts, &it->query, it->actual);
        gust_iter_get_next_table(world, it);
    }
    put_data(it);
    return 1;
}

inline void *gust_get_data(gust_iter_t *it, uint64_t index) {
    return it->data[index];
}

inline void *gust_get_data_from_name(gust_iter_t *it, const char *name) {
    return gust_get_data(it, index_query(&it->query, gust_get_component_id(it->world, name)));
}
