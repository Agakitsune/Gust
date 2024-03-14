
#ifndef GUST_POOL_ALLOCATOR_H
#define GUST_POOL_ALLOCATOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct pool_t {
    uint64_t type_size;
    uint64_t pool_size;
    void *pool;
    void *free;
} pool_t;

int pool_init(pool_t *pool, uint64_t type_size, uint64_t per_pool);
void pool_fini(pool_t *pool);

void *pool_allocate(pool_t *pool);
void pool_free(pool_t *pool, void *ptr);

#define pool_init_type(pool, type, count_per_pool) pool_init((pool), sizeof(type), (count_per_pool))

#ifdef __cplusplus
}
#endif

#endif // GUST_POOL_ALLOCATOR_H