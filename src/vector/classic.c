
#include <stdlib.h>
#include <string.h>

#include "vector/classic.h"

cvector_t *cvector_init(
    const classic_t *allocator,
    cvector_t *vector,
    uint64_t base_count
) {
    uint64_t size = allocator->elem_size * base_count;
    size = NGMn(size, allocator->block_size);
    vector->data = malloc(size);
    if (vector->data == NULL) {
        return NULL;
    }
    vector->block_count = size / allocator->block_size;
    if (size % allocator->block_size)
        vector->block_count++;
    vector->count = 0;
    return vector;
}

void cvector_free(cvector_t *vector) {
    free(vector->data);
}

int cvector_add(
    const classic_t *allocator,
    cvector_t *vector,
    void *data
) {
    uint64_t shift = allocator->elem_size * vector->count;
    uint64_t size = shift + allocator->elem_size;
    uint64_t block = allocator->block_size * vector->block_count;
    if (size > block) {
        vector->block_count++;
        vector->data = realloc(vector->data, block + allocator->block_size);
    }
    if (vector->data == NULL)
        return -1;
    vector->count++;
    memcpy((char*)vector->data + shift, data, allocator->elem_size);
    return 0;
}

int cvector_add_range(
    const classic_t *allocator,
    cvector_t *vector,
    void *data,
    uint64_t count
) {
    uint64_t shift = allocator->elem_size * vector->count;
    uint64_t size = shift + allocator->elem_size * count;
    uint64_t block = allocator->block_size * vector->block_count;
    if (size > block) {
        vector->block_count++;
        vector->data = realloc(vector->data, block + allocator->block_size);
    }
    if (vector->data == NULL)
        return -1;
    vector->count += count;
    memcpy((char*)vector->data + shift, data, allocator->elem_size * count);
    return 0;
}

int cvector_insert(const classic_t *allocator, cvector_t *vector, uint64_t index, void *data) {
    if (vector->count <= index)
        return -1;
    uint64_t shift = allocator->elem_size * index;
    uint64_t size = shift + allocator->elem_size;
    uint64_t block = allocator->block_size * vector->block_count;
    if (size > block) {
        vector->block_count++;
        vector->data = realloc(vector->data, block + vector->block_count);
    }
    if (vector->data == NULL)
        return -1;
    vector->count++;
    uint64_t rest = allocator->elem_size * (vector->count - (index + 1));
    char *shifted = (char *)vector->data + shift;
    if (rest)
        memcpy(shifted + allocator->elem_size, shifted, rest);
    memcpy(shifted, data, allocator->elem_size);
    return 0;
}

int cvector_insert_range(const classic_t *allocator, cvector_t *vector, uint64_t index, void *data, uint64_t count) {
    if (vector->count <= index)
        return -1;
    uint64_t shift = allocator->elem_size * index;
    uint64_t size = shift + allocator->elem_size * count;
    uint64_t block = allocator->block_size * vector->block_count;
    if (size > block) {
        vector->block_count++;
        vector->data = realloc(vector->data, block + vector->block_count);
    }
    if (vector->data == NULL)
        return -1;
    vector->count += count;
    uint64_t rest = allocator->elem_size * (vector->count - (index + count));
    char *shifted = (char *)vector->data + shift;
    if (rest)
        memcpy(shifted + allocator->elem_size * count, shifted, rest);
    memcpy(shifted, data, allocator->elem_size * count);
    return 0;
}

int cvector_remove(
    const classic_t *allocator,
    cvector_t *vector,
    uint64_t index
) {
    if (vector->count <= index)
        return -1;
    uint64_t shift = allocator->elem_size * index;
    uint64_t rest = allocator->elem_size * (vector->count - (index + 1));
    char *shifted = (char *)vector->data + shift;
    memset(shifted, 0, allocator->elem_size);
    if (rest)
        memcpy(shifted, shifted + allocator->elem_size, rest);
    vector->count--;
    return 0;
}

int cvector_remove_range(
    const classic_t *allocator,
    cvector_t *vector,
    uint64_t index,
    uint64_t count
) {
    if (vector->count <= index && vector->count <= (index + count))
        return -1;
    uint64_t shift = allocator->elem_size * index;
    uint64_t rest = allocator->elem_size * (vector->count - (index + count));
    char *shifted = (char *)vector->data + shift;
    memset(shifted, 0, allocator->elem_size * count);
    if (rest)
        memcpy(shifted, shifted + allocator->elem_size * count, rest);
    vector->count -= count;
    return 0;
}

void *cvector_data(const cvector_t *vector) {
    return vector->data;
}

uint64_t cvector_size(const cvector_t *vector) {
    return vector->count;
}

void *cvector_get_index(const classic_t *allocator, const cvector_t *vector, uint64_t index) {
    if (vector->count <= index)
        return NULL;
    uint64_t shift = allocator->elem_size * index;
    char *shifted = (char*)vector->data + shift;
    return shifted;
}
