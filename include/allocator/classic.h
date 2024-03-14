
#ifndef GUST_CLASSIC_ALLOCATOR_H
#define GUST_CLASSIC_ALLOCATOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct classic_t {
    uint64_t elem_size;
    uint64_t block_size;
} classic_t;

#define classic_init(type, block) (classic_t){sizeof(type), block}

#ifdef __cplusplus
}
#endif

#endif // GUST_CLASSIC_ALLOCATOR_H