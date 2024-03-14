
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>

#include "type.h"
#include "component.h"

int new_type(type_t *type, entity_t component) {
    type->ids = malloc(sizeof(entity_t));
    if (type->ids == NULL) {
        return -1;
    }
    type->ids[0] = component;
    type->count = 1;
    return 0;
}

int copy_type(type_t *dest, const type_t *src) {
    dest->ids = malloc(src->count * sizeof(entity_t));
    if (dest->ids == NULL) {
        return -1;
    }
    dest->count = src->count;
    for (uint64_t i = 0; i < src->count; i++) {
        dest->ids[i] = src->ids[i];
    }
    return 0;
}

int merge_type(type_t *dest, const type_t *src) {
    uint64_t count = dest->count + src->count;
    entity_t *ids = malloc(count * sizeof(entity_t));
    if (ids == NULL) {
        return -1;
    }
    uint64_t i = 0;
    for (; i < dest->count; i++) {
        ids[i] = dest->ids[i];
    }
    for (uint64_t j = 0; j < src->count; j++) {
        ids[i + j] = src->ids[j];
    }
    free(dest->ids);
    dest->ids = ids;
    dest->count = count;
    return 0;
}

int add_type(type_t *type, entity_t component) {
    uint64_t insert = -1;
    for (uint64_t i = 0; i < type->count; i++) {
        if (type->ids[i] == component) {
            return 0;
        }
        if (component < type->ids[i]) {
            insert = i;
            break;
        }
    }
    entity_t *ids = realloc(type->ids, (type->count + 1) * sizeof(entity_t));
    if (ids == NULL) {
        return -1;
    }
    if (insert == (uint64_t)-1) {
        ids[type->count] = component;
    } else {
        for (uint64_t i = type->count; i > insert; i--) {
            ids[i] = ids[i - 1];
        }
        ids[insert] = component;
    }
    type->ids = ids;
    type->count++;
    return 0;
}

int remove_type(type_t *type, entity_t component) {
    for (uint64_t i = 0; i < type->count; i++) {
        if (type->ids[i] == component) {
            for (uint64_t j = i; j < type->count - 1; j++) {
                type->ids[j] = type->ids[j + 1];
            }
            type->count--;
            return 0;
        }
    }
    return -1;
}

int has_type(const type_t *type, entity_t component) {
    for (uint64_t i = 0; i < type->count; i++) {
        if (type->ids[i] == component) {
            return 1;
        }
    }
    return 0;
}

int compare_type(const type_t *a, const type_t *b) {
    if (a->count != b->count) {
        return 0;
    }
    for (uint64_t i = 0; i < a->count; i++) {
        if (a->ids[i] != b->ids[i]) {
            return 0;
        }
    }
    return 1;
}

uint64_t index_type(const type_t *type, entity_t component) {
    for (uint64_t i = 0; i < type->count; i++) {
        if (type->ids[i] == component) {
            return i;
        }
    }
    return -1;
}

inline void free_type(type_t *type) {
    free(type->ids);
}

void display_type(const type_t *type) {
    printf("Type: ");
    for (uint64_t i = 0; i < type->count; i++) {
        printf("%lu ", type->ids[i]);
    }
    printf("\n");
}

uint64_t shift_type(const world_t *world, const type_t *type, uint64_t id) {
    uint64_t shift = 0;
    for (uint64_t i = 0; i < type->count; i++) {
        if (type->ids[i] == id) {
            return shift;
        }
        shift += gust_get_component_size(world, type->ids[i]);
    }
    return -1;
}
