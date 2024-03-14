
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include "query.h"

int add_query(query_t *query, entity_t component) {
    entity_t *ids = realloc(query->ids, (query->count + 1) * sizeof(entity_t));
    if (ids == NULL) {
        return -1;
    }
    ids[query->count] = component;
    query->ids = ids;
    query->count++;
    return 0;
}

void display_query(const query_t *query) {
    printf("Query: ");
    for (uint64_t i = 0; i < query->count; i++) {
        printf("%lu ", query->ids[i]);
    }
    printf("\n");
}

int query_match_type(const query_t *query, const type_t *type) {
    for (uint64_t i = 0; i < query->count; i++) {
        if (!has_type(type, query->ids[i])) {
            return 0;
        }
    }
    return 1;
}
