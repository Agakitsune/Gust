
#ifndef GUST_SYSTEM_H
#define GUST_SYSTEM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "utils.h"
#include "iter.h"

// typedef struct world_t world_t;

typedef struct system_t {
    void (*system)(gust_iter_t*);
} system_t;

int gust_generate_query(world_t *world, query_t *query, const char *str);
entity_t gust_declare_system(world_t *world, void (*system)(gust_iter_t*), const char *query);
entity_t gust_declare_system_w_query(world_t *world, void (*system)(gust_iter_t*), query_t *query);
entity_t gust_new_system(world_t *world, void (*system)(gust_iter_t*));

system_t *gust_get_system(world_t *world, entity_t system);

#define GUST_SYSTEM(world, func, ...) gust_declare_system((world), (func), #__VA_ARGS__)

#define GUST_SYSTEM_BIT 9
#define GUST_SYSTEM_MASK MASK(GUST_SYSTEM_BIT)
#define GUST_SYSTEM_PAGE_SIZE PAGE_SIZE(GUST_SYSTEM_BIT)

#ifdef __cplusplus
}
#endif

#endif // GUST_SYSTEM_H
