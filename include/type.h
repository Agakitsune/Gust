
#ifndef GUST_TYPE_H
#define GUST_TYPE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "entity.h"

typedef struct type_t {
    entity_t *ids;
    uint64_t count;
} type_t;

int new_type(type_t *type, entity_t component);
int copy_type(type_t *dest, const type_t *src);
int merge_type(type_t *dest, const type_t *src);
int add_type(type_t *type, entity_t component);
int remove_type(type_t *type, entity_t component);
int has_type(const type_t *type, entity_t component);
int compare_type(const type_t *a, const type_t *b);
uint64_t index_type(const type_t *type, entity_t component);
void free_type(type_t *type);
void display_type(const type_t *type);

#ifdef __cplusplus
}
#endif

#endif // GUST_TYPE_H
