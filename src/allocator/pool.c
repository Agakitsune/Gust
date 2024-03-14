
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include "allocator/pool.h"

void pool_populate(pool_t *pool, char *data) {
    pool->free = data;
    char *p = data;
    for (uint64_t i = 0; i < pool->pool_size; i += pool->type_size) {
        *(char **)p = p + pool->type_size;
        p += pool->type_size;
    }
    p -= pool->type_size;
    *(char **)p = NULL;

    if (pool->pool == NULL) {
        pool->pool = data - 8;
    } else {
        p = data - 8;
        *(char **)p = pool->pool;
        pool->pool = p;
    }
}

int pool_init(pool_t *pool, uint64_t type_size, uint64_t per_pool) {
    if (type_size < 8)
        return -1;
    pool->type_size = type_size;
    pool->pool_size = type_size * per_pool;
    char *head = malloc(8 + pool->pool_size);
    if (head == NULL)
        return -1;
    memset(head, 0, 8 + pool->pool_size);
    pool_populate(pool, head + 8);
    return 0;
}

void pool_fini(pool_t *pool) {
    char *p = pool->pool;
    while (p != NULL) {
        char *next = *(char **)p;
        free(p);
        p = next;
    }
    pool->pool = NULL;
    pool->free = NULL;
    pool->type_size = 0;
    pool->pool_size = 0;
}

void *pool_allocate(pool_t *pool) {
    if (pool->free == NULL) {
        char *head = malloc(8 + pool->pool_size);
        if (head == NULL)
            return NULL;
        memset(head, 0, 8 + pool->pool_size);
        pool_populate(pool, head + 8);
    }
    void *ret = pool->free;
    pool->free = *(char **)pool->free;
    return ret;
}

void pool_free(pool_t *pool, void *ptr) {
    *(char **)ptr = pool->free;
    pool->free = ptr;
}
