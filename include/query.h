
#ifndef GUST_QUERY_H
#define GUST_QUERY_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "type.h"

typedef struct query_t {
    entity_t *ids;
    uint64_t count;
} query_t;

#define new_query(query, component) new_type((type_t*)(query), (component))
#define copy_query(dest, src) copy_type((type_t*)(dest), (const type_t*)(src))
int add_query(query_t *query, entity_t component);
#define remove_query(query, component) remove_type((type_t*)(query), (component))
#define has_query(query, component) has_type((const type_t*)(query), (component))
#define compare_query(a, b) compare_type((const type_t*)(a), (const type_t*)(b))
#define index_query(query, component) index_type((const type_t*)(query), (component))
#define free_query(query) free_type((type_t*)(query))
void display_query(const query_t *query);
int query_match_type(const query_t *query, const type_t *type);
#define shift_query(world, query, id) shift_type((world), (type_t*)(query), (id))

#ifdef __cplusplus
}
#endif

#endif // GUST_QUERY_H
